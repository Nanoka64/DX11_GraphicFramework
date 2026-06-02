#include "pch.h"
#include "Component_PlayerController.h"
#include "Component_3DCamera.h"
#include "Component_SkinnedMeshAnimator.h"
#include "Component_WeaponController.h"
#include "Component_Health.h"
#include "Component_Physics.h"
#include "GameObject.h"
#include "InputFactory.h"
#include "DirectWriteManager.h"
#include "RendererEngine.h"
#include "CollisionInfo.h"

using namespace Input;
using namespace VECTOR3;
using namespace VECTOR2;

using namespace PlayerData;

using namespace DirectX;
using namespace Tool;

constexpr float MOVE_SPEED = 150.0f;	// プレイヤーの移動速度
constexpr float ROLLING_SPEED = 32.0f;	// ローリング時初速
constexpr float ROLLING_DURATION = 1.0f;// ローリング時間
constexpr float JUMP_HEIGHT = 2.5f;		// ジャンプの高さ
constexpr float GRAVITY = 18.0f;		// 重力
constexpr float ANIM_SPEED = 1.25f;		// アニメーション速度

//*---------------------------------------------------------------------------------------
//*【?】コンストラクタ
//* [引数]
//* std::weak_ptr<GameObject> : オーナーオブジェクト
//*	updateRank : 更新順番
//*----------------------------------------------------------------------------------------
PlayerController::PlayerController(std::weak_ptr<GameObject> pOwner, int updateRank) : IComponent(pOwner, updateRank),
m_pCameraComp(),
m_IsAnim(false),
m_IsJump(false),
m_IsDead(false),
m_IsRolling(false),
m_IsGrounded (true),
m_IsContinuousAngle(true),
m_MoveVelocity(VEC3()),
m_MoveSpeed(MOVE_SPEED),
m_CrntAnimID(PLAYER_RANGER_ANIM_ID::RIFLE_AMING_IDLE),
m_JumpVelocity(0.0f),
m_Gravity(GRAVITY),
m_JumpForce(0.0f),
m_RollingElapsedTime(0.0f),
m_RollingDuration(ROLLING_DURATION)
{
	this->set_Tag("PlayerController");
}

//*---------------------------------------------------------------------------------------
//*【?】デストラクタ
//*----------------------------------------------------------------------------------------
PlayerController::~PlayerController()
{

}

//*---------------------------------------------------------------------------------------
//*【?】初期化
//*
//* [引数]
//* RendererEngine& : 描画エンジンの参照
//*
//* [返値]
//* void
//*----------------------------------------------------------------------------------------
void PlayerController::Start(RendererEngine& renderer)
{
	// カメラコンポーネントの取得
	auto obj = Master::m_pGameObjectManager->get_ObjectByTag("Camera");
	
	if (obj == nullptr) 
	{
		assert(false);
	}
	
	// カメラコンポーネント
	m_pCameraComp = obj->get_Component<Camera3D>();

	// アニメーションコンポーネントの取得
	m_pAnimatorComp = m_pOwner.lock()->get_Component<SkinnedMeshAnimator>();

	//武器制御用コンポーネントの取得
	m_pWeaponController = m_pOwner.lock()->get_Component<WeaponController>();

	m_CrntAnimID = PLAYER_RANGER_ANIM_ID::RIFLE_AMING_IDLE;

	m_pPhysicsComp = m_pOwner.lock()->get_Component<Physics>();
	m_pPhysicsComp.lock()->set_MaxSpeed(10.0f);
	m_pPhysicsComp.lock()->set_GravityScale(14.0f);
	m_pPhysicsComp.lock()->set_Restitution(0.0f);

	// HP管理コンポーネントの取得
	m_pHealthComp = m_pOwner.lock()->get_Component<Health>();
	//m_pHealthComp.lock()->set_MaxHP(200.0f);
	//m_pHealthComp.lock()->set_CrntHP(200.0f);

	// ジャンプ力の計算
	// https://heron-no-suugaku.sakura.ne.jp/jump-implementation-math/#toc1
	m_JumpForce = sqrtf(2.0f * m_Gravity * JUMP_HEIGHT);
	m_JumpForce = 8.0f;

	// 被弾時のコールバック
	m_pHealthComp.lock()->RegisterOnDamage(
		[this] (float _damage)
		{
			//ChangeAnimation(PLAYER_ANIMATION_ID::HIT_HEAD); 
		}
	);	
	// 死亡時のコールバック
	m_pHealthComp.lock()->RegisterOnDead(
		[this] 
		{
			m_IsDead = true;
			//ChangeAnimation(PLAYER_ANIMATION_ID::DEATH01); 
		}
	);
}

//*---------------------------------------------------------------------------------------
//*【?】更新
//*
//* [引数]
//* RendererEngine& : 描画エンジンの参照
//*
//* [返値]
//* void
//*----------------------------------------------------------------------------------------
void PlayerController::Update(RendererEngine &renderer)
{
	m_IsGrounded = false;
}

//*---------------------------------------------------------------------------------------
//*【?】遅延更新
//*
//* [引数]
//* RendererEngine& : 描画エンジンの参照
//*
//* [返値]
//* void
//*----------------------------------------------------------------------------------------
void PlayerController::LateUpdate(RendererEngine& renderer)
{
	// プレイヤー死亡
	if (m_IsDead) {
		Master::m_pDataManager->set_IsPlayerDead(true);
		return;
	}

	m_pAnimatorComp.lock()->PlayAnim(Master::m_pTimeManager->get_DeltaTime() * ANIM_SPEED);

	// ローリングの処理のみ行って返す
	if (m_IsRolling)
	{
		RollingUpdate();
		return;
	}

	// 武器制御の更新
	//m_pWeaponController.lock()->Update(renderer);

	float deltaTime = Master::m_pTimeManager->get_DeltaTime();
	auto physics = m_pPhysicsComp.lock();

	auto pOwner = m_pOwner.lock();
	VEC3 upVec = VEC3(0.0f, 1.0f, 0.0f);					// カメラから取得
	float angle_H = m_pCameraComp.lock()->get_Angle_H();	// 水平アングル取得
	m_pMyTransformComp = pOwner->get_Transform().lock();

	VEC3 newPos = VEC3();				// 新しく入れる座標
	VEC3 crntPos = m_pMyTransformComp.lock()->get_VEC3ToPos();			// 現在の座標
	VEC3 crntRot = m_pMyTransformComp.lock()->get_VEC3ToRotateToRad();	// 現在の回転

	// 待機アニメーション
	ChangeAnimation(PLAYER_RANGER_ANIM_ID::RIFLE_AMING_IDLE);

	// 前方向と右方向ベクトルを作る 
	// 右方向ベクトルは上方向と前方向ベクトルの外積を取ることでできる
	// カメラ側で「プレイヤー → カメラ」の処理にしているため、
	// 反転させている
	VEC3 forward = VEC3();
	forward.x = -cosf(angle_H);
	forward.y = 0.0f;
	forward.z = -sinf(angle_H);
	forward = forward.Normalize();

	// 右方向ベクトル
	VEC3 right = VEC3::Cross(upVec, forward);

	// 正規化
	forward = forward.Normalize();
	right = right.Normalize();

	/*VEC3 velocity{ 0.0f,0.0f,0.0f };*/
	VEC3 inputDir = VEC3(0.0f, 0.0f, 0.0f);
	m_MoveVelocity.x = 0.0f;
	m_MoveVelocity.z = 0.0f;


	//-----------------------------------------------------------------------------
	// ■ 移動入力 / ジャンプ時はローリングさせない
	//-----------------------------------------------------------------------------
	if (GetInput(GAME_CONFIG::MOVE_FORWARD))	// 前進
	{
		inputDir += forward;
	}
	if (GetInput(GAME_CONFIG::MOVE_BACK))		// 後退
	{
		inputDir -= forward;
	}
	if (GetInput(GAME_CONFIG::MOVE_RIGHT))		// 右へ
	{
		inputDir += right;

		// 右ローリング
		if (GetInputDown(GAME_CONFIG::MOVE_JUMP) && m_IsJump == false)
		{
			inputDir = inputDir.Normalize();
			m_MoveVelocity.x = inputDir.x;
			m_MoveVelocity.z = inputDir.z;


			m_IsRolling = true;
			// ****************************************************
			//				 ローリング開始音/声 再生
			// ****************************************************
			Master::m_pSoundManager->Play(SOUND_TYPE::SE, SOUND_ID_TO_INT(SOUND_ID::SOLDIER_R_JUMP_IN));
			Master::m_pSoundManager->Play_Rand(SOUND_TYPE::VOICE, SOUND_ID_TO_INT(VOICE_ID::SOLDIER_R_SHOUT_01), 3);
			return;
		}
	}
	if (GetInput(GAME_CONFIG::MOVE_LEFT))		// 左へ
	{
		inputDir -= right;

		// 左ローリング
		if (GetInputDown(GAME_CONFIG::MOVE_JUMP) && m_IsJump == false)
		{
			inputDir = inputDir.Normalize();
			m_MoveVelocity.x = inputDir.x;
			m_MoveVelocity.z = inputDir.z;


			m_IsRolling = true;
			// ****************************************************
			//				 ローリング開始音/声 再生
			// ****************************************************
			Master::m_pSoundManager->Play(SOUND_TYPE::SE, SOUND_ID_TO_INT(SOUND_ID::SOLDIER_R_JUMP_IN));
			Master::m_pSoundManager->Play_Rand(SOUND_TYPE::VOICE, SOUND_ID_TO_INT(VOICE_ID::SOLDIER_R_SHOUT_01), 3);

			return;
		}
	}

	// 正規化
	inputDir = inputDir.Normalize();
	m_MoveVelocity.x = inputDir.x;
	m_MoveVelocity.z = inputDir.z;

	//-----------------------------------------------------------------------------
	// ■ ジャンプ入力受付 / ジャンプ中処理
	//-----------------------------------------------------------------------------
	// 接地しているならジャンプ可能
	if (m_IsGrounded)
	{
		// ジャンプ中に接地した場合、リセット
		if (m_IsJump)
		{
			// ****************************************************
			//				 ジャンプ - 着地音 再生
			// ****************************************************
			Master::m_pSoundManager->Play(SOUND_TYPE::SE, SOUND_ID_TO_INT(SOUND_ID::SOLDIER_R_JUMP_LAND));
			ChangeAnimation(PLAYER_RANGER_ANIM_ID::JUMP_DOWN);

			m_JumpVelocity = 0.0f;
			m_IsJump = false;
		}

		// ジャンプ状態ではない、かつ、ローリング中でもない
		if (GetInputDown(GAME_CONFIG::MOVE_JUMP) && m_IsRolling == false)
		{
			m_IsJump = true;
			//m_JumpVelocity = m_JumpForce;

			// 上方向に衝撃を加える
			physics->AddImpulse(VEC3(0.0f, m_JumpForce, 0.0f));

			// ****************************************************
			//				 ジャンプ開始音/声 再生
			// ****************************************************
			Master::m_pSoundManager->Play(SOUND_TYPE::SE, SOUND_ID_TO_INT(SOUND_ID::SOLDIER_R_JUMP_IN));
			Master::m_pSoundManager->Play_Rand(SOUND_TYPE::VOICE, SOUND_ID_TO_INT(VOICE_ID::SOLDIER_R_SHOUT_01), 3);

			// ジャンプ開始アニメーション
			ChangeAnimation(PLAYER_RANGER_ANIM_ID::JUMP_UP);
		}
	}
	else
	{
		// 空中にいる場合は重力をかけ続ける
		ChangeAnimation(PLAYER_RANGER_ANIM_ID::JUMP_LOOP);
		//m_JumpVelocity -= m_Gravity * deltaTime;

		// 世界の裏側に落下した場合
		if (crntPos.y < -100.0f)
		{
			m_IsJump = false;
			m_JumpVelocity = 0.0f;
			m_pMyTransformComp.lock()->set_Pos(VEC3(0.0f, 100.0f, 0.0f));
			return;
		}
	}
	// 正規化後に入れる
	m_MoveVelocity.y = m_JumpVelocity;

	m_IsAnim = false;

	float ang = 0.0f;

	//-----------------------------------------------------------------------------
	// ■ velocityをもとに実際に移動させ、回転も計算する
	//-----------------------------------------------------------------------------
	if (m_MoveVelocity.Length() > 0.001f)
	{
		// 水平方向の移動ベクトル
		VEC3 horizontalMove = m_MoveVelocity;
		horizontalMove.y = 0;

		VEC3 moveForce = horizontalMove * m_MoveSpeed;

		// 移動計算
		//newPos = (crntPos + (horizontalMove * m_MoveSpeed * deltaTime) + (VEC3(0, m_JumpVelocity, 0) * deltaTime));


		// 動いているならアニメーション
		m_IsAnim = true;

		// =========================================================
		// 空中にいるときは加える力を弱くする
		// =========================================================
		if (m_IsGrounded == false)
		{
			// 空中では地上より力を10%に抑える
			// もっと空中で動かしたい場合は 0.5f などに調整してください
			moveForce *= 0.2f;
		}

		physics->AddForce(moveForce);

		/*
		// 移動ベクトルに合わせてY軸のみ回転させる
		// ジャンプ時に回転しないよう、X/Zのみ考慮
		*/
		if (horizontalMove.Length() > 0.001f)
		{
			//!***********************************
			// 方向の処理を外に出すとガタガタする
			//!***********************************
			float targetAngle = 0.0f;      //目標角度

			// ジャンプ中でないなら走る
			if (m_IsJump == false)
			{
				// 走りアニメーション
				ChangeAnimation(PLAYER_RANGER_ANIM_ID::RUNING);
			}

			if (!m_IsContinuousAngle)
			{
				MovedAngle(crntRot, m_MoveVelocity);
			}
		}

		//m_pMyTransformComp.lock()->set_Pos(newPos);
	}


	if (m_pWeaponController.lock()->get_IsCrntWeaponReloading())
	{

		if (m_MoveVelocity.Length() > 0.001f) {
			ChangeAnimation(PLAYER_RANGER_ANIM_ID::RUNNNING_RELOAD);
		}
		else {
			ChangeAnimation(PLAYER_RANGER_ANIM_ID::RELOADING_IDLE);
		}
	}


	// 継続的に視点を変える
	if (m_IsContinuousAngle)
	{
		ContinuousAngle(crntRot);
	}
}

//*---------------------------------------------------------------------------------------
//*【?】ローリング処理
//*		目標までの移動ベクトルが必要		
//*		別に目標に到達できなくていい
//*		時間で終わらせる
//* 
//* 
//* [引数]なし
//* [返値]なし
//*----------------------------------------------------------------------------------------
void PlayerController::RollingUpdate()
{
	// ローリングアニメーションに
	ChangeAnimation(PLAYER_RANGER_ANIM_ID::RUNING_DIVE_ROLL);

	float deltaTime = Master::m_pTimeManager->get_DeltaTime();
	

	auto pOwner = m_pOwner.lock();
	m_pMyTransformComp = pOwner->get_Transform().lock();

	VEC3 crntRot = m_pMyTransformComp.lock()->get_VEC3ToRotateToRad();
	VEC3 crntPos = m_pMyTransformComp.lock()->get_VEC3ToPos();	// 現在の位置
	VEC3 newPos = VEC3();	// 新しい位置

	// 0.0 ～ 1.0 に正規化
	float t = std::min(
		m_RollingElapsedTime / m_RollingDuration,
		1.0f); 

	// ※ イージングより、経過時間の割合の方が、本家の挙動に似ている感じがするので、変更
	// イージング関数でカウンタに合わせて速度を落としていく
	//float easeOut = 1.0f - Tool::Easing::EaseOutSin(t);
	newPos = crntPos + (m_MoveVelocity * (ROLLING_SPEED * (1.0f - t))) * deltaTime;

	
	m_RollingElapsedTime += deltaTime;

	// 時間で止める
	if (t >= 1.0f)
	{
		/* 移動ベクトルとかを元に戻す */
		m_IsRolling = false;
		m_RollingElapsedTime = 0.0f;

		// ****************************************************
		//				 ローリング終了音再生
		// ****************************************************
		Master::m_pSoundManager->Play(SOUND_TYPE::SE, SOUND_ID_TO_INT(SOUND_ID::SOLDIER_R_JUMP_LAND));
	}
	
	m_pMyTransformComp.lock()->set_Pos(newPos);

	// ローリング方向に合わせて回転させる
	MovedAngle(crntRot,m_MoveVelocity);
}


//*---------------------------------------------------------------------------------------
//*【?】衝突処理
//*
//* [引数]
//* &other : 衝突相手の情報
//*
//* [返値]
//* void
//*----------------------------------------------------------------------------------------
void PlayerController::OnCollisionEnter(const class CollisionInfo &other)
{
	if (other.get_HitObject().lock()->get_Tag() == "Ant")
	{
		//m_pHealthComp.lock()->TakeDamage(1.0f);
	}

	VEC3 normal = other.get_HitNormal();

	// 法線のY成分が一定以上（例：0.7f以上で約45度以下の坂）なら床とみなす
	if (normal.y < -0.7f)
	{
		m_IsGrounded = true;

		// めり込み防止のため、下方向への速度（重力による落下速度）をリセット
		if (m_JumpVelocity < 0.0f) {
			m_JumpVelocity = 0.0f;
		}
	}
}

//*---------------------------------------------------------------------------------------
//*【?】パラメータ等をリセットする
//*
//* [引数] なし
//* [返値] なし
//*----------------------------------------------------------------------------------------
void PlayerController::Reset()
{
	m_IsDead = false;
	m_IsJump = false;
	m_IsRolling = false;
	if (m_pHealthComp.lock()) {
		m_pHealthComp.lock()->Reset();
	}
	if (m_pAnimatorComp.lock())	{
		ChangeAnimation(PlayerData::PLAYER_RANGER_ANIM_ID::RIFLE_AMING_IDLE);
	}
}

//*---------------------------------------------------------------------------------------
//*【?】アニメーションの切り替え
//*
//* [引数]
//* id : アニメーション番号
//* [返値]なし
//*----------------------------------------------------------------------------------------
void PlayerController::ChangeAnimation(PlayerData::PLAYER_RANGER_ANIM_ID id)
{
	// 同じ又はアニメーションが止まっているなら返す
	if (id == m_CrntAnimID)
	{
		return;
	}

	auto animComp = m_pAnimatorComp.lock();

	// ひとつ前のアニメーションIDセット
	animComp->set_PrevAnimIndex(static_cast<int>(m_CrntAnimID));

	m_CrntAnimID = id;

	// 現在のアニメーションIDセット
	animComp->set_AnimIndex(static_cast<int>(m_CrntAnimID));

	if (m_CrntAnimID == PLAYER_RANGER_ANIM_ID::RUNING_DIVE_ROLL)
	{
		animComp->set_AnimProcTime(0.0f);
		animComp->set_ShadowAnimProcTime(0.0f);
	}
}

//*---------------------------------------------------------------------------------------
//*【?】マウスに合わせて継続的に回転する
//*
//* [引数]なし
//* [返値]なし
//*----------------------------------------------------------------------------------------
void PlayerController::ContinuousAngle(const VECTOR3::VEC3 &_crntRot)
{
	float angle_H = m_pCameraComp.lock()->get_Angle_H();	// 水平アングル取得
	float angle_V = m_pCameraComp.lock()->get_Angle_V();	// 垂直アングル取得

	POINT mousePos = Master::m_pInputManager->GetMousePos();
	float targetAngleY = (angle_H + 1.57f) * -1.0f;

	// 目標とするクォータニオン
	XMVECTOR targetRotQ = XMQuaternionRotationRollPitchYaw(0.0f, targetAngleY, 0.0f);

	XMVECTOR crntRotQ = m_pMyTransformComp.lock()->get_RotationQuaternion();

	// クォータニオンの球面線形補間
	// 普通の線形補間だと、値が飛んでしまうためクォータニオンの場合は球面線形補間を使う
	XMVECTOR newRotQ = XMQuaternionSlerp(crntRotQ, targetRotQ, 0.5f);

	m_pMyTransformComp.lock()->set_RotationQuaternion(newRotQ);
}

//*---------------------------------------------------------------------------------------
//*【?】移動方向に回転する
//*
//* [引数]なし
//* [返値]なし
//*----------------------------------------------------------------------------------------
void PlayerController::MovedAngle(const VECTOR3::VEC3 &_crntRot, const VECTOR3::VEC3 &_velocity)
{
	//目標の方向ベクトルから角度値を算出c
	float targetAngleY = atan2(_velocity.x, _velocity.z);
	//targetAngleY -= 3.14f;	// ※ プレイヤーモデルが前後反転してしまっているため  追記：直した

	// 目標とするクォータニオン
	XMVECTOR targetRotQ = XMQuaternionRotationRollPitchYaw(0.0f, targetAngleY, 0.0f);
	
    XMVECTOR crntRotQ = m_pMyTransformComp.lock()->get_RotationQuaternion();

	// クォータニオンの球面線形補間
	// 普通の線形補間だと、値が飛んでしまうためクォータニオンの場合は球面線形補間を使う
    XMVECTOR newRotQ = XMQuaternionSlerp(crntRotQ, targetRotQ, 0.5f);

	m_pMyTransformComp.lock()->set_RotationQuaternion(newRotQ);
}

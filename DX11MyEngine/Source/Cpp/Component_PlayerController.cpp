#include "pch.h"
#include "Component_PlayerController.h"
#include "Component_3DCamera.h"
#include "Component_SkinnedMeshAnimator.h"
#include "Component_Health.h"
#include "GameObject.h"
#include "InputFactory.h"
#include "DirectWriteManager.h"
#include "RendererEngine.h"
#include "CollisionInfo.h"

using namespace Input;
using namespace VECTOR3;
using namespace VECTOR2;

using namespace DirectX;
using namespace Tool;

constexpr float MOVE_SPEED = 3.0f;		// プレイヤーの移動速度
constexpr float ROLLING_SPEED = 10.0f;	// ローリング時のスピード
constexpr int ROLLING_DURATION = 60;	// ローリング時間

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
m_IsContinuousAngle(true),
m_MoveVelocity(VEC3()),
m_MoveSpeed(MOVE_SPEED),
m_CrntAnimID(PLAYER_ANIMATION_ID::T_POSE),
m_JumpVelocity(0.0f),
m_RollingCounter(0),
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

	m_CrntAnimID = PLAYER_ANIMATION_ID::CROUCH_FWD_LOOP;

	// HP管理コンポーネントの取得
	m_pHealthComp = m_pOwner.lock()->get_Component<Health>();
	m_pHealthComp.lock()->set_MaxHP(200.0f);
	m_pHealthComp.lock()->set_CrntHP(200.0f);

	// 被弾時のコールバック
	m_pHealthComp.lock()->RegisterOnDamage(
		[this] (float _damage)
		{
			ChangeAnimation(PLAYER_ANIMATION_ID::HIT_HEAD); 
		}
	);	
	// 死亡時のコールバック
	m_pHealthComp.lock()->RegisterOnDead(
		[this] 
		{
			m_IsDead = true;
			ChangeAnimation(PLAYER_ANIMATION_ID::DEATH01); 
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
	if (m_IsDead)return;
	
	// ローリングの処理のみ行って返す
	if (m_IsRolling)
	{
		RollingUpdate();
		return;
	}

	auto pOwner = m_pOwner.lock();
	VEC3 upVec = VEC3(0.0f, 1.0f, 0.0f);					// カメラから取得
	float angle_H = m_pCameraComp.lock()->get_Angle_H();	// 水平アングル取得
	m_pMyTransformComp = pOwner->get_Transform().lock();

	VEC3 newPos = VEC3();				// 新しく入れる座標
	VEC3 crntPos = m_pMyTransformComp.lock()->get_VEC3ToPos();			// 現在の座標
	VEC3 crntRot = m_pMyTransformComp.lock()->get_VEC3ToRotateToRad();	// 現在の回転

	// 待機アニメーション
	ChangeAnimation(PLAYER_ANIMATION_ID::IDLE_LOOP);

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

	m_MoveVelocity.x = 0.0f;
	m_MoveVelocity.z = 0.0f;

	//-----------------------------------------------------------------------------
	// ■ 移動入力 / ジャンプ時はローリングさせない
	//-----------------------------------------------------------------------------
	if (GetInput(GAME_CONFIG::MOVE_FORWARD))	// 前進
	{
		m_MoveVelocity += forward;
	}
	if (GetInput(GAME_CONFIG::MOVE_BACK))		// 後退
	{
		m_MoveVelocity -= forward;
	}
	if (GetInput(GAME_CONFIG::MOVE_RIGHT))		// 右へ
	{ 
		m_MoveVelocity += right;

		// 右ローリング
		if (GetInputDown(GAME_CONFIG::MOVE_JUMP) && m_IsJump == false)	
		{
			m_MoveVelocity = m_MoveVelocity.Normalize();
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
		m_MoveVelocity -= right;

		// 左ローリング
		if (GetInputDown(GAME_CONFIG::MOVE_JUMP) && m_IsJump == false)
		{
			m_MoveVelocity = m_MoveVelocity.Normalize();
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
	m_MoveVelocity = m_MoveVelocity.Normalize();

	//-----------------------------------------------------------------------------
	// ■ ジャンプ入力受付 / ジャンプ中処理
	//-----------------------------------------------------------------------------
	// ジャンプ状態ではない、かつ、ローリング中でもない
	if (m_IsJump == false && m_IsRolling == false)
	{
		if (GetInputDown(GAME_CONFIG::MOVE_JUMP))
		{
			m_IsJump = true;
			m_JumpVelocity = m_JumpForce;

			// ****************************************************
			//				 ジャンプ開始音/声 再生
			// ****************************************************
			Master::m_pSoundManager->Play(SOUND_TYPE::SE, SOUND_ID_TO_INT(SOUND_ID::SOLDIER_R_JUMP_IN));
			Master::m_pSoundManager->Play_Rand(SOUND_TYPE::VOICE, SOUND_ID_TO_INT(VOICE_ID::SOLDIER_R_SHOUT_01), 3);

			// ジャンプ開始アニメーション
			ChangeAnimation(PLAYER_ANIMATION_ID::JUMP_START);
		}
	}
	else
	{
		// ジャンプ中アニメーション
		ChangeAnimation(PLAYER_ANIMATION_ID::JUMP_LOOP);
		m_JumpVelocity -= m_Gravity;	// 重力
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
		//m_MoveVelocity = m_MoveVelocity.Normalize();

		// 移動計算
		newPos = (crntPos + (m_MoveVelocity * m_MoveSpeed));

		if (m_IsJump)
		{
			// 地面判定
			if (newPos.y < 0.0f)
			{
				// ****************************************************
				//				 ジャンプ - 着地音再生
				// ****************************************************
				Master::m_pSoundManager->Play(SOUND_TYPE::SE, SOUND_ID_TO_INT(SOUND_ID::SOLDIER_R_JUMP_LAND));

				newPos.y = 0.0f;
				m_JumpVelocity = 0.0f;
				m_IsJump = false;
				ChangeAnimation(PLAYER_ANIMATION_ID::JUMP_LAND);
			}
		}

		// 移動ベクトルを加算
		m_pMyTransformComp.lock()->set_Pos(newPos);

		// 動いているならアニメーション
		m_IsAnim = true;
		
		/*
		// 移動ベクトルに合わせてY軸のみ回転させる
		// ジャンプ時に回転しないよう、X/Zのみ考慮
		*/
		VEC2 velocityXZ = VEC2(m_MoveVelocity.x, m_MoveVelocity.z);
		if (velocityXZ.Length() > 0.001f)
		{
			//!***********************************
			// 方向の処理を外に出すとガタガタする
			//!***********************************
			float targetAngle = 0.0f;      //目標角度

			// ジャンプ中でないなら走る
			if (m_IsJump == false)
			{
				// 走りアニメーション
				ChangeAnimation(PLAYER_ANIMATION_ID::JOG_FWD_LOOP);
			}

			if (!m_IsContinuousAngle)
			{
				MovedAngle(crntRot,m_MoveVelocity);
			}
		}
	}

	// 継続的に視点を変える
	if (m_IsContinuousAngle)
	{
		ContinuousAngle(crntRot);
	}
}

//*---------------------------------------------------------------------------------------
//*【?】描画
//*
//* [引数]
//* RendererEngine& : 描画エンジンの参照
//*
//* [返値]
//* void
//*----------------------------------------------------------------------------------------
void PlayerController::Draw(RendererEngine& renderer)
{
	return;
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
	ChangeAnimation(PLAYER_ANIMATION_ID::ROLL);

	auto pOwner = m_pOwner.lock();
	m_pMyTransformComp = pOwner->get_Transform().lock();

	VEC3 crntRot = m_pMyTransformComp.lock()->get_VEC3ToRotateToRad();
	VEC3 crntPos = m_pMyTransformComp.lock()->get_VEC3ToPos();	// 現在の位置
	VEC3 newPos = VEC3();	// 新しい位置

	// 0.0 ～ 1.0 に正規化
	float t = std::min(
		static_cast<float>( m_RollingCounter) / static_cast<float>(m_RollingDuration), 
		1.0f); 

	// イージング関数でカウンタに合わせて速度を落としていく
	float factor = Tool::Easing::EaseOutCubic(t);
	newPos = crntPos + (m_MoveVelocity * (ROLLING_SPEED * factor));

	// 時間で止める
	if (m_RollingCounter >= m_RollingDuration)
	{
		/* 移動ベクトルとかを元に戻す */
		m_IsRolling = false;
		m_RollingCounter = 0;

		// ****************************************************
		//				 ローリング終了音再生
		// ****************************************************
		Master::m_pSoundManager->Play(SOUND_TYPE::SE, SOUND_ID_TO_INT(SOUND_ID::SOLDIER_R_JUMP_LAND));
	}
	
	m_pMyTransformComp.lock()->set_Pos(newPos);

	// ローリング方向に合わせて回転させる
	MovedAngle(crntRot,m_MoveVelocity);

	m_RollingCounter++;
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
}

//*---------------------------------------------------------------------------------------
//*【?】アニメーションの切り替え
//*
//* [引数]
//* id : アニメーション番号
//* [返値]なし
//*----------------------------------------------------------------------------------------
void PlayerController::ChangeAnimation(PLAYER_ANIMATION_ID id)
{
	// 同じ又はアニメーションが止まっているなら返す
	if (id == m_CrntAnimID)
	{
		return;
	}

	// ひとつ前のアニメーションIDセット
	m_pAnimatorComp.lock()->set_PrevAnimIndex(static_cast<int>(m_CrntAnimID));

	m_CrntAnimID = id;

	// 現在のアニメーションIDセット
	m_pAnimatorComp.lock()->set_AnimIndex(static_cast<int>(m_CrntAnimID));

	if (m_CrntAnimID == PLAYER_ANIMATION_ID::ROLL)
	{
		m_pAnimatorComp.lock()->set_AnimProcTime(0.0f);
		m_pAnimatorComp.lock()->set_ShadowAnimProcTime(0.0f);
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
	float targetAngleY = (angle_H - 1.57) * -1;

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
	targetAngleY -= 3.14f;	// ※ プレイヤーモデルが前後反転してしまっているため

	// 目標とするクォータニオン
	XMVECTOR targetRotQ = XMQuaternionRotationRollPitchYaw(0.0f, targetAngleY, 0.0f);
	
    XMVECTOR crntRotQ = m_pMyTransformComp.lock()->get_RotationQuaternion();

	// クォータニオンの球面線形補間
	// 普通の線形補間だと、値が飛んでしまうためクォータニオンの場合は球面線形補間を使う
    XMVECTOR newRotQ = XMQuaternionSlerp(crntRotQ, targetRotQ, 0.5f);

	m_pMyTransformComp.lock()->set_RotationQuaternion(newRotQ);
}

#include "pch.h"
#include "Component_PlayerController.h"
#include "Component_3DCamera.h"
#include "Component_SkinnedMeshAnimator.h"
#include "GameObject.h"
#include "InputFactory.h"
#include "DirectWriteManager.h"
#include "RendererEngine.h"

using namespace Input;
using namespace VECTOR3;
using namespace VECTOR2;

using namespace DirectX;
using namespace Tool;

#define PLAYER_MOVE_SPEED  5.0f		// プレイヤーの移動スピード

//*---------------------------------------------------------------------------------------
//*【?】コンストラクタ
//* [引数]
//* std::weak_ptr<GameObject> : オーナーオブジェクト
//*	updateRank : 更新順番
//*----------------------------------------------------------------------------------------
PlayerController::PlayerController(std::weak_ptr<GameObject> pOwner, int updateRank) : IComponent(pOwner, updateRank),
m_pCameraComp(),
m_StateMachine(this),
m_IsAnim(false),
m_IsJump(false),
m_MoveVelocity(VEC3()),
m_MoveSpeed(PLAYER_MOVE_SPEED),
m_CrntAnimID(PLAYER_ANIMATION_ID::T_POSE)
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
void PlayerController::Init(RendererEngine& renderer)
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
	m_AnimatorComp = m_pOwner.lock()->get_Component<SkinnedMeshAnimator>();

	m_CrntAnimID = PLAYER_ANIMATION_ID::CROUCH_FWD_LOOP;
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
	auto pOwner = m_pOwner.lock();
	VEC3 upVec = VEC3(0.0f, 1.0f, 0.0f);			// カメラから取得
	float angle_H = m_pCameraComp->get_Angle_H();	// 水平アングル取得
	auto pTransform = pOwner->get_Transform().lock();

	VEC3 newPos = VEC3();				// 新しく入れる座標
	VEC3 crntPos = pTransform->get_VEC3ToPos();			// 現在の座標
	VEC3 crntRot = pTransform->get_VEC3ToRotateToRad();	// 現在の回転

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

	// 移動 
	if (GetInput(CONFIG_INPUT::MOVE_F)) m_MoveVelocity += forward;
	if (GetInput(CONFIG_INPUT::MOVE_B)) m_MoveVelocity -= forward;
	if (GetInput(CONFIG_INPUT::MOVE_R)) m_MoveVelocity += right;
	if (GetInput(CONFIG_INPUT::MOVE_L)) m_MoveVelocity -= right;
	//m_MoveVelocity = m_MoveVelocity.Normalize();

	//if (GetInput(CONFIG_INPUT::JUMP))   velocity = velocity + upVec;
	//if (GetInput(CONFIG_INPUT::C))		velocity = velocity - upVec;

	//float flightDuration = (m_JumpForce * 2) / -m_Gravity;	// 滞空時間

	//-----------------------------------------------------------------------------
	// ■ ジャンプ入力受付 / ジャンプ中処理
	//-----------------------------------------------------------------------------
	if (m_IsJump == false)
	{
		if (GetInput(CONFIG_INPUT::JUMP))
		{
			m_IsJump = true;
			m_MoveVelocity.y = m_JumpForce;

			// ジャンプ開始アニメーション
			ChangeAnimation(PLAYER_ANIMATION_ID::JUMP_START);
		}
	}
	else
	{
		// ジャンプ中アニメーション
		ChangeAnimation(PLAYER_ANIMATION_ID::JUMP_LOOP);
		m_MoveVelocity.y -= m_Gravity;	// 重力
	}

	m_IsAnim = false;

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
				newPos.y = 0.0f;
				m_MoveVelocity.y = 0.0f;
				m_IsJump = false;
				ChangeAnimation(PLAYER_ANIMATION_ID::JUMP_LAND);
			}
		}

		// 移動ベクトルを加算
		pTransform->set_Pos(newPos);

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

			//目標の方向ベクトルから角度値を算出c
			targetAngle = atan2(m_MoveVelocity.x, m_MoveVelocity.z);
			targetAngle -= 3.14f;	// ※ プレイヤーモデルが前後反転してしまっているため

			// 線形補間
			targetAngle = Lerp(crntRot.y, targetAngle, 0.5f);

			pTransform->set_RotateToRad(0.0f, targetAngle, 0.0f);
		}
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

void PlayerController::ChangeAnimation(PLAYER_ANIMATION_ID id)
{
	// 同じなら返す
	if (id == m_CrntAnimID)
	{
		return;
	}

	// ひとつ前のアニメーションIDセット
	m_AnimatorComp->set_PrevAnimIndex(static_cast<int>(m_CrntAnimID));

	m_CrntAnimID = id;

	// 現在のアニメーションIDセット
	m_AnimatorComp->set_AnimIndex(static_cast<int>(m_CrntAnimID));
	m_AnimatorComp->set_AnimTime(0.0f);
}
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

#define PLAYER_MOVE_SPEED  5.0f		// プレイヤーの移動スピード

//*---------------------------------------------------------------------------------------
//*【?】コンストラクタ
//* [引数]
//* std::weak_ptr<GameObject> : オーナーオブジェクト
//*	updateRank : 更新順番
//*----------------------------------------------------------------------------------------
PlayerController::PlayerController(std::weak_ptr<GameObject> pOwner, int updateRank) : IComponent(pOwner, updateRank),
m_pCameraComp()
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
	
	m_pCameraComp = obj->get_Component<Camera3D>();
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
void PlayerController::Update(RendererEngine& renderer)
{
	auto pOwner = m_pOwner.lock();
	VEC3 upVec = VEC3(0.0f, 1.0f, 0.0f);	// カメラから取得
	VEC3 camPos = m_pCameraComp->get_OwnerObj().lock()->get_Transform().lock()->get_VEC3ToPos();
	VEC3 camFocusPoint = m_pCameraComp->get_FocusPoint();
	auto pTransform = pOwner->get_Transform().lock();

	// 前方向と右方向ベクトルを作る 
	// 右方向ベクトルは上方向と前方向ベクトルの外積を取ることでできる
	VEC3 forward = camFocusPoint - camPos;
	forward.y = 0.0f;
	VEC3 right = VEC3::Cross(upVec, forward);
	VEC3 moveDir{ 0.0f,0.0f,0.0f };

	forward = forward.Normalize();
	right = right.Normalize();

	// 移動 
	if (GetInput(CONFIG_INPUT::MOVE_F)) moveDir = moveDir + forward;
	if (GetInput(CONFIG_INPUT::MOVE_B)) moveDir = moveDir - forward;
	if (GetInput(CONFIG_INPUT::MOVE_R)) moveDir = moveDir + right;
	if (GetInput(CONFIG_INPUT::MOVE_L)) moveDir = moveDir - right;
	if (GetInput(CONFIG_INPUT::JUMP))   moveDir = moveDir + upVec;
	if (GetInput(CONFIG_INPUT::C))		moveDir = moveDir - upVec;

	VEC3 pos = m_pOwner.lock()->get_Transform().lock()->get_VEC3ToPos();;

	static float debugSpeed = 1.5f;
	m_IsAnim = false;
	if (moveDir.Length() > 0.001) {
		moveDir = moveDir.Normalize();
		float speed = PLAYER_MOVE_SPEED * debugSpeed;
		VEC3 crntPos = pTransform->get_VEC3ToPos();
		VEC3 setPos = (crntPos + (moveDir * speed));

		// 移動ベクトルを加算
		pTransform->set_Pos(setPos);

		// 動いていればアニメーション
		m_IsAnim = true;

		//!***********************************
		// 方向の処理を外に出すとガタガタする
		//!***********************************
		float targetAngle = 0.0f;      //目標角度
		VEC3 Ca;
		Ca = camFocusPoint - camPos;
		
		//目標の方向ベクトルから角度値を算出c
		targetAngle = atan2(Ca.x, Ca.z); 
		targetAngle -= 3.14f;
		pTransform->set_RotateToRad(0.0f, targetAngle, 0.0f);
	}

	pOwner->get_Component<SkinnedMeshAnimator>()->set_IsAnim(m_IsAnim);


	// Imgui デバッグ
	Master::m_pDebugger->BeginDebugWindow("PlayerInfo");
	Master::m_pDebugger->DG_SliderFloat("AccelerationSpeed:", 1, &debugSpeed, 0.1f, 8.0f);
	Master::m_pDebugger->DG_TextValue("CrntMoveSpeed: %f.1", (PLAYER_MOVE_SPEED * debugSpeed));
	Master::m_pDebugger->DG_DragVec3("Pos:", &pos, 5.1f, -10000.0f, 10000.0f);
	Master::m_pDebugger->EndDebugWindow();
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

}
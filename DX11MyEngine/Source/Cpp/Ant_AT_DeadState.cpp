#include "pch.h"
#include "Component_EnemyController.h"
#include "Component_BoxCollider.h"
#include "Component_Physics.h"
#include "Ant_StateHeader.h"
#include "GameObject.h"

using namespace DirectX;
using namespace VECTOR3;
using namespace VECTOR2;
using namespace UtilityData;
using namespace EnemyData;

//*---------------------------------------------------------------------------------------
//* @:Ant_AT_DeadState Class 
//*【?】開始
//* 引数：1.EnemyController
//* 返値：void
//*----------------------------------------------------------------------------------------
void Ant_AT_DeadState::OnEnter(class EnemyController* pOwner)
{
	// 移動ベクトルは0
	pOwner->set_MoveVelocity(VEC3());

	pOwner->set_IsAnim(false);

	pOwner->set_MoveLogicState(MOVE_BEHAVIOUR_TYPE::NONE);

	auto myTransform = pOwner->get_OwnerObj().lock()->get_Transform().lock();
	m_StartRotQ = myTransform->get_RotationQuaternion();

	float angle = Master::m_pRandomManager->GetFloatRandom(Tool::G_PI_2_F, Tool::G_PI_F);
	angle = Tool::G_PI_F;

	// 前方を軸に180度回転させる
	XMVECTOR axis = myTransform->get_Forward();

	XMVECTOR targetQuat = XMQuaternionRotationAxis(axis, angle);
	m_TargetRotQ = XMQuaternionMultiply(m_StartRotQ, targetQuat);


	// コライダーの判定をオフに
	//pOwner->get_OwnerObj().lock()->get_Component<BoxCollider>()->set_IsEnable(false);	
	auto collider = pOwner->get_OwnerObj().lock()->get_Component<BoxCollider>();
	collider->set_Center(VEC3(0.0f, -1.0f, 0.0f));
}

//*---------------------------------------------------------------------------------------
//* @:Ant_AT_DeadState Class 
//*【?】終了
//* 引数：1.EnemyController
//* 返値：void
//*----------------------------------------------------------------------------------------
void Ant_AT_DeadState::OnExit(class EnemyController* pOwner)
{
	pOwner->clear_StateTimer();
}

//*---------------------------------------------------------------------------------------
//* @:Ant_AT_DeadState Class 
//*【?】更新
//* 引数：1.EnemyController
//* 返値：void
//*----------------------------------------------------------------------------------------
int Ant_AT_DeadState::Update(class EnemyController* pOwner)
{
	if (pOwner->get_OwnerObj().expired())
	{
		MessageBoxA(NULL, "ターゲットがいません", "Ant_PT_MoveState", MB_OK);
		assert(false);
	}
	else
	{
		// 共通処理
		int commonRes = Ant_CommonStateProcess::CommonProcess(pOwner);
		// 同じ場合も返す
		if (commonRes != -1 && commonRes != ANT_STATE::ANT_STATE_ACTIVE_DEAD){
			return commonRes;
		}

		float deltaTime = Master::m_pTimeManager->get_DeltaTime();
		auto myTransform = pOwner->get_OwnerObj().lock()->get_Transform().lock();
		VEC3 crntPos = myTransform->get_VEC3ToPos();					// 現在の座標
		VEC3 crntRot = myTransform->get_VEC3ToLocal_RotateToRad();		// 現在のオイラー
		XMVECTOR crntRotQ = myTransform->get_RotationQuaternion();		// 現在のクオータニオン
		float timer = pOwner->get_StateTimer();

		//*****************************************************************************************
		//						ひっくり返る
		//*****************************************************************************************
		float t = timer / OVERTURN_TIME;
		if (t <= 1.0f) {
			float ease = std::min(Tool::Easing::EaseOutBounce(t), 1.0f);
			crntRotQ = XMQuaternionSlerp(crntRotQ, m_TargetRotQ, ease);	// 球面補間でひっくり返らせる
			myTransform->set_RotationQuaternion(crntRotQ);

			// 跳ねる感じに
			//crntPos.y = ((Tool::Easing::EaseOutBounce(t)*-1.0f) * -3.0f);
			//myTransform->set_Pos(crntPos);
		}


		//*****************************************************************************************
		//						裏世界へ落下する
		//*****************************************************************************************
		if (timer - OVERTURN_TIME > DELETE_TIME)
		{
			pOwner->get_OwnerObj().lock()->get_Component<Physics>()->set_IsEnable(false);
			pOwner->get_OwnerObj().lock()->set_StatusFlag(OBJECT_STATUS_BITFLAG::IS_DELETE);
		}
	}

	return ANT_STATE::ANT_STATE_ACTIVE_DEAD;
}

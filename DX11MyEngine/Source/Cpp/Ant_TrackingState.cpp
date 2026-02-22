#include "pch.h"
#include "Ant_TrackingState.h"
#include "Component_EnemyController.h"
#include "GameObject.h"

using namespace VECTOR3;
using namespace VECTOR2;

//*---------------------------------------------------------------------------------------
//* @:Ant_TrackingState Class 
//*【?】開始
//* 引数：1.EnemyController
//* 返値：void
//*----------------------------------------------------------------------------------------
void Ant_TrackingState::OnEnter(class EnemyController *pOwner)
{
	pOwner->set_IsAnim(true);
}

//*---------------------------------------------------------------------------------------
//* @:Ant_TrackingState Class 
//*【?】終了
//* 引数：1.EnemyController
//* 返値：void
//*----------------------------------------------------------------------------------------
void Ant_TrackingState::OnExit(class EnemyController *pOwner)
{

}

//*---------------------------------------------------------------------------------------
//* @:Ant_TrackingState Class 
//*【?】更新
//* 引数：1.EnemyController
//* 返値：void
//*----------------------------------------------------------------------------------------
int Ant_TrackingState::Update(class EnemyController *pOwner)
{
	auto targetObj = pOwner->get_Target();
	auto myObj = pOwner->get_OwnerObj().lock();

	// 目標が居なくなったら待機状態へ戻る
	if (targetObj == nullptr || myObj == nullptr)
	{
		pOwner->ChangeState(ANT_STATE::ANT_STATE_IDLE);
	}

	auto targetTransform = targetObj->get_Transform().lock();
	auto myTransform = myObj->get_Transform().lock();

	VEC3 targetPos = targetTransform->get_VEC3ToPos();	// 目標位置
	VEC3 myPos = myTransform->get_VEC3ToPos();			// 自分の位置
	VEC3 targetDir = (targetPos - myPos).Normalize();	// 目標方向
	

	// 距離が離れたら待機へ戻る
	if (VEC3::Distance(myPos, targetPos) > 600.0f)
	{
		return ANT_STATE::ANT_STATE_IDLE;
	}

	// 移動ベクトルを渡す
	pOwner->set_MoveVelocity(targetDir * pOwner->get_MoveSpeed());


	return ANT_STATE::ANT_STATE_TRACKING;
}


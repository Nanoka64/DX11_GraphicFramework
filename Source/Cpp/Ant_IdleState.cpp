#include "pch.h"
#include "Component_EnemyController.h"
#include "Ant_IdleState.h"
#include "GameObject.h"

using namespace VECTOR3;
using namespace VECTOR2;

//*---------------------------------------------------------------------------------------
//* @:Ant_IdleState Class 
//*【?】開始
//* 引数：1.EnemyController
//* 返値：void
//*----------------------------------------------------------------------------------------
void Ant_IdleState::OnEnter(class EnemyController* pOwner)
{
	// 移動ベクトルは0
	pOwner->set_MoveVelocity(VEC3());


	pOwner->set_IsAnim(false);
}

//*---------------------------------------------------------------------------------------
//* @:Ant_IdleState Class 
//*【?】終了
//* 引数：1.EnemyController
//* 返値：void
//*----------------------------------------------------------------------------------------
void Ant_IdleState::OnExit(class EnemyController* pOwner)
{

}

//*---------------------------------------------------------------------------------------
//* @:Ant_IdleState Class 
//*【?】更新
//* 引数：1.EnemyController
//* 返値：void
//*----------------------------------------------------------------------------------------
int Ant_IdleState::Update(class EnemyController* pOwner)
{
	auto target = pOwner->get_Target();	// 目標オブジェクト
	if (target)
	{
		auto targetTransform = target->get_Transform().lock();
		VEC3 targetPos = targetTransform->get_VEC3ToPos();	// 目標位置

		// オーナーオブジェクト
		if (!pOwner->get_OwnerObj().expired())
		{
			auto myTransform = pOwner->get_OwnerObj().lock()->get_Transform().lock();

			VEC3 myPos = myTransform->get_VEC3ToPos();	// 自分の位置

			// 距離が200未満なら追従ステートへ
			if (VEC3::Distance(myPos, targetPos) < 500.0f)
			{
				return ANT_STATE::ANT_STATE_TRACKING;
			}
		}
	}

	return ANT_STATE::ANT_STATE_IDLE;
}

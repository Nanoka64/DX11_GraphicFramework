#include "pch.h"
#include "Component_EnemyController.h"
#include "Octahedron_StateHeader.h"
#include "GameObject.h"

using namespace VECTOR3;
using namespace VECTOR2;
using namespace UtilityData;
using namespace EnemyData;

//*---------------------------------------------------------------------------------------
//* @:Octahedron_AT_IdleState Class 
//*【?】開始
//* 引数：1.EnemyController
//* 返値：void
//*----------------------------------------------------------------------------------------
void Octahedron_AT_IdleState::OnEnter(class EnemyController* pOwner)
{
	// 移動ベクトルは0
	pOwner->set_MoveVelocity(VEC3());
	pOwner->set_IsAnim(false);
	pOwner->set_MoveLogicState(MOVE_BEHAVIOUR_TYPE::NONE);
	m_IdleDuration = Master::m_pRandomManager->GetFloatRandom(IDLE_MIN_TIME, IDLE_MAX_TIME);

}

//*---------------------------------------------------------------------------------------
//* @:Octahedron_AT_IdleState Class 
//*【?】終了
//* 引数：1.EnemyController
//* 返値：void
//*----------------------------------------------------------------------------------------
void Octahedron_AT_IdleState::OnExit(class EnemyController* pOwner)
{
	pOwner->clear_StateTimer();
}

//*---------------------------------------------------------------------------------------
//* @:Octahedron_AT_IdleState Class 
//*【?】更新
//* 引数：1.EnemyController
//* 返値：void
//*----------------------------------------------------------------------------------------
int Octahedron_AT_IdleState::Update(class EnemyController* pOwner)
{
	auto target = pOwner->get_Target();	// 目標オブジェクト
	if (target == nullptr || pOwner->get_OwnerObj().expired())
	{
		MessageBoxA(NULL, "ターゲットがいません", "Ant_PT_MoveState", MB_OK);
		assert(false);
	}
	else
	{
		// 共通処理
		int commonRes = Octahedron_CommonStateProcess::CommonProcess(pOwner);
		if (commonRes != -1)
		{
			return commonRes;
		}

		//=========================================================================================
		//
		//						待機期間を終えたら、移動または、追跡ステートへ
		//
		//=========================================================================================
		if (pOwner->get_StateTimer() > m_IdleDuration)
		{
			// trueなら追跡、falseなら移動
			bool isTracking = Master::m_pRandomManager->GetBoolRandom();

			if (isTracking)
			{
				// 追跡ステートへ
				return OCTAHEDRON_STATE::OCTAHEDRON_STATE_ACTIVE_TRACKING;
			}
			else
			{
				// 移動ステートへ
				return OCTAHEDRON_STATE::OCTAHEDRON_STATE_ACTIVE_MOVE;
			}
		}
	}

	return OCTAHEDRON_STATE::OCTAHEDRON_STATE_ACTIVE_IDLE;
};
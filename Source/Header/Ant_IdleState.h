#pragma once
#include "IState.h"

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:Ant_IdleState Class --- */
//
//  ★継承：IComponent ★
//
// 【?】待機時ステート
//		
// ***************************************************************************************
class Ant_IdleState : public IState<class EnemyController>
{
private:



public:
	void OnEnter(class EnemyController* pOwner) override;
	void OnExit(class EnemyController* pOwner)override;
	int Update(class EnemyController* pOwner)override;

};


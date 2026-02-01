#include "pch.h"
#include "Component_EnemyController.h"
#include "Ant_IdleState.h"

void Ant_IdleState::OnEnter(class EnemyController* pOwner)
{

}

void Ant_IdleState::OnExit(class EnemyController* pOwner)
{

}

int  Ant_IdleState::Update(class EnemyController* pOwner)
{
	return ANT_STATE::ANT_STATE_IDLE;
}

void Ant_IdleState::Draw(class EnemyController* pOwner)
{

}

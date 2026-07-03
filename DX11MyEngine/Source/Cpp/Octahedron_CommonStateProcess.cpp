#include "pch.h"
#include "Octahedron_StateHeader.h"
#include "Component_EnemyController.h"
#include "ConstantEnemyData.h"

using namespace VECTOR3;
using namespace EnemyData;

//*---------------------------------------------------------------------------------------
//*【?】共通処理
//*
//* [引数]
//* *pOwner : 親
//*
//* [返値]
//* ステートID（-1の場合は変更なし） 
//*----------------------------------------------------------------------------------------
int Octahedron_CommonStateProcess::CommonProcess(class EnemyController* pOwner)
{
	// 死亡ステートへ
	if (pOwner->get_IsDead())
	{
		return OCTAHEDRON_STATE::OCTAHEDRON_STATE_ACTIVE_DEAD;
	}

	// 変更なし
	return -1;
}
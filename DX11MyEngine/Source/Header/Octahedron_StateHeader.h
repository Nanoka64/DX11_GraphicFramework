#pragma once
#include "IState.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//						八面体のステートのヘッダクラス定義をまとめたヘッダ
// 
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// PT:巡回中
// AT:戦闘態勢


// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:Octahedron_AT_IdleState Class --- */
//
//  ★継承：IState ★
//
// 【?】[アクティブ]
//		待機時ステート
//		
// ***************************************************************************************
class Octahedron_AT_IdleState : public IState<class EnemyController>
{
private:


public:
	void OnEnter(class EnemyController* pOwner) override;
	void OnExit(class EnemyController* pOwner)override;
	int Update(class EnemyController* pOwner)override;
};


// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:Octahedron_AT_MoveState Class --- */
//
//  ★継承：IState ★
//
// 【?】[アクティブ]
//		移動ステート
//		
// ***************************************************************************************
class Octahedron_AT_MoveState : public IState<class EnemyController>
{
private:


public:
	void OnEnter(class EnemyController* pOwner) override;
	void OnExit(class EnemyController* pOwner)override;
	int Update(class EnemyController* pOwner)override;
};


// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:Octahedron_AT_TrackingState Class --- */
//
//  ★継承：IState ★
//
// 【?】[アクティブ]
//		追跡ステート
//		
// ***************************************************************************************
class Octahedron_AT_TrackingState : public IState<class EnemyController>
{
private:


public:
	void OnEnter(class EnemyController* pOwner) override;
	void OnExit(class EnemyController* pOwner)override;
	int Update(class EnemyController* pOwner)override;
};


// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:Octahedron_AT_AttackLaser01State Class --- */
//
//  ★継承：IState ★
//
// 【?】[アクティブ]
//		レーザー攻撃ステート
//		一定時間、対象に照射する。
//		
// ***************************************************************************************
class Octahedron_AT_AttackLaser01State : public IState<class EnemyController>
{
private:


public:
	void OnEnter(class EnemyController* pOwner) override;
	void OnExit(class EnemyController* pOwner)override;
	int Update(class EnemyController* pOwner)override;
};


// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:Octahedron_AT_AttackLaser02State Class --- */
//
//  ★継承：IState ★
//
// 【?】[アクティブ]
//		レーザー攻撃ステート
//		4箇所から対象へレーザーを照射する
//		
// ***************************************************************************************
class Octahedron_AT_AttackLaser02State : public IState<class EnemyController>
{
private:


public:
	void OnEnter(class EnemyController* pOwner) override;
	void OnExit(class EnemyController* pOwner)override;
	int Update(class EnemyController* pOwner)override;
};


// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:Octahedron_AT_AttackLaser03State Class --- */
//
//  ★継承：IState ★
//
// 【?】[アクティブ]
//		レーザー攻撃ステート
//		
//		
// ***************************************************************************************
class Octahedron_AT_AttackLaser03State : public IState<class EnemyController>
{
private:


public:
	void OnEnter(class EnemyController* pOwner) override;
	void OnExit(class EnemyController* pOwner)override;
	int Update(class EnemyController* pOwner)override;
};




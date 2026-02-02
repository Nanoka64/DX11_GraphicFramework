#pragma once

/// <summary>
/// 敵の種類
/// </summary>
enum ENEMY_TYPE
{
	ENEMY_TYPE_ANT_Normal,

	ENEMY_TYPE_NUM,
};

/// <summary>
/// アリのステート
/// </summary>
enum ANT_STATE
{
	ANT_STATE_IDLE,			// 待機
	ANT_STATE_TRACKING,		// 追従
	ANT_STATE_ATTACK_BITE,	// 噛みつき攻撃
	ANT_STATE_ATTACK_ACID,	// 酸攻撃
	ANT_STATE_ESCAPE,		// 逃げる

	ANT_STATE_NUM,
};

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:EnemyStateFactory Class --- */
//
// 【?】敵のステートの作成を行う
//
// ***************************************************************************************
class EnemyStateFactory
{
private:

public:
	/// <summary>
	/// シーンステートを作成して挿入する
	/// </summary>
	/// <param name="_out">出力先</param>
	/// <param name="_createScene">作成するシーン</param>
	/// <param name="_renderer">描画エンジンの参照</param>
	static void Create(StateMachine<class EnemyController>& _out, int _createState, RendererEngine& _renderer);

private:
	/// <summary>
	/// 蟻の作成
	/// </summary>
	/// <param name="_out">出力先</param>
	/// <param name="_renderer">描画エンジンの参照</param>
	static void CreateAntState(StateMachine<class EnemyController>& _out, RendererEngine& _renderer);
};


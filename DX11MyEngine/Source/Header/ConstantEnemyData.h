#pragma once

namespace EnemyData
{
	/// <summary>
	/// 敵の種類
	/// </summary>
	enum class ENEMY_TYPE
	{
		GIANT_ANT_Normal,	// アリ

		OCTAHEDRON,			// 八面体

		NUM,
	};


	/*
	* 
	* PT : 巡回中
	* AT : 戦闘態勢
	* 
	*/

	/// <summary>
	/// アリのステート
	/// </summary>
	enum ANT_STATE
	{
		// 非アクティブ
		ANT_STATE_PATROL_IDLE,	// 待機
		ANT_STATE_PATROL_MOVE,	// 移動
		

		// アクティブ
		ANT_STATE_ACTIVE_MOVE,			// 移動
		ANT_STATE_ACTIVE_TRACKING,		// 追従
		ANT_STATE_ACTIVE_ATTACK_BITE,	// 噛みつき攻撃
		ANT_STATE_ACTIVE_ATTACK_ACID,	// 酸攻撃
		ANT_STATE_ACTIVE_DAMAGED,		// ダメージ
		ANT_STATE_ACTIVE_BLOWN_AWAY,	// 爆発による吹き飛び
		ANT_STATE_ACTIVE_FALLING,		// 落下中
		ANT_STATE_ACTIVE_HIT_STUN,		// 被弾
		ANT_STATE_ACTIVE_DEAD,			// 死亡
	};

	inline const char* g_AntStateNames[] =
	{
		"PATROL_IDLE",
		"PATROL_MOVE",
		"ACTIVE_MOVE",
		"ACTIVE_TRACKING",
		"ACTIVE_ATTACK_BITE",
		"ACTIVE_ATTACK_ACID",
		"ACTIVE_DAMAGED",
		"ACTIVE_BLOWN_AWAY",
		"ACTIVE_FALLING",
		"ACTIVE_HIT_STUN",
		"ACTIVE_DEAD",
	};

	const float ENEMY_ANT01_BASE_HP = 200.0f;	// アリの基本HP

	/// <summary>
	/// 敵の基本データ
	/// </summary>
	struct BaseEnemyData
	{
		ENEMY_TYPE _enmyType = ENEMY_TYPE::GIANT_ANT_Normal;

		float _hp = 0.0f;
		float _attack = 0.0f;
		float _moveSpeed = 0.0f;
	};


	/// <summary>
	/// 難易度係数
	/// </summary>
	struct EnemyDifficultyFactor
	{
		float _hpRate = 0.0f;
		float _attackRate = 0.0f;
		float _moveSpeedRate = 0.0f;
	};
}
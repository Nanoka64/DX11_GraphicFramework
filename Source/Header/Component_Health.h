#pragma once
#include "IComponent.h"



// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:Health Class --- */
//
//  ★継承：IComponent ★
//
// 【?】キャラクタのHPを管理する
//		
// ***************************************************************************************
class Health : public IComponent
{
private:
	float m_CrntHP;	// 現在のHP
	float m_MaxHP;	// 最大HP
	bool m_IsDead;


	std::vector<std::function<void(float)>> m_DamageTaskArray;	// ダメージを受けた際の処理
	std::vector<std::function<void()>> m_DeathTaskArray;		// 死んだときの処理

public:
	Health(std::weak_ptr<GameObject> pOwner, int updateRank);
	~Health();

	void Start(RendererEngine& renderer) override;	// 初期化
	void Update(RendererEngine& renderer) override;// 更新
	void TakeDamage(const float _dmg);	// ダメージ処理

	void RegisterOnDead(std::function<void()> _callback);	// 死んだときの処理の登録
	void RegisterOnDamage(std::function<void(float)> _callback);	// ダメージを受けた際の処理の登録

	/* HP */
	float get_CrntHP()const { return m_CrntHP; }
	float get_MaxHP()const { return m_MaxHP; }
	void set_MaxHP(const float _hp) { m_MaxHP = _hp; }
	void set_CrntHP(const float _hp) { m_CrntHP = _hp; }
};


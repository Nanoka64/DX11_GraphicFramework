#pragma once
#include "Component_BulletBase.h"

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:safeitemname Class --- */
//
//  ★継承：BulletBase Component ★
//
// 【?】爆発する弾
//		
// ***************************************************************************************
class ExplosionBullet : public BulletBase
{
private:
	BulletData::ExplosionBulletData m_Parameter;	// 弾に関するパラメータ

public:
	ExplosionBullet(std::weak_ptr<GameObject> pOwner, int updateRank = 100);
	~ExplosionBullet();

	void Start(RendererEngine &renderer) override;		// 初期化
	void Update(RendererEngine &renderer) override;		// 更新処理

	void OnCollisionEnter(const class CollisionInfo &other);	//　衝突時の処理
	void set_Parameter(const BulletData::ExplosionBulletData &_param) { m_Parameter = _param; };	// パラメータの設定
	void Reset()override;
	void Setup()override;
};

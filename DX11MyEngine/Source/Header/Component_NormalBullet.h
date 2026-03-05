#pragma once
#include "Component_BulletBase.h"
#include "ConstantBulletData.h"

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:Bullet Class --- */
//
//  ★継承：BulletBase Component ★
//
// 【?】弾の移動等
//		
// ***************************************************************************************
class NormalBullet : public BulletBase
{
private:
	BulletData::NormalBulletData m_Parameter;	// 弾に関するパラメータ

public:
	NormalBullet(std::weak_ptr<GameObject> pOwner, int updateRank = 100);
	~NormalBullet();

	void Start(RendererEngine &renderer) override;		// 初期化
	void Update(RendererEngine &renderer) override;		// 更新処理

	void OnCollisionEnter(const class CollisionInfo &other);	// 衝突処理
	void set_Parameter(const BulletData::NormalBulletData &_param) { m_Parameter = _param; };	// パラメータ設定
	void Reset()override;
	void Setup()override;

};


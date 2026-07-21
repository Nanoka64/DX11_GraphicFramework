//#pragma once
//#include "Component_BulletBase.h"
//
//// ***************************************************************************************
//// ---------------------------------------------------------------------------------------
///* --- @:safeitemname Class --- */
////
////  ★継承：BulletBase Component ★
////
//// 【?】爆発する弾
////		
//// ***************************************************************************************
//class ExplosionBullet : public BulletBase
//{
//private:
//	//BulletData::ExplosionBulletData m_Parameter;	// 弾に関するパラメータ
//	float m_SmokeSpwanTimer = 0.0f;
//	const GameObject* m_pTarget = nullptr;
//
//public:
//	ExplosionBullet(std::weak_ptr<GameObject> pOwner, int updateRank = 100);
//	~ExplosionBullet();
//
//	void Start(RendererEngine &renderer) override;		// 初期化
//	void Update(RendererEngine &renderer) override;		// 更新処理
//	void LateUpdate(RendererEngine &renderer) override;		// 更新処理
//
//	void OnTriggerEnter(const class CollisionInfo &other);		//　衝突時の処理
//	const BulletData::ExplosionBulletData* get_ExplosionParameter()const;					// パラメータの取得
//	void Reset()override;
//	void Setup(const BulletData::BulletDataBase* _pParam)override;
//
//};

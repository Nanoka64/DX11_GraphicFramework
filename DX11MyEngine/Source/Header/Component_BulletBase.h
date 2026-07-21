//#pragma once
//#include "IComponent.h"
//
//// ***************************************************************************************
//// ---------------------------------------------------------------------------------------
///* --- @:safeitemname Class --- */
////
////  ★継承：IComponent ★
////
//// 【?】弾コンポーネントの基底クラス
////		
//// ***************************************************************************************
//class BulletBase : public IComponent
//{
//protected:
//	VECTOR3::VEC3 m_StartPos;	// 発射開始位置
//	VECTOR3::VEC3 m_PrevPos;	// 前の座標
//	VECTOR3::VEC3 m_MoveDir;
//	float m_Counter;
//	std::function<void(const class CollisionInfo &_other)> m_CollisionTask;		// 衝突時の処理
//	int m_CrntPenetrationCount;	// 現在の貫通数
//	//std::shared_ptr<BulletData::NormalBulletData> m_pBulletParameter;	// 弾のパラメータ
//	const BulletData::BulletDataBase* m_pWeaponData;	// 読みとり専用の弾データを持つ
//
//public:
//	BulletBase(std::weak_ptr<GameObject> pOwner, int updateRank = 100);
//	virtual ~BulletBase() = default;
//
//	void set_CollisionTask(std::function<void(const class CollisionInfo &_other)> task) { m_CollisionTask = task; }		// 衝突時の処理の設定
//	//void set_BulletParameter(std::shared_ptr<BulletData::NormalBulletData> _pParam) { m_pBulletParameter = _pParam; }	// 弾のパラメータの設定
//	//std::shared_ptr<BulletData::NormalBulletData> get_BulletParameter()const { return m_pBulletParameter; }				// 弾のパラメータの取得
//	virtual void Setup(const BulletData::BulletDataBase* _pParam) = 0;
//	virtual void Reset() = 0;
//};
//

#pragma once
#include "IComponent.h"
class Bullet : public IComponent
{
private:
	VECTOR3::VEC3 m_StartPos;	// 発射開始位置
	VECTOR3::VEC3 m_PrevPos;	// 前の座標
	VECTOR3::VEC3 m_MoveDir;
	int m_CrntPenetrationCount;	// 現在の貫通数
	float m_Counter;
	std::function<void(const class CollisionInfo& _other)> m_CollisionTask;		// 衝突時の処理
	const BulletData::BulletDataBase* m_pBulletData;							// 読みとり専用の弾データを持つ

public:
	Bullet(std::weak_ptr<GameObject> pOwner, int updateRank = 100);
	virtual ~Bullet() = default;

	void set_CollisionTask(std::function<void(const class CollisionInfo& _other)> task) { m_CollisionTask = task; }		// 衝突時の処理の設定
	virtual void Setup(const BulletData::NormalBulletData* _pParam) = 0;
	virtual void Reset() = 0;
};


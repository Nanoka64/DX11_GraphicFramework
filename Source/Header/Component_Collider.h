#pragma once
#include "IComponent.h"

enum class COLLIDER_TYPE
{
	NONE,
	BOX,
	SPHERE,
	RAY,
};

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:Collider Class --- */
//
//  ★ 継承 ★
//
// 【?】コライダーコンポーネントの基底クラス
//
// ***************************************************************************************
class Collider : public IComponent
{
protected:
	bool m_IsEnable;		// 使用するかどうか
	bool m_IsTrigger;		// 衝突判定のみ取るかどうか（falseなら物理も判定をする）
	VECTOR3::VEC3 m_Center;	// コライダーの中心位置
	bool m_IsHit;			// 現在衝突しているかどうか
	COLLIDER_TYPE m_ColliderType;	// コライダーの種類
	std::weak_ptr<class Transform> m_pTransform;
	bool m_IsStatic;	// 静的かどうか（建物など動かないもの）

public:
	Collider(std::weak_ptr<GameObject> pOwner, int updateRank = 100);
	~Collider();

	// 使用フラグ
	void set_IsEnable(bool _flag) { m_IsEnable = _flag; }
	bool get_IsEnable()const { return m_IsEnable; }

	// 物理判定フラグ
	void set_IsTrigger(bool _flag) { m_IsTrigger = _flag; }
	bool get_IsTrigger()const { return m_IsTrigger; }

	// 中心位置
	void set_Center(const VECTOR3::VEC3 &_vIn) { m_Center = _vIn; }
	VECTOR3::VEC3 get_Center()const { return m_Center; }

	COLLIDER_TYPE get_ColliderType()const { return m_ColliderType; }

	std::weak_ptr<class Transform>get_Transform() { return m_pTransform; };

	/* 衝突したかどうか */
	void set_IsHit(bool _flag) { m_IsHit = _flag; }
	bool get_IsHit()const { return m_IsHit; }

	/* 静的かどうか */
	void set_IsStatic(bool _flag) { m_IsStatic = _flag; }
	bool get_IsStatic()const { return m_IsStatic; }
};


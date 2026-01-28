#pragma once
#include "IComponent.h"

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:Bullet Class --- */
//
//  šŒp³FIComponent š
//
// y?z’e‚ÌˆÚ“®“™
//		
// ***************************************************************************************
class Bullet : public IComponent
{
private:
	std::shared_ptr<GameObject> m_pBulletObject;	// ’e‚Æ‚È‚éƒIƒuƒWƒFƒNƒg
	VECTOR3::VEC3 m_StartPos;
	VECTOR3::VEC3 m_MoveVelocity;

public:
	Bullet(std::weak_ptr<GameObject> pOwner, int updateRank = 100);
	~Bullet();

	void Start(RendererEngine &renderer) override;		// ‰Šú‰»
	void Update(RendererEngine &renderer) override;		// XVˆ—
	void Draw(RendererEngine &renderer)override;		// •`‰æˆ—
	void OnCollisionEnter(const class CollisionInfo &other);


	void set_BulletObject(std::shared_ptr<GameObject> pObj);
};


#pragma once
#include "Component_Collider.h"


// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:BoxCollider Class --- */
//
//  š Œp³ š
//
// y?z” ”»’èƒRƒ‰ƒCƒ_[
//
// ***************************************************************************************
class BoxCollider : public Collider
{
private:
	VECTOR3::VEC3 m_Size;

public:
	BoxCollider(std::weak_ptr<GameObject> pOwner, int updateRank = 100);
	~BoxCollider();

	void Start(RendererEngine &renderer) override;		// ‰Šú‰»
	void Update(RendererEngine &renderer) override;		// XVˆ—
	void Draw(RendererEngine &renderer)override;		// •`‰æˆ—

	bool AABB(const VECTOR3::VEC3& _src,const VECTOR3::VEC3& _dest);

	// ‘å‚«‚³
	void set_Size(const VECTOR3::VEC3 &_vIn) { m_Size = _vIn; }
	VECTOR3::VEC3 get_Size()const { return m_Size; }
};


#include "pch.h"
#include "CollisionInfo.h"

CollisionInfo::CollisionInfo():
	m_HitPoint(VECTOR3::VEC3()),
	m_HitNormal(VECTOR3::VEC3()),
	m_RelativeVelocity(VECTOR3::VEC3()),
	m_PenetrationDepth(0.0f)
{

}

CollisionInfo::~CollisionInfo()
{

}


#include "pch.h"
#include "Component_BulletBase.h"
#include "RendererEngine.h"


using namespace VECTOR3;

//*---------------------------------------------------------------------------------------
//*【?】コンストラクタ
//* [引数]
//* pOwner : オーナーオブジェクト
//* updateRank : 更新レイヤー
//*----------------------------------------------------------------------------------------
BulletBase::BulletBase(std::weak_ptr<GameObject> pOwner, int updateRank) : 
    IComponent(pOwner, updateRank),
    m_StartPos(VEC3()),
    m_PrevPos(VEC3()),
    m_MoveVelocity(VEC3()),
    m_Counter(0)
{
	this->set_Tag("BulletBase");
}


#include "pch.h"
#include "Component_Bullet.h"
#include "RendererEngine.h"


using namespace VECTOR3;

//*---------------------------------------------------------------------------------------
//*【?】コンストラクタ
//* [引数]
//* pOwner : オーナーオブジェクト
//* updateRank : 更新レイヤー
//*----------------------------------------------------------------------------------------
Bullet::Bullet(std::weak_ptr<GameObject> pOwner, int updateRank) :
    IComponent(pOwner, updateRank),
    m_StartPos(VEC3()),
    m_PrevPos(VEC3()),
    m_MoveDir(VEC3()),
    m_pBulletData(nullptr),
    m_CrntPenetrationCount(0),
	m_StateMachine(this)
{
    this->set_Tag("Bullet");
}


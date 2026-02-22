#include "pch.h"
#include "Component_Collider.h"
#include "RendererEngine.h"
#include "GameObjectManager.h"
#include "GameObject.h"
#include "ResourceManager.h"

using namespace GIGA_Engine;
using namespace VECTOR3;

//*---------------------------------------------------------------------------------------
//*【?】コンストラクタ
//* [引数]
//* pOwner : オーナーオブジェクト
//* updateRank : 更新レイヤー
//*----------------------------------------------------------------------------------------
Collider::Collider(std::weak_ptr<GameObject> pOwner, int updateRank) 
    : IComponent(pOwner, updateRank),
    m_IsEnable(true),
    m_IsTrigger(false),
    m_IsHit(false),
    m_IsStatic(false),
    m_Center(VEC3()),
    m_ColliderType(COLLIDER_TYPE::BOX)
{
    this->set_Tag("Collider");
}



//*---------------------------------------------------------------------------------------
//*【?】デストラクタ
//*----------------------------------------------------------------------------------------
Collider::~Collider()
{

}

#include "pch.h"
#include "Component_SphereCollider.h"
#include "GameObject.h"

using namespace GIGA_Engine;
using namespace VECTOR3;

//*---------------------------------------------------------------------------------------
//*【?】コンストラクタ
//* [引数]
//* pOwner : オーナーオブジェクト
//* updateRank : 更新レイヤー
//*----------------------------------------------------------------------------------------
SphereCollider::SphereCollider(std::weak_ptr<GameObject> pOwner, int updateRank)
    :Collider(pOwner, updateRank),
    m_Radius(1.0f)
{
    this->set_Tag("SphereCollider");
    m_ColliderType = COLLIDER_TYPE::SPHERE;
}


//*---------------------------------------------------------------------------------------
//*【?】デストラクタ
//*----------------------------------------------------------------------------------------
SphereCollider::~SphereCollider()
{

}


//*---------------------------------------------------------------------------------------
//*【?】初期化
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//* [返値]なし
//*----------------------------------------------------------------------------------------
void SphereCollider::Start(RendererEngine &renderer)
{
    m_pTransform = m_pOwner.lock()->get_Transform();

    if (m_pTransform.expired())
    {
        MessageBox(NULL, "コンポーネントが取得できませんでした。", "Collider", MB_OK);
    }
}


//*---------------------------------------------------------------------------------------
//*【?】更新
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//* [返値]なし
//*----------------------------------------------------------------------------------------
void SphereCollider::Update(RendererEngine &renderer)
{

}


//*---------------------------------------------------------------------------------------
//*【?】描画
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//* [返値]なし
//*----------------------------------------------------------------------------------------
void SphereCollider::Draw(RendererEngine &renderer)
{

}

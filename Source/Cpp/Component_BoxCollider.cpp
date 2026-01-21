#include "pch.h"
#include "Component_BoxCollider.h"
#include "RendererEngine.h"
#include "DebugMesh.h"
#include "GameObject.h"

using namespace GIGA_Engine;
using namespace VECTOR3;

//*---------------------------------------------------------------------------------------
//*【?】コンストラクタ
//* [引数]
//* pOwner : オーナーオブジェクト
//* updateRank : 更新レイヤー
//*----------------------------------------------------------------------------------------
BoxCollider::BoxCollider(std::weak_ptr<GameObject> pOwner, int updateRank)
    :Collider(pOwner, updateRank)
{
    this->set_Tag("BoxCollider");
}

//*---------------------------------------------------------------------------------------
//*【?】デストラクタ
//*----------------------------------------------------------------------------------------
BoxCollider::~BoxCollider()
{

}

//*---------------------------------------------------------------------------------------
//*【?】初期化
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//* [返値]なし
//*----------------------------------------------------------------------------------------
void BoxCollider::Start(RendererEngine &renderer)
{
    m_pBoxMesh = std::make_unique<DebugMesh>();
    m_pBoxMesh->Setup(renderer,DEBUG_MESHS_TYPE::CUBE);
}


//*---------------------------------------------------------------------------------------
//*【?】更新
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//* [返値]なし
//*----------------------------------------------------------------------------------------
void BoxCollider::Update(RendererEngine &renderer)
{

}


//*---------------------------------------------------------------------------------------
//*【?】描画
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//* [返値]なし
//*----------------------------------------------------------------------------------------
void BoxCollider::Draw(RendererEngine &renderer)
{
    auto pContext = renderer.get_DeviceContext();
    XMMATRIX localMat = XMMatrixIdentity();

    auto transform = m_pOwner.lock()->get_Transform().lock();
    VEC3 ownerPos = transform->get_VEC3ToPos();

    XMVECTOR scl = m_Size;
    XMVECTOR pos = ownerPos + m_Center; // 中心位置のオフセットを足す

    XMMATRIX mtxS = XMMatrixScalingFromVector(scl);
    XMMATRIX mtxT = XMMatrixTranslationFromVector(pos);

    localMat = transform->get_ExcludingRotWorldMtx(mtxS, mtxT);

    // メッシュ表示
    m_pBoxMesh->Draw(renderer, localMat);
}


bool BoxCollider::AABB(const VECTOR3::VEC3 &_src, const VECTOR3::VEC3 &_dest)
{
    return true;
}

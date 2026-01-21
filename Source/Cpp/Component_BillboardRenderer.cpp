#include "pch.h"
#include "Component_BillboardRenderer.h"
#include "Component_BillboardResource.h"
#include "RendererEngine.h"
#include "GameObject.h"
#include "Texture.h"
#include "BlendManager.h"


using namespace VERTEX;

//*---------------------------------------------------------------------------------------
//*【?】コンストラクタ
//* [引数]
//* std::weak_ptr<class GameObject> : オーナーオブジェクト
//* updateRank : 更新レイヤー
//*----------------------------------------------------------------------------------------
BillboardRenderer::BillboardRenderer(std::weak_ptr<class GameObject> pOwner, int updateRank) : IComponent(pOwner, updateRank)
{
    this->set_Tag("BillboardRenderer");
}

//*---------------------------------------------------------------------------------------
//*【?】デストラクタ
//*----------------------------------------------------------------------------------------
BillboardRenderer::~BillboardRenderer()
{

}

//*---------------------------------------------------------------------------------------
//*【?】初期化
//*
//* [引数]
//* RendererEngine : 描画エンジンの参照
//*
//* [返値]
//* void
//*----------------------------------------------------------------------------------------
void BillboardRenderer::Start(RendererEngine& renderer)
{

}


//*---------------------------------------------------------------------------------------
//*【?】更新
//*
//* [引数]
//* RendererEngine : 描画エンジンの参照
//*
//* [返値]
//* void
//*----------------------------------------------------------------------------------------
void BillboardRenderer::Update(RendererEngine& renderer)
{

}


//*---------------------------------------------------------------------------------------
//*【?】描画
//*
//* [引数]
//* RendererEngine : 描画エンジンの参照
//*
//* [返値]
//* void
//*----------------------------------------------------------------------------------------
void BillboardRenderer::Draw(RendererEngine& renderer)
{
    auto pContext = renderer.get_DeviceContext();
    std::shared_ptr<MeshResourceData> meshData = m_pResource.lock()->m_pMeshData;
    CB_TRANSFORM_SET* cbTransSet = m_pResource.lock()->m_pCBTransformSet;
    CB_MATERIAL_SET* cbMatSet = m_pResource.lock()->m_pCBMaterialDataSet;
    ID3D11Buffer* vtxBuff = meshData->pVertexBuffer;
    ID3D11Buffer* idxBuff = meshData->pIndexBuffer;

    std::shared_ptr<Transform> transform = m_pOwner.lock()->get_Transform().lock();

    // シェーダセット ==========================
    Master::m_pShaderManager->DeviceToSetShader(SHADER_TYPE::FORWARD_UNLIT_STATIC);

    /* ========== 定数バッファの更新 ========== */
    XMMATRIX viewInvMtx = renderer.get_ViewInvMatrix(); // ビュー逆行列取得

    const XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMVECTOR forward = viewInvMtx.r[2];     // [2]に前方向が入ってる（0:rg1:up2:fw）
    forward = XMVectorSetY(forward, 0.0f);  // Y成分を0にする
    forward = XMVector3Normalize(forward);  // 正規化

    XMVECTOR right = XMVector3Cross(up, forward);   // 外積を使って右方向ベクトルを求める

    viewInvMtx.r[0] = right;   // X軸
    viewInvMtx.r[1] = up;      // Y軸
    viewInvMtx.r[2] = forward; // Z軸
    viewInvMtx.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);// 平行移動成分をゼロにする

    // ワールド行列セット ==========================

    // 明示的に行列を設定
    XMMATRIX worldMtx = 
        transform->get_WorldMtx(
            transform->get_MtxScale(), 
            viewInvMtx, 
            transform->get_MtxPos()
        );

    XMMATRIX mtx = XMMatrixTranspose(worldMtx);        // 行列の転置
    XMStoreFloat4x4(&cbTransSet->Data.WorldMtx, mtx);  // XMMATRIX → XMFLOAT4X4変換
    
    // 定数バッファに転送
    pContext->UpdateSubresource(
        cbTransSet->pBuff,
        0,
        nullptr,
        &cbTransSet->Data,
        0,
        0
    );

    auto pMatData = meshData->pMaterials.lock();

    // マテリアル情報セット ==========================
    CB_MATERIAL mat{};
    mat.Diffuse = pMatData->m_DiffuseColor;
    mat.Specular = pMatData->m_SpecularColor;
    mat.SpecularPower = pMatData->m_SpecularPower;
    cbMatSet->Data = mat;

    // 定数バッファに転送
    pContext->UpdateSubresource(
        cbMatSet->pBuff,
        0,
        nullptr,
        &cbMatSet->Data,
        0,
        0
    );

    // 定数バッファをセット ==========================
    pContext->VSSetConstantBuffers(0, 1, &cbTransSet->pBuff);
    pContext->PSSetConstantBuffers(4, 1, &cbMatSet->pBuff);
    //pContext->PSSetConstantBuffers(0, 1, &m_pCB3DObjectSet->pBuff);


    // テクスチャセット ==========================
    ID3D11ShaderResourceView* diffuseSRV = nullptr;
    ID3D11ShaderResourceView* normalSRV = nullptr;
    ID3D11ShaderResourceView* specularSRV = nullptr;

    if (auto tex = pMatData->m_DiffuseMap.Texture.lock()) {
        diffuseSRV = tex.get()->get_SRV();
    }
    if (auto tex = pMatData->m_NormalMap.Texture.lock()) {
        normalSRV = tex.get()->get_SRV();
    }
    if (auto tex = pMatData->m_SpecularMap.Texture.lock()) {
        specularSRV = tex.get()->get_SRV();
    }

    pContext->PSSetShaderResources(0, 1, &diffuseSRV);
    //pContext->PSSetShaderResources(1, 1, &normalSRV);
    //pContext->PSSetShaderResources(2, 1, &specularSRV);

    // 頂点＆インデックスバッファ設定 ==========================
    UINT stride = sizeof(VERTEX_Static);
    UINT offset = 0;
    pContext->IASetVertexBuffers(0, 1, &vtxBuff, &stride, &offset); // 頂点バッファをセット
    pContext->IASetIndexBuffer(idxBuff, DXGI_FORMAT_R16_UINT, 0);    // インデックスバッファをセット
    pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);// Set primitive topology 頂点の組み合わせ方

    //ブレンドステート設定 ==========================
    Master::m_pBlendManager->DeviceToSetBlendState(pMatData->m_BlendMode);

    // 描画コール：インデックス数は（三角形個 × 3頂点） ==========================
    pContext->DrawIndexed(meshData->NumIndex, 0, 0);

    Master::m_pBlendManager->DeviceToSetBlendState(BLEND_MODE::NONE);
}


//*---------------------------------------------------------------------------------------
//* @:BillboardRenderer Class 
//*【?】BillboardResource参照用のポインタ設定
//* 引数：1.BillboardResource
//* 返値：void
//*----------------------------------------------------------------------------------------
void BillboardRenderer::set_BillboardResource(std::weak_ptr<BillboardResource> billboardResource)
{
    m_pResource = billboardResource;
}

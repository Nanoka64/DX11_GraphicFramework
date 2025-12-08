#include "pch.h"
#include "Component_BillboardRenderer.h"
#include "Component_BillboardResource.h"
#include "RendererEngine.h"
#include "GameObject.h"
#include "Texture.h"
#include "BlendManager.h"


using namespace BASE_VERTEX;

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
void BillboardRenderer::Init(RendererEngine& renderer)
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
    MeshInfo* meshInfo = m_pResource.lock()->m_pMeshInfo;
    CB_TRANSFORM_SET* cbTransSet = m_pResource.lock()->m_pCBTransformSet;
    CB_MATERIAL_SET* cbMatSet = m_pResource.lock()->m_pCBMaterialDataSet;
    ID3D11Buffer* vtxBuff = m_pResource.lock()->m_pVertexBuffer;
    ID3D11Buffer* idxBuff = m_pResource.lock()->m_pIndexBuffer;

    // シェーダセット ==========================
    Master::m_pShaderManager->DeviceToSetShader(SHADER_TYPE::DEFFERD_STANDARD_SIMPLE);


    /* ========== 定数バッファの更新 ========== */

    // ワールド行列セット ==========================
    XMMATRIX worldMtx = m_pOwner.lock()->get_Transform().lock()->get_WorldMtx();
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


    // マテリアル情報セット ==========================
    CB_MATERIAL mat{};
    mat.Diffuse = meshInfo->pMaterials[0].DiffuseColor;
    mat.Specular = meshInfo->pMaterials[0].SpecularColor;
    mat.Normal = meshInfo->pMaterials[0].NormalColor;
    mat.SpecularPower = meshInfo->pMaterials[0].SpecularPower;
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
    if (auto tex = meshInfo->pMaterials->Diffuse.Texture.lock()) {
        diffuseSRV = tex.get()->get_SRV();
    }
    if (auto tex = meshInfo->pMaterials->Normal.Texture.lock()) {
        normalSRV = tex.get()->get_SRV();
    }
    if (auto tex = meshInfo->pMaterials->Specular.Texture.lock()) {
        specularSRV = tex.get()->get_SRV();
    }

    pContext->PSSetShaderResources(0, 1, &diffuseSRV);
    //pContext->PSSetShaderResources(1, 1, &normalSRV);
    pContext->PSSetShaderResources(2, 1, &specularSRV);

    // 頂点＆インデックスバッファ設定 ==========================
    UINT stride = sizeof(VERTEX);
    UINT offset = 0;
    pContext->IASetVertexBuffers(0, 1, &vtxBuff, &stride, &offset); // 頂点バッファをセット
    pContext->IASetIndexBuffer(idxBuff, DXGI_FORMAT_R16_UINT, 0);    // インデックスバッファをセット
    pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);// Set primitive topology 頂点の組み合わせ方

    // 描画コール：インデックス数は（三角形個 × 3頂点） ==========================
    pContext->DrawIndexed(meshInfo->NumIndex, 0, 0);
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

#include "pch.h"
#include "Component_SkyRenderer.h"
#include "pch.h"
#include "GameObject.h"
#include "RendererEngine.h"
#include "Component_IMeshResource.h"
#include "Component_MeshRenderer.h"
#include "Texture.h"
#include "BlendManager.h"

using namespace DirectX;
using namespace VERTEX;
using namespace Tool::UV;

//*---------------------------------------------------------------------------------------
//* @:SkyRenderer Class 
//*【?】コンストラクタ
//* 引数：1.オーナーオブジェクト
//* 引数：2.更新レイヤー
//*----------------------------------------------------------------------------------------
SkyRenderer::SkyRenderer(std::weak_ptr<GameObject> pOwner, int updateRank) : IComponent(pOwner, updateRank),
m_GeometryType(SKY_GEOMETRY_TYPE::BOX)
{
    this->set_Tag("SkyRenderer");
}


//*---------------------------------------------------------------------------------------
//* @:SkyRenderer Class 
//*【?】デストラクタ
//*----------------------------------------------------------------------------------------
SkyRenderer::~SkyRenderer()
{

}


//*---------------------------------------------------------------------------------------
//* @:SkyRenderer Class 
//*【?】初期化
//* 引数：1.RendererEngine
//* 返値：void
//*----------------------------------------------------------------------------------------
void SkyRenderer::Init(RendererEngine& renderer)
{

}


//*---------------------------------------------------------------------------------------
//* @:SkyRenderer Class 
//*【?】更新
//* 引数：1.RendererEngine
//* 返値：void
//*----------------------------------------------------------------------------------------
void SkyRenderer::Update(RendererEngine& renderer)
{

}


//*---------------------------------------------------------------------------------------
//* @:SkyRenderer Class 
//*【?】描画
//* 引数：1.RendererEngine
//* 返値：void
//*----------------------------------------------------------------------------------------
void SkyRenderer::Draw(RendererEngine& renderer)
{
    auto pContext = renderer.get_DeviceContext();
    auto meshData = m_pMeshResource.lock()->m_pMeshData;
    CB_TRANSFORM_SET* cbTransSet = m_pMeshResource.lock()->m_pCBTransformSet;
    CB_MATERIAL_SET* cbMatSet = m_pMeshResource.lock()->m_pCBMaterialDataSet;
    ID3D11Buffer* vtxBuff = meshData->pVertexBuffer;
    ID3D11Buffer* idxBuff = meshData->pIndexBuffer;

    // シェーダセット ==========================
    Master::m_pShaderManager->DeviceToSetShader(m_pMeshResource.lock()->get_ShaderType());

    /* ========== 定数バッファの更新 ========== */
    XMMATRIX viewMtx = renderer.get_ViewMatrix();
    viewMtx.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);// 平行移動成分をゼロにする

    // ワールド行列セット ==========================
    XMMATRIX worldMtx = m_pOwner.lock()->get_Transform().lock()->get_ExcludingRotWorldMtx();
    worldMtx *= viewMtx;
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

    // 定数バッファをセット ==========================
    pContext->VSSetConstantBuffers(0, 1, &cbTransSet->pBuff);
    //pContext->PSSetConstantBuffers(4, 1, &cbMatSet->pBuff);   // マテリアルは今のところ必要なし

    // テクスチャセット ==========================
    ID3D11ShaderResourceView* diffuseSRV = nullptr;

    // 今のところディフューズのみ
    if (auto tex = meshData->pMaterials->m_DiffuseMap.Texture.lock()) {
        diffuseSRV = tex.get()->get_SRV();
    }
    pContext->PSSetShaderResources(0, 1, &diffuseSRV);

    // 頂点＆インデックスバッファ設定 ==========================
    UINT stride = sizeof(VERTEX_Static);
    UINT offset = 0;
    pContext->IASetVertexBuffers(0, 1, &vtxBuff, &stride, &offset);         // 頂点バッファをセット
    pContext->IASetIndexBuffer(idxBuff, DXGI_FORMAT_R16_UINT, 0);           // インデックスバッファをセット
    pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);// Set primitive topology 頂点の組み合わせ方

    // 描画コール：インデックス数は（三角形個 × 3頂点） ==========================
    pContext->DrawIndexed(meshData->NumIndex, 0, 0);

}


//*---------------------------------------------------------------------------------------
//* @:SkyRenderer Class 
//*【?】IMeshResource参照用のポインタ設定
//* 引数：1.IMeshResource
//* 返値：void
//*----------------------------------------------------------------------------------------
void SkyRenderer::set_MeshResource(std::weak_ptr<class IMeshResource> meshResource)
{
    m_pMeshResource = meshResource;
}




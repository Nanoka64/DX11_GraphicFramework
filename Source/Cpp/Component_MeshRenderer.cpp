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
//* @:MeshRenderer Class 
//*【?】コンストラクタ
//* 引数：1.オーナーオブジェクト
//* 引数：2.更新レイヤー
//*----------------------------------------------------------------------------------------
MeshRenderer::MeshRenderer(std::weak_ptr<GameObject> pOwner, int updateRank) : IComponent(pOwner,updateRank)
{
    this->set_Tag("MeshRenderer");
}


//*---------------------------------------------------------------------------------------
//* @:MeshRenderer Class 
//*【?】デストラクタ
//*----------------------------------------------------------------------------------------
MeshRenderer::~MeshRenderer()
{

}


//*---------------------------------------------------------------------------------------
//* @:MeshRenderer Class 
//*【?】初期化
//* 引数：1.RendererEngine
//* 返値：void
//*----------------------------------------------------------------------------------------
void MeshRenderer::Init(RendererEngine& renderer)
{

}


//*---------------------------------------------------------------------------------------
//* @:MeshRenderer Class 
//*【?】更新
//* 引数：1.RendererEngine
//* 返値：void
//*----------------------------------------------------------------------------------------
void MeshRenderer::Update(RendererEngine& renderer)
{

}


//*---------------------------------------------------------------------------------------
//* @:MeshRenderer Class 
//*【?】描画
//* 引数：1.RendererEngine
//* 返値：void
//*----------------------------------------------------------------------------------------
void MeshRenderer::Draw(RendererEngine& renderer)
{
    auto pContext = renderer.get_DeviceContext();
    auto meshInfo = m_pMeshResource.lock()->m_pMeshData;
    CB_TRANSFORM_SET* cbTransSet =  m_pMeshResource.lock()->m_pCBTransformSet;
    CB_MATERIAL_SET* cbMatSet =  m_pMeshResource.lock()->m_pCBMaterialDataSet;
    ID3D11Buffer* vtxBuff = meshInfo->pVertexBuffer;
    UINT vtxStride = meshInfo->VertexStride;
    ID3D11Buffer* idxBuff = meshInfo->pIndexBuffer;

    // シェーダセット ==========================
    Master::m_pShaderManager->DeviceToSetShader(m_pMeshResource.lock()->get_ShaderType());


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
    pContext->PSSetShaderResources(1, 1, &normalSRV);
    pContext->PSSetShaderResources(2, 1, &specularSRV);

    // 頂点＆インデックスバッファ設定 ==========================
    UINT offset = 0;
    pContext->IASetVertexBuffers(0, 1, &vtxBuff, &vtxStride, &offset); // 頂点バッファをセット
    pContext->IASetIndexBuffer(idxBuff, DXGI_FORMAT_R16_UINT, 0);    // インデックスバッファをセット
    pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);// Set primitive topology 頂点の組み合わせ方

    // 描画コール：インデックス数は（三角形個 × 3頂点） ==========================
    pContext->DrawIndexed(meshInfo->NumIndex, 0, 0);

}


//*---------------------------------------------------------------------------------------
//* @:MeshRenderer Class 
//*【?】IMeshResource参照用のポインタ設定
//* 引数：1.IMeshResource
//* 返値：void
//*----------------------------------------------------------------------------------------
void MeshRenderer::set_MeshResource(std::weak_ptr<class IMeshResource> meshResource)
{
    m_pMeshResource = meshResource;
}




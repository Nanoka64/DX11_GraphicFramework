#include "pch.h"
#include "Component_DecalRenderer.h"
#include "RendererEngine.h"
#include "GameObject.h"
#include "Component_IMeshResource.h"
#include "Texture.h"
#include "BlendManager.h"

using namespace VERTEX;

//*---------------------------------------------------------------------------------------
//*【?】コンストラクタ
//* [引数]
//* pOwner : オーナーオブジェクト
//* updateRank : 更新レイヤー
//*----------------------------------------------------------------------------------------
DecalRenderer::DecalRenderer(std::weak_ptr<GameObject> pOwner, int updateRank) 
    : IComponent(pOwner, updateRank),
    m_IsDynamic(false),
    m_pCBDecalSet(nullptr)
{
	this->set_Tag("DecalRenderer");
}


//*---------------------------------------------------------------------------------------
//*【?】デストラクタ
//*----------------------------------------------------------------------------------------
DecalRenderer::~DecalRenderer()
{
    if (m_pCBDecalSet) {
        if (m_pCBDecalSet->pBuff) {
            m_pCBDecalSet->pBuff->Release();
        }
        delete m_pCBDecalSet;
        m_pCBDecalSet = nullptr;
    }
}


//*---------------------------------------------------------------------------------------
//*【?】初期化
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//* [返値]なし
//*----------------------------------------------------------------------------------------
void DecalRenderer::Start(RendererEngine &renderer)
{
    // デカール用定数バッファ作成
    if (!CreateDecalConstantBuffer(renderer))
    {
        MessageBox(NULL, "デカール用定数バッファが作成できませんでした", "Error", MB_OK);
    }
}


//*---------------------------------------------------------------------------------------
//*【?】更新
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//* [返値]なし
//*----------------------------------------------------------------------------------------
void DecalRenderer::Update(RendererEngine &renderer)
{

}


//*---------------------------------------------------------------------------------------
//*【?】描画
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//* [返値]なし
//*----------------------------------------------------------------------------------------
void DecalRenderer::Draw(RendererEngine &renderer)
{
    // シャドウパスは返す
    if (renderer.get_CrntRenderPass() == RENDER_PASS::SHADOW) {
        return;
    }

    auto pContext = renderer.get_DeviceContext();
    std::shared_ptr<MeshResourceData> meshInfo = m_pMeshResource.lock()->m_pMeshData;
    CB_TRANSFORM_SET *cbTransSet = m_pMeshResource.lock()->m_pCBTransformSet;
    CB_MATERIAL_SET *cbMatSet = m_pMeshResource.lock()->m_pCBMaterialDataSet;
    ID3D11Buffer *vtxBuff = meshInfo->pVertexBuffer;
    UINT vtxStride = meshInfo->VertexStride;
    ID3D11Buffer *idxBuff = meshInfo->pIndexBuffer;

    /* ========== 定数バッファの更新 ========== */
    // ワールド行列セット ==========================
    XMMATRIX worldMtx = m_pOwner.lock()->get_Transform().lock()->get_WorldMtx();
    worldMtx = XMMatrixTranspose(worldMtx);                 // 行列の転置
    XMStoreFloat4x4(&cbTransSet->Data.WorldMtx, worldMtx);  // XMMATRIX → XMFLOAT4X4変換

    // 定数バッファに転送
    pContext->UpdateSubresource(
        cbTransSet->pBuff,
        0,
        nullptr,
        &cbTransSet->Data,
        0,
        0
    );

    // デカール情報セット ==========================
    // GPUメモリにアクセス
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    if (SUCCEEDED(pContext->Map(m_pCBDecalSet->pBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
    {
        // ワールド逆行列
        XMMATRIX invWorld = XMMatrixInverse(NULL, worldMtx);
        XMStoreFloat4x4(&m_pCBDecalSet->Data.DecalWorldInvMtx, invWorld);

        // データのコピー 
        memcpy(mappedResource.pData, &m_pCBDecalSet->Data, sizeof(CB_DECAL));
        // アクセス終了
        pContext->Unmap(m_pCBDecalSet->pBuff, 0);
    }

    // 通常パス **********************************************************
    if (renderer.get_CrntRenderPass() == RENDER_PASS::MAIN) {
        Master::m_pShaderManager->DeviceToSetShader(SHADER_TYPE::DEFERRED_STD_DECAL);

        // マテリアル取得
        auto pMatData = meshInfo->pMaterials.lock();

        // マテリアル情報セット ==========================
        CB_MATERIAL mat{};
        mat.Diffuse = pMatData->m_DiffuseColor;
        mat.Specular = pMatData->m_SpecularColor;
        mat.SpecularPower = pMatData->m_SpecularPower;
        mat.EmissivePower = pMatData->m_EmissivePower;
        mat.EmissiveColor = pMatData->m_EmissiveColor;
        mat.OffsetUV;
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
        pContext->VSSetConstantBuffers(0, 1, &cbTransSet->pBuff);       // ワールド
        pContext->PSSetConstantBuffers(11,1, &m_pCBDecalSet->pBuff);    // デカール
        pContext->VSSetConstantBuffers(4, 1, &cbMatSet->pBuff);         // マテリアル
        pContext->PSSetConstantBuffers(4, 1, &cbMatSet->pBuff);         // ,,,

        // テクスチャセット（アルベドと法線のみ） ==========================
        ID3D11ShaderResourceView *diffuseSRV = nullptr;
        ID3D11ShaderResourceView *normalSRV = nullptr;
        if (auto tex = pMatData->m_DiffuseMap.Texture.lock()) {
            diffuseSRV = tex.get()->get_SRV();
        }
        if (auto tex = pMatData->m_NormalMap.Texture.lock()) {
            normalSRV = tex.get()->get_SRV();
        }

        // シェーダーリソースビューをセット
        pContext->PSSetShaderResources(0, 1, &diffuseSRV);
        pContext->PSSetShaderResources(1, 1, &normalSRV);
    }

    // 頂点＆インデックスバッファ設定 ==========================
    UINT offset = 0;
    pContext->IASetVertexBuffers(0, 1, &vtxBuff, &vtxStride, &offset);      // 頂点バッファをセット
    pContext->IASetIndexBuffer(idxBuff, DXGI_FORMAT_R16_UINT, 0);           // インデックスバッファをセット
    pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);// Set primitive topology 頂点の組み合わせ方

    // 描画コール ==========================
    pContext->DrawIndexed(meshInfo->NumIndex, 0, 0);

    pContext->PSSetShaderResources(0, 0, nullptr);
    pContext->PSSetShaderResources(1, 0, nullptr);
}

//*---------------------------------------------------------------------------------------
//*【?】デカール用定数バッファの作成
//* [引数]
//* &renderer : 描画エンジンの参照
//* [返値]なし
//*----------------------------------------------------------------------------------------
bool DecalRenderer::CreateDecalConstantBuffer(RendererEngine &renderer)
{
    auto pDevice = renderer.get_Device();

    D3D11_USAGE usage;  // 使用方法
    UINT accesssFlags;  // CPU書き込みフラグ

    // 動的変更をするか
    if (m_IsDynamic){
        usage = D3D11_USAGE_DYNAMIC;
        accesssFlags = D3D11_CPU_ACCESS_WRITE;
    }
    else{
        usage = D3D11_USAGE_DEFAULT;
        accesssFlags = 0;
    }

    m_pCBDecalSet = new CB_DECAL_SET();

    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
    bd.Usage = usage;
    bd.ByteWidth = sizeof(CB_DECAL);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = accesssFlags;

    D3D11_SUBRESOURCE_DATA initData;
    ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
    initData.pSysMem = nullptr;

    // バッファの作成
    HRESULT hr = pDevice->CreateBuffer(&bd, nullptr, &m_pCBDecalSet->pBuff);
    if (FAILED(hr))
    {
        return false;
    }

    return true;
}


//*---------------------------------------------------------------------------------------
//* @:DecalRenderer Class 
//*【?】IMeshResource参照用のポインタ設定
//* 引数：1.IMeshResource
//* 返値：void
//*----------------------------------------------------------------------------------------
void DecalRenderer::set_MeshResource(std::weak_ptr<class IMeshResource> meshResource)
{
    m_pMeshResource = meshResource;
}

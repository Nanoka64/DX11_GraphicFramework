#include "pch.h"
#include "RendererManager.h"
#include "Texture.h"
#include "GameObject.h"
#include "DirectWriteManager.h"
#include "Component_ModelMeshRenderer.h"
#include "Component_ModelMeshResource.h"


using namespace DirectX;
using namespace BASE_VERTEX;
using namespace Tool::UV;

//*---------------------------------------------------------------------------------------
//* @:ModelMeshRenderer Class 
//*【?】コンストラクタ
//* 引数：1.
//*----------------------------------------------------------------------------------------
ModelMeshRenderer::ModelMeshRenderer(std::weak_ptr<GameObject> pOwner, int updateRank) : IComponent(pOwner, updateRank),
m_IsDrawLine(false)
{
    this->set_Tag("ModelMeshRenderer");
}


//*---------------------------------------------------------------------------------------
//* @:ModelMeshRenderer Class 
//*【?】デストラクタ
//*----------------------------------------------------------------------------------------
ModelMeshRenderer::~ModelMeshRenderer()
{

}


//*---------------------------------------------------------------------------------------
//* @:ModelMeshRenderer Class 
//*【?】初期化
//* 引数：1.RendererManager
//* 返値：void
//*----------------------------------------------------------------------------------------
void ModelMeshRenderer::Init(RendererManager &renderer)
{
    // 初期化時に自身にMeshResourceコンポーネントがあれば設定する
    this->set_MeshResource(m_pOwner.lock()->get_Component<ModelMeshResource>());
}


//*---------------------------------------------------------------------------------------
//* @:ModelMeshRenderer Class 
//*【?】更新
//* 引数：1.RendererManager
//* 返値：void
//*----------------------------------------------------------------------------------------
void ModelMeshRenderer::Update(RendererManager &renderer)
{

}


//*---------------------------------------------------------------------------------------
//* @:ModelMeshRenderer Class 
//*【?】描画
//* 引数：1.RendererManager
//* 返値：void
//*----------------------------------------------------------------------------------------
void ModelMeshRenderer::Draw(RendererManager &renderer)
{
    if (m_pMeshResource.lock() == nullptr) return;

    auto pDeviceContext             = renderer.get_DeviceContext();
    std::vector<MATERIAL>matList    = m_pMeshResource.lock()->get_ModelData().lock()->get_MaterialList();
    UINT meshNum                    = m_pMeshResource.lock()->get_ModelData().lock()->get_MeshNum();
    const aiScene *pScene           = m_pMeshResource.lock()->get_ModelData().lock()->get_Scene();
    ModelMesh *pMeshes              = m_pMeshResource.lock()->get_ModelData().lock()->get_Meshes();
    CB_MATERIAL_SET *CB_MatSet      = m_pMeshResource.lock()->get_ModelData().lock()->GetConstantBufferMaterialDataSet();
    CB_TRANSFORM_SET *CB_TransSet   = m_pMeshResource.lock()->get_ModelData().lock()->GetConstantBufferTransformSet();


    // トランスフォームのセット ==========================
    XMMATRIX worldMtx = m_pOwner.lock()->get_Component<Transform>()->get_WorldMtx();
    worldMtx = XMMatrixTranspose(worldMtx);
    XMStoreFloat4x4(&CB_TransSet->Data.WorldMtx, worldMtx);  // XMMATRIX → XMFLOAT4X4変換

    // 定数バッファに転送
    pDeviceContext->UpdateSubresource(
        CB_TransSet->pBuff,
        0,
        nullptr,
        &CB_TransSet->Data,
        0,
        0
    );

    // トランスフォーム用定数バッファのセット
    pDeviceContext->VSSetConstantBuffers(0, 1, &CB_TransSet->pBuff);


    // マテリアル情報セット ==========================
    CB_MATERIAL mat{};
    mat.Diffuse         = matList[0].DiffuseColor;
    mat.Specular        = matList[0].SpecularColor;
    mat.Normal          = matList[0].NormalColor;
    mat.SpecularPower   = matList[0].SpecularPower;
    CB_MatSet->Data     = mat;

    // 定数バッファに転送
    pDeviceContext->UpdateSubresource(
        CB_MatSet->pBuff,
        0,
        nullptr,
        &CB_MatSet->Data,
        0,
        0
    );

    // マテリアル用定数バッファのセット
    pDeviceContext->VSSetConstantBuffers(4, 1, &CB_MatSet->pBuff);

    // モデルシェーダに切り替え
    ShaderManager::Instance().DeviceToSetShader(SHADER_TYPE::MODEL);


    /* デバッグモード指定の場合、ワイヤーフレームで表示 */
    if (m_IsDrawLine) {
        pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
    }
    else {
        pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    }


    // メッシュの描画
    for (u_int meshIdx = 0; meshIdx < meshNum; meshIdx++)
    {
        aiMesh *mesh = pScene->mMeshes[meshIdx];
        auto &mat = matList[mesh->mMaterialIndex];

        /* ビューの設定 */
        {
            // ディフューズ
            if (mat.Diffuse.Texture.lock() != nullptr) {
                auto diff = mat.Diffuse.Texture.lock()->get_SRV();
                if (diff != nullptr)
                    pDeviceContext->PSSetShaderResources(0, 1, &diff);
            }

            // ノーマル
            if (mat.Normal.Texture.lock() != nullptr) {
                auto norm = mat.Normal.Texture.lock()->get_SRV();
                if (norm != nullptr)
                    pDeviceContext->PSSetShaderResources(1, 1, &norm);
            }
            // スペキュラ
            if (mat.Specular.Texture.lock() != nullptr) {
                auto spec = mat.Specular.Texture.lock()->get_SRV();
                if (spec != nullptr)
                    pDeviceContext->PSSetShaderResources(2, 1, &spec);
            }
        }
        pMeshes[meshIdx].Draw(renderer);
    }
}


//*---------------------------------------------------------------------------------------
//* @:ModelMeshRenderer Class 
//*【?】IMeshResource参照用のポインタ設定
//* 引数：1.IMeshResource
//* 返値：void
//*----------------------------------------------------------------------------------------
void ModelMeshRenderer::set_MeshResource(std::weak_ptr<class ModelMeshResource> meshResource)
{
    m_pMeshResource = meshResource;
}



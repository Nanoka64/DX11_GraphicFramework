#include "pch.h"
#include "RendererEngine.h"
#include "Texture.h"
#include "GameObject.h"
#include "DirectWriteManager.h"
#include "Component_ModelMeshRenderer.h"
#include "Component_ModelMeshResource.h"


using namespace DirectX;
using namespace VERTEX;
using namespace Tool::UV;

//*---------------------------------------------------------------------------------------
//* @:ModelMeshRenderer Class 
//*【?】コンストラクタ
//* 引数：1.オーナーオブジェクト
//* 引数：2.更新レイヤー
//*----------------------------------------------------------------------------------------
ModelMeshRenderer::ModelMeshRenderer(std::weak_ptr<GameObject> pOwner, int updateRank) : IComponent(pOwner, updateRank),
m_IsDrawLine(false),
m_DebugDrawBoneNum(0)
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
//* 引数：1.RendererEngine
//* 返値：void
//*----------------------------------------------------------------------------------------
void ModelMeshRenderer::Init(RendererEngine &renderer)
{
    // 初期化時に自身にMeshResourceコンポーネントがあれば設定する
    this->set_MeshResource(m_pOwner.lock()->get_Component<ModelMeshResource>());
}


//*---------------------------------------------------------------------------------------
//* @:ModelMeshRenderer Class 
//*【?】更新
//* 引数：1.RendererEngine
//* 返値：void
//*----------------------------------------------------------------------------------------
void ModelMeshRenderer::Update(RendererEngine &renderer)
{

}


//*---------------------------------------------------------------------------------------
//* @:ModelMeshRenderer Class 
//*【?】描画
//* 引数：1.RendererEngine
//* 返値：void
//*----------------------------------------------------------------------------------------
void ModelMeshRenderer::Draw(RendererEngine &renderer)
{
    if (m_pMeshResource.lock() == nullptr) return;
    auto modelData = m_pMeshResource.lock()->get_ModelData().lock();
    auto pDeviceContext             = renderer.get_DeviceContext();
    std::vector<Material>matList    = modelData->get_MaterialList();
    const aiScene *pScene           = modelData->get_Scene();
    ModelMesh *pMeshes              = modelData->get_Meshes();
    UINT meshNum                    = modelData->get_MeshNum();
    CB_MATERIAL_SET *CB_MatSet      = modelData->GetConstantBufferMaterialDataSet();
    CB_TRANSFORM_SET *CB_TransSet   = modelData->GetConstantBufferTransformSet();
    UINT vertexNum                  = pMeshes->get_VertexNum();
    VERTEX_Skneed* vertices         = pMeshes->get_Vertices();
    SHADER_TYPE shaderType          = modelData->get_ShaderType();
    SHADER_TYPE shadowShaderType    = modelData->get_ShadowShaderType();


    /* デバッグモード指定の場合、ワイヤーフレームで表示 */
    if (m_IsDrawLine) {
        pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
    }
    else {
        pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    }

    // ワールド行列更新 ==========================
    XMMATRIX worldMtx = m_pOwner.lock()->get_Component<Transform>()->get_WorldMtx();
    worldMtx = XMMatrixTranspose(worldMtx);
    XMStoreFloat4x4(&CB_TransSet->Data.WorldMtx, worldMtx);  // XMMATRIX → XMFLOAT4X4変換

    // バッファの更新
    pDeviceContext->UpdateSubresource(
        CB_TransSet->pBuff,
        0,
        nullptr,
        &CB_TransSet->Data,
        0,
        0
    );

    // 通常パス **********************************************************
    if (renderer.get_CrntRenderPass() == RENDER_PASS::MAIN) {
        Master::m_pShaderManager->DeviceToSetShader(shaderType);

        // マテリアル情報セット ==========================
        CB_MATERIAL mat{};
        mat.Diffuse = matList[0].m_DiffuseColor;
        mat.Specular = matList[0].m_SpecularColor;
        mat.SpecularPower = matList[0].m_SpecularPower;
        CB_MatSet->Data = mat;

        // 定数バッファに転送
        pDeviceContext->UpdateSubresource(
            CB_MatSet->pBuff,
            0,
            nullptr,
            &CB_MatSet->Data,
            0,
            0
        );

        // トランスフォーム用定数バッファのセット
        pDeviceContext->VSSetConstantBuffers(0, 1, &CB_TransSet->pBuff);
        pDeviceContext->PSSetConstantBuffers(4, 1, &CB_MatSet->pBuff);

        // メッシュの描画
        for (u_int meshIdx = 0; meshIdx < meshNum; meshIdx++)
        {
            aiMesh *mesh = pScene->mMeshes[meshIdx];
            auto &mat = matList[mesh->mMaterialIndex];

            /* ビューの設定 */
            {
                // ディフューズ
                if (mat.m_DiffuseMap.Texture.lock() != nullptr) {
                    auto diff = mat.m_DiffuseMap.Texture.lock()->get_SRV();
                    if (diff != nullptr)
                        pDeviceContext->PSSetShaderResources(0, 1, &diff);
                }

                // ノーマル
                if (mat.m_NormalMap.Texture.lock() != nullptr) {
                    auto norm = mat.m_NormalMap.Texture.lock()->get_SRV();
                    if (norm != nullptr)
                        pDeviceContext->PSSetShaderResources(1, 1, &norm);
                }
                // スペキュラ
                if (mat.m_SpecularMap.Texture.lock() != nullptr) {
                    auto spec = mat.m_SpecularMap.Texture.lock()->get_SRV();
                    if (spec != nullptr)
                        pDeviceContext->PSSetShaderResources(2, 1, &spec);
                }
            }
            pMeshes[meshIdx].Draw(renderer);
        }
    }
    // シャドウパス **********************************************************
    else if (renderer.get_CrntRenderPass() == RENDER_PASS::SHADOW) {
        if (pScene->HasAnimations()){
        }
        else{
            Master::m_pShaderManager->DeviceToSetShader(SHADER_TYPE::POST_SHADOWMAP);
        }
            Master::m_pShaderManager->DeviceToSetShader(SHADER_TYPE::POST_SHADOWMAP_SKINNED);
        // トランスフォーム用定数バッファのセット
        pDeviceContext->VSSetConstantBuffers(0, 1, &CB_TransSet->pBuff);

        // メッシュの描画 マテリアル等なし
        for (u_int meshIdx = 0; meshIdx < meshNum; meshIdx++)
        {
            aiMesh *mesh = pScene->mMeshes[meshIdx];
            pMeshes[meshIdx].Draw(renderer);
        }
    }

    // ImGUI
    Master::m_pDebugger->BeginDebugWindow(m_pOwner.lock()->get_Tag());
    Master::m_pDebugger->DG_SliderInt("DrawBoneNum", 1, &m_DebugDrawBoneNum, 10, vertexNum);
    for (size_t i = 0; i < std::min<size_t>(vertexNum, m_DebugDrawBoneNum); i++)
    {
        for (size_t j = 0; j < 4; j++)
        {
            if (vertices[i].boneIDs[j] == 0)
            {
                Master::m_pDebugger->DG_TextValue("Vertex%d============================", i);

                Master::m_pDebugger->DG_TextValue("BoneIds : %d", vertices[i].boneIDs[j]);
                Master::m_pDebugger->DG_TextValue("Weight  : %f.2", vertices[i].boneWeights[j]);
            }
        }
    }
    Master::m_pDebugger->EndDebugWindow();
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



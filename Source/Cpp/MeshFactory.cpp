#include "pch.h"
#include "MeshFactory.h"
#include "GameObjectManager.h"
#include "GameObject.h"
#include "RendererEngine.h"
#include "Component_ModelMeshResource.h"
#include "Component_ModelMeshRenderer.h"
#include "Component_MeshRenderer.h"
#include "Component_SkinnedMeshAnimator.h"
#include "Component_SpriteRenderer.h"
#include "Component_BillboardRenderer.h"
#include "Component_BillboardResource.h"
#include "ResourceManager.h"
#include "Texture.h"

using namespace GIGA_Engine;

//*---------------------------------------------------------------------------------------
//* @:MeshFactory Class 
//*【?】モデルの生成
//* 引数：1.CreateModelInfo&
//* 返値：std::weak_ptr<GameObject>
//*----------------------------------------------------------------------------------------
std::weak_ptr<class GameObject> MeshFactory::CreateModel(const CreateModelInfo &info)
{
    // モデルの読み込み
    std::weak_ptr<ModelData> modeldata = ResourceManager::Instance().LoadModel(info.Path.c_str());
    if (modeldata.lock() == nullptr)return{};

    // オブジェクトの生成
    std::weak_ptr<GameObject> pModelObj = Instantiate(std::move(std::make_shared<GameObject>()));
    pModelObj.lock()->Init(*info.pRenderer);
    pModelObj.lock()->set_Tag(info.ObjTag.c_str());


    // オブジェクトの状態をアクティブにする
    if (info.IsActive) {
        pModelObj.lock()->set_StatusFlag(OBJECT_STATUS_BITFLAG::IS_ACTIVE);
    }

    // メッシュリソースコンポーネントを追加
    std::weak_ptr<ModelMeshResource> meshResource = pModelObj.lock()->add_Component<ModelMeshResource>();


    // マテリアルの設定
    for (size_t i = 0; i < info.MatNum; i++)
    {
        if (!modeldata.lock()->SetupTextureMap(*info.MaterialData[i].pMat,info.MaterialData[i].MatIndex))
        {
            MessageBox(NULL, "マテリアルが設定できませんでした", "Error", MB_OK);
        }
    }
    
    // 使用するシェーダの設定
    modeldata.lock()->set_ShaderType(info.ShaderType);

    // リソースにモデル情報を持たせる
    meshResource.lock()->set_ModelData(modeldata);

    // 描画コンポーネント追加
    std::weak_ptr<ModelMeshRenderer> meshRenderer = pModelObj.lock()->add_Component<ModelMeshRenderer>();
    meshRenderer.lock()->set_MeshResource(meshResource);

    // スキニングモデルの場合、アニメーションコンポーネントを追加
    if (info.ShaderType == SHADER_TYPE::DEFFERD_STANDARD_SKINNED)
    {
        // アニメーションコンポーネント追加
        if (info.IsAnim) {
            // Renderer よりも更新を速めに（シェーダに渡る情報がおかしくなるため）
            pModelObj.lock()->add_Component<SkinnedMeshAnimator>(99)->set_MeshResource(meshResource);;
            pModelObj.lock()->get_Component<SkinnedMeshAnimator>()->Init(*info.pRenderer);
            pModelObj.lock()->get_Component<SkinnedMeshAnimator>()->set_AnimIndex(info.InitAnimIndex);
        }
    }
    return pModelObj;
}



//*---------------------------------------------------------------------------------------
//* @:MeshFactory Class 
//*【?】汎用メッシュの生成
//* 引数：1.CreateUtilityMeshInfo&
//* 返値：std::weak_ptr<GameObject>
//*----------------------------------------------------------------------------------------
std::weak_ptr<class GameObject> MeshFactory::CreateUtilityMesh(const CreateUtilityMeshInfo& info)
{
    // オブジェクトの生成
    std::weak_ptr<GameObject> pObj = Instantiate(std::move(std::make_shared<GameObject>()));
    pObj.lock()->Init(*info.pRenderer);
    pObj.lock()->set_Tag(info.ObjTag.c_str());

    // オブジェクトの状態をアクティブにする
    if (info.IsActive) {
        pObj.lock()->set_StatusFlag(OBJECT_STATUS_BITFLAG::IS_ACTIVE);
    }

    // 種別がなければそのままのオブジェクトを返す
    if (info.Type == UTILITY_MESH_TYPE::NONE)return pObj;

    // コンポーネントの追加
    auto meshResource = pObj.lock()->add_Component<IMeshResource>();
    auto meshRenderer = pObj.lock()->add_Component<MeshRenderer>();

    // リソースのセットアップ
    if (!meshResource->Setup(*info.pRenderer,info.ShaderType, info.Type, info.MaterialData->pMat, info.MatNum))return {};
    
    // Rendererにリソースを設定
    meshRenderer->set_MeshResource(meshResource);

    return pObj;
}


//*---------------------------------------------------------------------------------------
//* @:MeshFactory Class 
//*【?】スプライトの生成
//* 引数：1.CreateSpriteInfo&
//* 返値：std::weak_ptr<GameObject>
//*----------------------------------------------------------------------------------------
std::weak_ptr<class GameObject> MeshFactory::CreateSprite(const CreateSpriteInfo &info)
{
    // オブジェクトの生成
    std::weak_ptr<GameObject> pObj = Instantiate(std::move(std::make_shared<GameObject>()));
    pObj.lock()->Init(*info.pRenderer);
    pObj.lock()->set_Tag(info.ObjTag.c_str());

    // オブジェクトの状態をアクティブにする
    if (info.IsActive) {
        pObj.lock()->set_StatusFlag(OBJECT_STATUS_BITFLAG::IS_ACTIVE);
    }

    // コンポーネントの追加
    auto sprite = pObj.lock()->add_Component<SpriteRenderer>();

    // リソースのセットアップ
    if (!sprite->Setup(info))return {};

    return pObj;
}


//*---------------------------------------------------------------------------------------
//* @:MeshFactory Class 
//*【?】ビルボードの生成
//* 引数：1.CreateBillboradInfo&
//* 返値：std::weak_ptr<GameObject>
//*----------------------------------------------------------------------------------------
std::weak_ptr<class GameObject> MeshFactory::CreateBillboard(const CreateBillboradInfo& info)
{
    // オブジェクトの生成
    std::weak_ptr<GameObject> pObj = Instantiate(std::move(std::make_shared<GameObject>()));
    pObj.lock()->Init(*info.pRenderer);
    pObj.lock()->set_Tag(info.ObjTag.c_str());

    // オブジェクトの状態をアクティブにする
    if (info.IsActive) {
        pObj.lock()->set_StatusFlag(OBJECT_STATUS_BITFLAG::IS_ACTIVE);
    }

    // コンポーネントの追加
    auto billboardResource = pObj.lock()->add_Component<BillboardResource>();
    auto billboardRenderer = pObj.lock()->add_Component<BillboardRenderer>();

    // リソースのセットアップ
    if (!billboardResource->Setup(*info.pRenderer, info.Type, info.MaterialData->pMat, info.MatNum))return {};

    // Rendererにリソースを設定
    billboardRenderer->set_BillboardResource(billboardResource);

    return pObj;
}

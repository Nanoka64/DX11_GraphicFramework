#include "pch.h"
#include "ModelFactory.h"
#include "GameObjectManager.h"
#include "GameObject.h"
#include "RendererManager.h"
#include "Component_ModelMeshResource.h"
#include "Component_ModelMeshRenderer.h"
#include "Component_SkinnedMeshAnimator.h"
#include "ResourceManager.h"
#include "Texture.h"


//*---------------------------------------------------------------------------------------
//* @:ModelFactory Class 
//*【?】モデルの生成
//* 引数：1.CreateModelInfo&
//* 返値：std::weak_ptr<GameObject>
//*----------------------------------------------------------------------------------------
std::weak_ptr<class GameObject> ModelFactory::CreateModel(const CreateModelInfo &info)
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
    
    // リソースにモデル情報を持たせる
    meshResource.lock()->set_ModelData(modeldata);


    // 描画コンポーネント追加
    std::weak_ptr<ModelMeshRenderer> meshRenderer = pModelObj.lock()->add_Component<ModelMeshRenderer>();
    meshRenderer.lock()->set_MeshResource(meshResource);


    // アニメーションコンポーネント追加
    if (info.IsAnim) {
        pModelObj.lock()->add_Component<SkinnedMeshAnimator>()->set_MeshResource(meshResource);;
        pModelObj.lock()->get_Component<SkinnedMeshAnimator>()->Init(*info.pRenderer);
    }

    return pModelObj;
}

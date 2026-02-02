#include "pch.h"
#include "c_Game_LoadProcess.h"
#include "ResourceManager.h"
#include "RendererEngine.h"
#include "SceneStateEnums.h"
#include "GameObject.h"
#include "InputFactory.h"
#include "Component_3DCamera.h"
#include "Component_PlayerController.h"
#include "Component_SkinnedMeshAnimator.h"
#include "Component_ModelMeshResource.h"
#include "Component_ModelMeshRenderer.h"
#include "Component_MeshRenderer.h"
#include "MeshFactory.h"
#include "Component_DirectionalLight.h"
#include "Component_PointLight.h"
#include "Component_SpriteRenderer.h"
#include "Component_BillboardRenderer.h"
#include "Component_SkyRenderer.h"
#include "Component_AssultRifle.h"
#include "Component_BoxCollider.h"
#include "Component_SphereCollider.h"
#include "Component_TrailRenderer.h"
#include "Component_EnemyController.h"
#include "Component_Health.h"

using namespace SceneStateEnums;
using namespace VECTOR4;
using namespace VECTOR3;
using namespace VECTOR2;
//*---------------------------------------------------------------------------------------
//* @:c_Game_LoadProcess Class 
//*【?】開始
//* 引数：1.SceneManager
//* 返値：void
//*----------------------------------------------------------------------------------------
void c_Game_LoadProcess::OnEnter(SceneManager *pOwner)
{
	// ロード画面用スプライトをオンに
	auto obj = Master::m_pGameObjectManager->get_ObjectByTag("LoadScreen_Sp");
	if (obj)
	{
		obj->set_StatusFlag(OBJECT_STATUS_BITFLAG::IS_ACTIVE);
	}
}


//*---------------------------------------------------------------------------------------
//* @:c_Game_LoadProcess Class 
//*【?】終了
//* 引数：1.SceneManager
//* 返値：void
//*----------------------------------------------------------------------------------------
void c_Game_LoadProcess::OnExit(SceneManager* pOwner)
{
    /* 兵士 */
    {
        // マテリアル取得
        auto matPtr1 = Master::m_pResourceManager->FindMaterial("Soldier_body");
        auto matPtr2 = Master::m_pResourceManager->FindMaterial("Soldier_head");

        SetupMaterialInfo matInfo[3];
        matInfo[0].Index = 0;
        matInfo[0].pMaterialData = matPtr1; // 体

        matInfo[1].Index = 1;
        matInfo[1].pMaterialData = matPtr1; // ヘルメット（体）

        matInfo[2].Index = 2;
        matInfo[2].pMaterialData = matPtr2; // 頭

        CreateModelInfo model;
        model.pRenderer = m_pRenderer;
        model.Path = "Resource/Model/Player/chara.fbx";
        model.ObjTag = "Soldier";
        model.IsAnim = true;
        model.MatNum = 3;
        model.SetupMaterial = matInfo;
        model.ShaderType = SHADER_TYPE::DEFERRED_STD_SKINNED_N;
        auto obj = MeshFactory::CreateModel(model);
        obj->get_Component<SkinnedMeshAnimator>()->set_AnimIndex(-1);
    }

    std::shared_ptr<GameObject> sphireObj;

    /* スフィア */
    {
        // マテリアル取得
        auto matPtr = Master::m_pResourceManager->FindMaterial("Canonn");

        SetupMaterialInfo matInfo[1];
        matInfo[0].Index = 0;
        matInfo[0].pMaterialData = matPtr;

        CreateUtilityMeshInfo mesh;
        mesh.pRenderer = m_pRenderer;
        mesh.Type = UTILITY_MESH_TYPE::SPHERE;
        mesh.MatNum = 1;
        mesh.MaterialData = matInfo;
        mesh.IsActive = true;
        mesh.ShaderType = SHADER_TYPE::DEFERRED_STD_STATIC_N;
        mesh.IsNormalMap = false;
        mesh.ObjTag = "Canonn";

        sphireObj = MeshFactory::CreateUtilityMesh(mesh);
        sphireObj->get_Transform().lock()->set_Scale(10.0f, 10.0f, 10.0f);
        sphireObj->get_Transform().lock()->set_Pos(0.0f, 1000.0f, 0.0f);
        sphireObj->get_Transform().lock()->set_RotateToDeg(0.0f, 0.0f, 0.0f);

        auto light = sphireObj->add_Component<PointLight>();
    }

    /* アリ モデルの生成 */
    {
        // マテリアル取得
        auto matPtr = Master::m_pResourceManager->FindMaterial("Ant");

        SetupMaterialInfo matInfo[1];
        matInfo[0].Index = 0;
        matInfo[0].pMaterialData = matPtr;

        CreateModelInfo model;
        model.pRenderer = m_pRenderer;
        model.Path = "Resource/Model/Enemy/ant_lowpoly.fbx";
        model.ObjTag = "Ant1";
        model.IsAnim = true;
        model.MatNum = 1;
        model.SetupMaterial = matInfo;
        model.ShaderType = SHADER_TYPE::DEFERRED_STD_SKINNED_N;

        for (int i = 0; i < 100; i++)
        {
            model.ObjTag = "Ant_" + std::to_string(i + 1);   // タグ

            auto obj  = MeshFactory::CreateModel(model);
            auto transform = obj->get_Component<Transform>();

            obj->get_Component<SkinnedMeshAnimator>()->set_IsAnim(true);
            obj->get_Component<SkinnedMeshAnimator>()->set_AnimIndex(0);
            
            // エネミーコントローラーと体力管理を追加
            obj->add_Component<EnemyController>();
            obj->add_Component<Health>();

            VEC3 pos = VEC3();
            pos.x = Tool::RandRange(-1000.0, 1000.0);
            pos.y = 15.0f;
            pos.z = Tool::RandRange(-1000.0, 1000.0);
            
            VEC3 rot = VEC3();
            rot.y = Tool::RandRange(-180.0f, 180.0f);

            transform->set_Pos(pos);
            transform->set_RotateToDeg(rot);
            transform->set_Scale(0.1f, 0.1f, 0.1f);


            // コライダーの追加
            auto collider = obj->add_Component<BoxCollider>();
            collider->set_Size(VEC3(20, 15, 20));
            collider->set_Center(VEC3(0, 10, 0));

            // コライダーの登録
            Master::m_pCollisionManager->RegisterCollider(collider);
        }
    }

    /* B-2 モデルの生成 */
    {
        // マテリアル取得
        auto matPtr = Master::m_pResourceManager->FindMaterial("B_2");

        SetupMaterialInfo matInfo[1];
        matInfo[0].Index = 0;
        matInfo[0].pMaterialData = matPtr;

        CreateModelInfo model;
        model.pRenderer = m_pRenderer;
        model.Path = "Resource/Model/b-2/B-2_NonBone.fbx";
        model.ObjTag = "B-2";
        model.IsAnim = false;
        model.MatNum = 1;
        model.SetupMaterial = matInfo;
        model.ShaderType = SHADER_TYPE::DEFERRED_STD_STATIC_N;
        for (int i = 0; i < 3; i++)
        {
            model.ObjTag = "B-2_" + std::to_string(i + 1);
            auto obj = MeshFactory::CreateModel(model);
            obj->get_Component<Transform>()->set_Scale(0.05f, 0.05f, 0.05f);
        }
    }

    /* クレイモア モデルの生成 */
    {
        // マテリアル取得
        auto matPtr = Master::m_pResourceManager->FindMaterial("Claymore");

        SetupMaterialInfo matInfo[1];
        matInfo[0].Index = 0;
        matInfo[0].pMaterialData = matPtr;

        CreateModelInfo model;
        model.pRenderer = m_pRenderer;
        model.Path = "Resource/Model/Claymore/Claymore.fbx";
        model.ObjTag = "Claymore";
        model.IsAnim = false;
        model.MatNum = 1;
        model.SetupMaterial = matInfo;
        model.ShaderType = SHADER_TYPE::DEFERRED_STD_STATIC_N;
        auto obj = MeshFactory::CreateModel(model);
        obj->get_Component<Transform>()->set_Scale(2.0f, 2.0f, 2.0f);
        obj->get_Component<Transform>()->set_Pos(0.0f, 100.0f, 400.0f);
    }

    /* 建物 モデルの生成 */
    {
        // マテリアル取得
        auto matPtr = Master::m_pResourceManager->FindMaterial("Building");

        SetupMaterialInfo matInfo[1];
        matInfo[0].Index = 0;
        matInfo[0].pMaterialData = matPtr;

        CreateModelInfo model;
        model.pRenderer = m_pRenderer;
        model.Path = "Resource/Model/Building/01/building01.fbx";
        model.ObjTag = "Building";
        model.IsAnim = false;
        model.MatNum = 1;
        model.SetupMaterial = matInfo;
        model.ShaderType = SHADER_TYPE::DEFERRED_STD_STATIC;
        auto obj = MeshFactory::CreateModel(model);
        obj->get_Component<Transform>()->set_Scale(0.5f, 0.5f, 0.5f);
        obj->get_Component<Transform>()->set_Pos(300.0f, 0.0f, 0.0f);
        obj->get_Component<Transform>()->set_RotateToDeg(0.0f, 0.0f, 0.0f);

        // コライダーの追加
        auto collider = obj->add_Component<BoxCollider>();
        collider->set_Size(VEC3(170.0f, 300.0f, 170.0f));
        collider->set_Center(VEC3(0.0f, 300.0f, 0.0f));
        collider->set_IsStatic(true);

        // コライダーの登録
        Master::m_pCollisionManager->RegisterCollider(obj->get_Component<BoxCollider>());
    }

    /* マザーシップの生成 */
    {
        // マテリアル取得
        auto matPtr1 = Master::m_pResourceManager->FindMaterial("MotherShip");

        SetupMaterialInfo matInfo[1];
        matInfo[0].Index = 0;
        matInfo[0].pMaterialData = matPtr1;

        CreateModelInfo model;
        model.pRenderer = m_pRenderer;
        model.Path = "Resource/Model/Enemy/MotherShip.fbx";
        model.ObjTag = "MotherShip";
        model.IsAnim = false;
        model.MatNum = 1;
        model.IsActive = true;

        model.SetupMaterial = matInfo;

        model.ShaderType = SHADER_TYPE::DEFERRED_STD_STATIC;
        auto obj = MeshFactory::CreateModel(model);
        obj->get_Component<Transform>()->set_Scale(0.5f, 0.5f, 0.5f);
        obj->get_Component<Transform>()->set_Pos(0.0f, 2000.0f, 0.0f);
        obj->get_Component<Transform>()->set_RotateToDeg(0.0f, 0.0f, 0.0f);
    }

    /* クモ モデルの生成 */
    {
        // マテリアル取得
        auto matPtr1 = Master::m_pResourceManager->FindMaterial("Spider_1");
        auto matPtr2 = Master::m_pResourceManager->FindMaterial("Spider_2");
        auto matPtr3 = Master::m_pResourceManager->FindMaterial("Spider_3");
        auto matPtr4 = Master::m_pResourceManager->FindMaterial("Spider_4");

        SetupMaterialInfo matInfo[4];
        matInfo[0].Index = 0;
        matInfo[0].pMaterialData = matPtr1;
        matInfo[1].Index = 1;
        matInfo[1].pMaterialData = matPtr2;
        matInfo[2].Index = 2;
        matInfo[2].pMaterialData = matPtr3;
        matInfo[3].Index = 3;
        matInfo[3].pMaterialData = matPtr4;

        CreateModelInfo model;
        model.pRenderer = m_pRenderer;
        model.Path = "Resource/Model/fbx/Spider_3.fbx";
        model.ObjTag = "Spider";
        model.IsAnim = true;
        model.MatNum = 4;
        model.IsActive = false;

        model.SetupMaterial = matInfo;

        model.ShaderType = SHADER_TYPE::DEFERRED_STD_SKINNED_N;
        auto obj = MeshFactory::CreateModel(model);
        obj->get_Component<SkinnedMeshAnimator>()->set_IsAnim(true);
        obj->get_Component<Transform>()->set_Scale(0.5f, 0.5f, 0.5f);
        obj->get_Component<Transform>()->set_Pos(0.0f, 0.0f, 0.0f);
        obj->get_Component<Transform>()->set_RotateToDeg(0.0f, 0.0f, 0.0f);
    }

    /* 地面の生成 */
    {
        // マテリアル取得
        auto matPtr = Master::m_pResourceManager->FindMaterial("Ground");

        SetupMaterialInfo matInfo[1];
        matInfo[0].Index = 0;
        matInfo[0].pMaterialData = matPtr;

        CreateUtilityMeshInfo mesh;
        mesh.pRenderer = m_pRenderer;
        mesh.Type = UTILITY_MESH_TYPE::PLANE;
        mesh.ObjTag = "Ground";
        mesh.MatNum = 1;
        mesh.MaterialData = matInfo;
        mesh.ShaderType = SHADER_TYPE::DEFERRED_STD_STATIC_N;
        mesh.IsNormalMap = true;

        auto obj = MeshFactory::CreateUtilityMesh(mesh);
        obj->get_Transform().lock()->set_Scale(1000.0f, 500.0f, 1000.0f);
        obj->get_Transform().lock()->set_Pos(0.0f, 0.0f, 0.0f);
        obj->get_Transform().lock()->set_RotateToDeg(0.0f, 0.0f, 0.0f);

        // コライダーの追加
        auto collider = obj->add_Component<BoxCollider>();
        collider->set_Size(VEC3(1000, 1, 1000));
        collider->set_Center(VEC3(0, 0, 0));
        collider->set_IsStatic(true);

        // コライダーの登録
        Master::m_pCollisionManager->RegisterCollider(collider);
    }

    /* スカイボックスの生成 */
    {
        // マテリアル取得
        auto matPtr = Master::m_pResourceManager->FindMaterial("SkyBox");

        SetupMaterialInfo matInfo[1];
        matInfo[0].Index = 0;
        matInfo[0].pMaterialData = matPtr;

        CreateSkyboxInfo skyInfo;
        skyInfo.pRenderer = m_pRenderer;
        skyInfo.ObjTag = "Skybox";
        skyInfo.MatNum = 1;
        skyInfo.MaterialData = matInfo;
        skyInfo.ShaderType = SHADER_TYPE::POST_SKYBOX;
        skyInfo.IsActive = false;

        auto obj = MeshFactory::CreateSkybox(skyInfo);
        obj->get_Transform().lock()->set_Scale(1.0f, 1.0f, 1.0f);
        obj->get_Transform().lock()->set_Pos(0.0f, 0.0f, 0.0f);
    }

    /* ビルボードの生成 */
    {
        // マテリアル取得
        auto matPtr = Master::m_pResourceManager->FindMaterial("Billboard");

        SetupMaterialInfo matInfo[1];
        matInfo[0].Index = 0;
        matInfo[0].pMaterialData = matPtr;


        CreateBillboradInfo billboard;
        billboard.pRenderer = m_pRenderer;
        billboard.Type = BILLBOARD_USAGE_TYPE::SIMPLE;
        billboard.ShaderType = SHADER_TYPE::FORWARD_UNLIT_STATIC;
        billboard.IsActive = true;
        billboard.MatNum = 1;
        billboard.MaterialData = matInfo;
        billboard.IsTransparent = true; // 透明度があり

        for (int i = 0; i < 10; i++)
        {
            VEC3 pos;
            pos.x = static_cast<float>(rand() % 2000) - 1000.0f;
            pos.y = static_cast<float>(rand() % 200) - 100.0f;;
            pos.z = static_cast<float>(rand() % 2000) - 1000.0f;

            VEC3 scl;
            scl.x = static_cast<float>(rand() % 50) - 25.0f;
            scl.y = static_cast<float>(rand() % 50) - 25.0f;;
            scl.z = static_cast<float>(rand() % 50) - 25.0f;

            VEC3 col;
            col.x = static_cast<float>(rand() % 255) / 255.0f;
            col.y = static_cast<float>(rand() % 255) / 255.0f;
            col.z = static_cast<float>(rand() % 255) / 255.0f;

            //mat->m_DiffuseColor = VEC4(0.5, 0.5, 0.5, 1.0f);
            auto obj = MeshFactory::CreateBillboard(billboard);
            obj->get_Transform().lock()->set_Pos(pos);
            obj->get_Transform().lock()->set_Scale(50, 50, 50);
            obj->set_Tag("Billboard" + std::to_string(i));
        }
    }

    /* ポイントライトの生成 (Cubuで分かりやすく)*/
    {
        // マテリアル取得
        auto matPtr = Master::m_pResourceManager->FindMaterial("PointLight");

        SetupMaterialInfo matInfo[1];
        matInfo[0].Index = 0;
        matInfo[0].pMaterialData = matPtr;

        CreateUtilityMeshInfo mesh;
        mesh.pRenderer = m_pRenderer;
        mesh.Type = UTILITY_MESH_TYPE::CUBU;
        mesh.MatNum = 1;
        mesh.MaterialData = matInfo;
        mesh.IsActive = true;
        mesh.ShaderType = SHADER_TYPE::DEFERRED_STD_STATIC_N;
        mesh.IsNormalMap = true;

        for (int i = 0; i < 30; i++)
        {
            VEC3 pt;
            pt.x = static_cast<float>(rand() % 1000) - 500.0f;
            pt.y = 50.0f;
            pt.z = static_cast<float>(rand() % 1000) - 500.0f;
            VEC3 col;
            col.x = static_cast<float>(rand() % 255) / 255.0f;
            col.y = static_cast<float>(rand() % 255) / 255.0f;
            col.z = static_cast<float>(rand() % 255) / 255.0f;

            auto obj = MeshFactory::CreateUtilityMesh(mesh);
            obj->get_Transform().lock()->set_Pos(pt);
            obj->get_Transform().lock()->set_Scale(VEC3(10, 10, 10));
            obj->set_Tag("PointLight" + std::to_string(i));
            auto light = obj->add_Component<PointLight>();
            light->set_LightColor(col);
            light->set_Range(100.0f);
            light->set_Intensity(25.0f);
        }
    }


    // カメラ操作をオンに
    m_pRenderer->get_CameraComponent()->set_IsControl(true);

    // プレイヤーに操作コンポーネントつける
    auto player = Master::m_pGameObjectManager->get_ObjectByTag("Player");
    auto comp = player->add_Component<PlayerController>(1);
    comp->Start(*m_pRenderer);


    // ロード画面用スプライトをオフに
    auto obj = Master::m_pGameObjectManager->get_ObjectByTag("LoadScreen_Sp");
    if (obj)
    {
        obj->clear_StatusFlag(OBJECT_STATUS_BITFLAG::IS_ACTIVE);
    }
}


//*---------------------------------------------------------------------------------------
//* @:c_Game_LoadProcess Class 
//*【?】更新
//* 引数：1.SceneManager
//* 返値：void
//*----------------------------------------------------------------------------------------
int c_Game_LoadProcess::Update(SceneManager *pOwner)
{
	if (m_IsLoad)
	{
		return c_GAME::c_GAME_PLAY;
	}

	return c_GAME::c_GAME_LOAD;
}


//*---------------------------------------------------------------------------------------
//* @:c_Game_LoadProcess Class 
//*【?】描画
//* 引数：1.SceneManager
//* 返値：void
//*----------------------------------------------------------------------------------------
void c_Game_LoadProcess::Draw(SceneManager *pOwner)
{
	// すでにロードされているなら返す
	if (m_IsLoad)
	{
		return;
	}

	Master::m_pDirectWriteManager->DrawString("☆ロード中", VECTOR2::VEC2(940, 500));

	// ロード完了
	m_IsLoad = true;
}

#include "pch.h"
#include "c_Title_LoadProcess.h"
#include "RendererEngine.h"
#include "ResourceManager.h"
#include "SceneStateEnums.h"
#include "Component_SpriteRenderer.h"
#include "Component_SkinnedMeshAnimator.h"
#include "Component_PlayerController.h"
#include "Component_BoxCollider.h"
#include "Component_3DCamera.h"
#include "Component_Transform.h"
#include "Component_TrailRenderer.h"
#include "Component_DirectionalLight.h"
#include "Component_AssultRifle.h"
#include "Component_Health.h"
#include "GameObject.h"
#include "MeshFactory.h"
#include "InputFactory.h"

using namespace SceneStateEnums;
using namespace VECTOR2;
using namespace VECTOR3;
using namespace VECTOR4;
using namespace GIGA_Engine;;


//*---------------------------------------------------------------------------------------
//* @:c_Title_LoadProcess Class 
//*【?】開始
//* 引数：1.SceneManager
//* 返値：void
//*----------------------------------------------------------------------------------------
void c_Title_LoadProcess::OnEnter(SceneManager *pOwner)
{
	// すでにロードされているなら返す
	if (m_IsLoad) {
		m_pLoadBackSprite->get_OwnerObj().lock()->set_StatusFlag(OBJECT_STATUS_BITFLAG::IS_ACTIVE);
		return;
	}

    /* カメラの作成 */
    {
        auto obj = Instantiate(std::move(std::make_shared<GameObject>()), false);
        if (obj == nullptr)
        {
            assert(false);
        }
        obj->Init(*m_pRenderer);
        obj->set_Tag("Camera");
        obj->set_StatusFlag(OBJECT_STATUS_BITFLAG::IS_ACTIVE);
        obj->get_Transform().lock()->set_Pos(0.0f, 0.0f, 1000.0f);
        m_pCameraComp = obj->add_Component<Camera3D>(); // カメラコンポーネントの追加
        m_pCameraComp->set_IsControl(false);    // 操作フラグをオフに

        // 描画エンジン側にカメラコンポーネントを渡す
        m_pRenderer->set_CameraComponent(m_pCameraComp);
    }

    // ライトにカメラのTransformを持たせる
    Master::m_pLightManager->set_CameraTransform(m_pCameraComp->get_OwnerObj().lock()->get_Transform());


    // ロード画面用用スプライト**********************************************
	CreateSpriteInfo sprite;
	sprite.pTextureMap[0] = Master::m_pResourceManager->LoadWIC_Texture(L"Resource/Texture/Title/Load.png");
	sprite.IsActive = true;
	sprite.ObjTag = "LoadScreen_Sp";
	sprite.pRenderer = m_pRenderer;
	sprite.ShaderType = SHADER_TYPE::FORWARD_UNLIT_UI_SPRITE;
	sprite.Type = SPRITE_USAGE_TYPE::NORMAL;
	sprite.Width = m_pRenderer->get_ScreenWidth();
	sprite.Height = m_pRenderer->get_ScreenHeight();
	sprite.IsActive = true;
	sprite.IsTransparent = true;
	auto obj = MeshFactory::CreateSprite(sprite);
	if (obj) {
		obj->get_Transform().lock()->set_Pos(0.0f, 0.0f, 0.0f);
        m_pLoadBackSprite = obj->get_Component<SpriteRenderer>();
    }

}


//*---------------------------------------------------------------------------------------
//* @:c_Title_LoadProcess Class 
//*【?】終了
//* 引数：1.SceneManager
//* 返値：void
//*----------------------------------------------------------------------------------------
void c_Title_LoadProcess::OnExit(SceneManager *pOwner)
{

    // CSVからマテリアルデータの読み込み
    if (!Master::m_pResourceManager->ImportCSV_AllMaterialData("Resource/Excel_Param/MaterialParam.csv"))
    {
        assert(false);
    }

    // マテリアルの作成 (今後CSVで読み込むようにする)
    {
        /* クモ */
        {
            Material mat[4];
            mat[0].m_DiffuseMap.Texture = Master::m_pResourceManager->LoadWIC_Texture(L"Resource/Model/fbx/textures/Spinnen_Bein_tex.jpg");
            mat[0].m_NormalMap.Texture = Master::m_pResourceManager->LoadWIC_Texture(L"Resource/Model/fbx/textures/haar_detail_NRM.jpg");
            mat[0].m_DiffuseColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);
            mat[0].m_SpecularColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);
            mat[0].m_SpecularPower = 150.0f;

            // マテリアル登録
            Master::m_pResourceManager->RegisterMaterialData("Spider_1", mat[0]);

            mat[1].m_DiffuseMap.Texture = Master::m_pResourceManager->LoadWIC_Texture(L"Resource/Model/fbx/textures/Spinnen_Bein_tex.jpg");
            mat[1].m_DiffuseColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);
            mat[1].m_SpecularColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);
            mat[1].m_SpecularPower = 150.0f;

            // マテリアル登録
            Master::m_pResourceManager->RegisterMaterialData("Spider_2", mat[1]);

            mat[2].m_DiffuseMap.Texture = Master::m_pResourceManager->LoadWIC_Texture(L"Resource/Model/fbx/textures/Spinnen_Bein_tex.jpg");
            mat[2].m_DiffuseColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);
            mat[2].m_SpecularColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);
            mat[2].m_SpecularPower = 150.0f;

            // マテリアル登録
            Master::m_pResourceManager->RegisterMaterialData("Spider_3", mat[2]);

            mat[3].m_DiffuseMap.Texture = Master::m_pResourceManager->LoadWIC_Texture(L"Resource/Model/fbx/textures/SH3.png");
            mat[3].m_NormalMap.Texture = Master::m_pResourceManager->LoadWIC_Texture(L"Resource/Model/fbx/textures/haar_detail_NRM.jpg");
            mat[3].m_DiffuseColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);
            mat[3].m_SpecularColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);
            mat[3].m_SpecularPower = 100.0f;

            // マテリアル登録
            Master::m_pResourceManager->RegisterMaterialData("Spider_4", mat[3]);
        }

        /* スカイボックス */
        {
            Material mat;
            mat.m_DiffuseMap.Texture = Master::m_pResourceManager->LoadDDS_CubeMap_Texture(L"Resource/Texture/cloudy_skybox.dds");

            // マテリアル登録
            Master::m_pResourceManager->RegisterMaterialData("SkyBox", mat);
        }
    }

    std::shared_ptr<GameObject> pPlayerObj;
    /* プレイヤー モデルの生成 */
    {
        // マテリアル取得
        auto matPtr = Master::m_pResourceManager->FindMaterial("PlayerModel");

        SetupMaterialInfo matInfo[1];
        matInfo[0].Index = 0;
        matInfo[0].pMaterialData = matPtr;

        CreateModelInfo model;
        model.pRenderer = m_pRenderer;
        model.Path = "Resource/Model/Player2/AL_Standard.fbx";
        model.ObjTag = "Player";
        model.IsAnim = true;
        model.MatNum = 1;
        model.SetupMaterial = matInfo;
        model.ShaderType = SHADER_TYPE::DEFERRED_STD_SKINNED_N;
        model.Shadow_ShaderType = SHADER_TYPE::POST_SHADOWMAP;
        pPlayerObj = MeshFactory::CreateModel(model);
        pPlayerObj->get_Component<Transform>()->set_Scale(0.1f, 0.1f, 0.1f);
        pPlayerObj->get_Component<SkinnedMeshAnimator>()->set_IsAnim(true);
        pPlayerObj->get_Component<SkinnedMeshAnimator>()->set_AnimIndex(0);

        //pPlayerObj->add_Component<PlayerController>(1);
        pPlayerObj->get_Transform().lock()->set_Pos(0.0f, 0.0f, -1000.0f);
        //pPlayerObj->get_Transform().lock()->set_RotateToDeg(0.0f, 0.0f, 0.0f);

        auto trail = pPlayerObj->add_Component<TrailRenderer>();
        trail->set_MinVertexDistance(4.0f);
        trail->set_Width(5.0f);

        // コライダーの追加
        auto collider = pPlayerObj->add_Component<BoxCollider>();
        collider->set_Size(VEC3(5, 10, 5));
        collider->set_Center(VEC3(0, 10, 0));

        auto health = pPlayerObj->add_Component<Health>();

        // コライダーの登録
        Master::m_pCollisionManager->RegisterCollider(collider);

        // カメラのフォーカスオブジェクトに設定
        m_pCameraComp->set_FocusObject(pPlayerObj);
    }

    /* アサルトライフル */
    {
        // マテリアル取得
        auto matPtr1 = Master::m_pResourceManager->FindMaterial("AssultRifle");

        SetupMaterialInfo matInfo[1];
        matInfo[0].Index = 0;
        matInfo[0].pMaterialData = matPtr1; // 体

        CreateModelInfo model;
        model.pRenderer = m_pRenderer;
        model.Path = "Resource/Model/Weapon/M4A1.fbx";
        model.ObjTag = "AssultRifle";
        model.IsAnim = false;
        model.MatNum = 1;
        model.SetupMaterial = matInfo;
        model.ShaderType = SHADER_TYPE::DEFERRED_STD_STATIC;
        auto obj = MeshFactory::CreateModel(model);

        obj->add_Component<AssultRifle>();

        // プレイヤーを親に設定
        obj->get_Transform().lock()->set_Parent(pPlayerObj->get_Transform());
        obj->get_Transform().lock()->set_VEC3ToLocalOffset_Scale(VEC3(-0.985f, -0.985f, -0.985f));
        obj->get_Transform().lock()->set_VEC3ToLocalOffset_RotateToDeg(VEC3(0.0f, 0.0f, 90.0f));
        obj->get_Transform().lock()->set_VEC3ToLocalOffset_Pos(VEC3(0.0f, 150.0f, 0.0f));
    }

    /* ディレクションライトの生成(Cubuで分かりやすく) */
    {
        // マテリアル取得
        auto matPtr = Master::m_pResourceManager->FindMaterial("DirLight");

        SetupMaterialInfo matInfo[1];
        matInfo[0].Index = 0;
        matInfo[0].pMaterialData = matPtr;

        CreateUtilityMeshInfo mesh;
        mesh.pRenderer = m_pRenderer;
        mesh.Type = UTILITY_MESH_TYPE::CUBU;
        mesh.ObjTag = "DirLight";
        mesh.MatNum = 1;
        mesh.MaterialData = matInfo;

        auto obj = MeshFactory::CreateUtilityMesh(mesh);
        //auto obj = Instantiate(std::move(std::make_shared<GameObject>()));
        obj->set_Tag("DirLight");
        obj->set_StatusFlag(OBJECT_STATUS_BITFLAG::IS_ACTIVE);
        auto light = obj->add_Component<DirectionalLight>();
        light->set_LightColor(VEC3(1.0f, 1.0f, 1.0f));
        light->set_Intensity(2.0f);
        light->set_LightCameraTrackingObj(m_pCameraComp->get_OwnerObj().lock());
        light->Start(*m_pRenderer);

        obj->get_Transform().lock()->set_Pos(VEC3(0.0f, 1000.0f, -1000.0f));
        obj->get_Transform().lock()->set_Scale(VEC3(30.0, 30.0, 80.0));
        obj->get_Transform().lock()->set_RotateToRad(VEC3(0.85f, 1.6f, 0.0f));
    }

    /* タイトル画面背景用スプライト */
    {
        CreateSpriteInfo sprite;
        sprite.pTextureMap[0] = Master::m_pResourceManager->LoadWIC_Texture(L"Resource/Texture/Title/TitleBack.png");
        sprite.ObjTag = "TitleBack_Sp";
        sprite.pRenderer = m_pRenderer;
        sprite.ShaderType = SHADER_TYPE::FORWARD_UNLIT_UI_SPRITE;
        sprite.Type = SPRITE_USAGE_TYPE::NORMAL;
        sprite.Width = m_pRenderer->get_ScreenWidth();
        sprite.Height = m_pRenderer->get_ScreenHeight();
        sprite.IsActive = true;
        sprite.IsTransparent = true;
        auto obj = MeshFactory::CreateSprite(sprite);
        if (obj) {
            obj->get_Transform().lock()->set_Pos(0.0f, 0.0f, 0.0f);
        }
    }
    
    /* タイトルメニュー項目背景用スプライト */
    {
        CreateSpriteInfo sprite;
        sprite.pTextureMap[0] = Master::m_pResourceManager->LoadWIC_Texture(L"Resource/Texture/Title/Line.png");
        sprite.pRenderer = m_pRenderer;
        sprite.ShaderType = SHADER_TYPE::FORWARD_UNLIT_UI_SPRITE;
        sprite.Type = SPRITE_USAGE_TYPE::NORMAL;
        sprite.Width = 500.0f;
        sprite.Height = 200.0f;
        sprite.IsActive = true;
        sprite.IsTransparent = true;


        // 項目分生成
        for (int i = 0; i < 4; i++)
        {
            sprite.ObjTag = "MenuItemBack_Sp" + std::to_string(i + 1);
            auto obj = MeshFactory::CreateSprite(sprite);
        }
    }

    /* タイトル項目説明用スプライト */
    {
        CreateSpriteInfo sprite;
        sprite.pTextureMap[0] = Master::m_pResourceManager->LoadWIC_Texture(L"Resource/Texture/Title/Title_Description_01.png");
        sprite.pRenderer = m_pRenderer;
        sprite.ShaderType = SHADER_TYPE::FORWARD_UNLIT_UI_SPRITE;
        sprite.Type = SPRITE_USAGE_TYPE::NORMAL;
        sprite.Width = 427.0f;
        sprite.Height = 240.0f;
        sprite.IsActive = false;
        sprite.IsTransparent = true;
        // 項目分生成
        for (int i = 0; i < 4; i++)
        {
            sprite.ObjTag = "TitleDescription01_Sp" + std::to_string(i + 1);
            //auto obj = MeshFactory::CreateSprite(sprite);
        }
    }

    /* タイトルロゴ用スプライト */
    {
        CreateSpriteInfo sprite;
        sprite.pTextureMap[0] = Master::m_pResourceManager->LoadWIC_Texture(L"Resource/Texture/Title/Title_logo.png");
        sprite.ObjTag = "TitleLogo_Sp";
        sprite.pRenderer = m_pRenderer;
        sprite.ShaderType = SHADER_TYPE::FORWARD_UNLIT_UI_SPRITE;
        sprite.Type = SPRITE_USAGE_TYPE::NORMAL;
        sprite.Width = m_pRenderer->get_ScreenWidth();
        sprite.Height = m_pRenderer->get_ScreenHeight();
        sprite.IsActive = true;
        sprite.IsTransparent = true;
        auto obj = MeshFactory::CreateSprite(sprite);
        if (obj) {
            obj->get_Transform().lock()->set_Pos(0.0f, 0.0f, 0.0f);
        }
    }


    // ロード画面用スプライトオフ
	m_pLoadBackSprite->get_OwnerObj().lock()->clear_StatusFlag(OBJECT_STATUS_BITFLAG::IS_ACTIVE);
}


//*---------------------------------------------------------------------------------------
//* @:c_Title_LoadProcess Class 
//*【?】更新
//* 引数：1.SceneManager
//* 返値：void
//*----------------------------------------------------------------------------------------
int c_Title_LoadProcess::Update(SceneManager *pOwner)
{
    // ロード済みならメインメニューへ
    if (m_IsLoad)
    {
        return c_TITLE::c_TITLE_MAIN_MENU;
    }


	return c_TITLE::c_TITLE_LOAD_PROCESS;
}


//*---------------------------------------------------------------------------------------
//* @:c_Title_LoadProcess Class 
//*【?】描画
//* 引数：1.SceneManager
//* 返値：void
//*----------------------------------------------------------------------------------------
void c_Title_LoadProcess::Draw(SceneManager *pOwner)
{
    // すでにロードされているなら返す
    if (m_IsLoad)
    {
        return;
    }

	Master::m_pDirectWriteManager->DrawString("☆ロード中", VECTOR2::VEC2(40.0f, 500.0f));

    // ロード完了
    m_IsLoad = true;
}

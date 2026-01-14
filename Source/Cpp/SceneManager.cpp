#include "pch.h"
#include "SceneManager.h"
#include "RendererEngine.h"
#include "DirectWriteManager.h"
#include "ResourceManager.h"
#include "GameObjectManager.h"
#include "InputFactory.h"
#include "Cubu.h"
#include "Quad.h"
#include "SceneFactory.h"
#include "Camera.h"
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


using namespace VECTOR4;
using namespace VECTOR3;
using namespace VECTOR2;
using namespace Tool::UV;
using namespace Tool;
using namespace Input;

using namespace GIGA_Engine;



//*---------------------------------------------------------------------------------------
//* @:SceneManager Class 
//*【?】コンストラクタ
//* 引数：なし
//*----------------------------------------------------------------------------------------
SceneManager::SceneManager():
    m_pPlayer(),
    m_StateMachine(this),
    m_CrntSceneState(0)
{

}


//*---------------------------------------------------------------------------------------
//* @:SceneManager Class 
//*【?】デストラクタ
//* 引数：なし
//*----------------------------------------------------------------------------------------
SceneManager::~SceneManager()
{
}


//*---------------------------------------------------------------------------------------
//* @:SceneManager Class 
//*【?】初期化
//* 引数：1.RendererEngine
//* 返値：成功したか
//*----------------------------------------------------------------------------------------
bool SceneManager::Init(RendererEngine &renderer)
{
    // ステートマシンの作成
    SceneFactory::Create(m_StateMachine, renderer);

    std::vector<std::weak_ptr<GameObject>> objectList;

    // オブジェクトの生成
    {
        /* カメラの作成 */
        {
            m_pCamera = Instantiate(std::move(std::make_shared<GameObject>()));
            if (m_pCamera == nullptr) return false;
            m_pCamera->Init(renderer);
            m_pCamera->set_Tag("Camera");
            m_pCamera->clear_StatusFlag(OBJECT_STATUS_BITFLAG::IS_ACTIVE);
            m_pCamera->add_Component<Camera3D>();
            m_pCamera->get_Transform().lock()->set_Pos(0.0f, 800.0f, -1000.0f);
            m_pCamera->set_LayerRank(100);
            //m_pCamera->set_StatusFlag(OBJECT_STATUS_BITFLAG::IS_ACTIVE);
        }

        /* プレイヤー モデルの生成 */
        {
            Material mat[1];
            mat[0].m_DiffuseMap.Texture = Master::m_pResourceManager->LoadWIC_Texture(L"Resource/Texture/外壁S050.jpg");
            mat[0].m_NormalMap.Texture = Master::m_pResourceManager->LoadWIC_Texture(L"Resource/Texture/DefaultN_Map.png");
            mat[0].m_DiffuseColor = VEC4(0.4f, 0.4f, 0.6f, 1.0f);
            mat[0].m_SpecularPower = 150.0f;
            mat[0].m_SpecularColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);

            CreateModelInfo model;
            model.pRenderer = &renderer;
            model.Path = "Resource/Model/Player2/AL_Standard.fbx";
            model.ObjTag = "Player";
            model.IsAnim = true;
            model.MatNum = 1;
            model.MaterialData = new InputMaterial();
            model.MaterialData->MatIndex = 0;
            model.MaterialData->pMat = mat;
            model.ShaderType = SHADER_TYPE::DEFERRED_STD_SKINNED_N;
            model.Shadow_ShaderType = SHADER_TYPE::POST_SHADOWMAP;
            m_pPlayer = MeshFactory::CreateModel(model);
            m_pPlayer->get_Component<Transform>()->set_Scale(0.1f, 0.1f, 0.1f);
            m_pPlayer->get_Component<SkinnedMeshAnimator>()->set_IsAnim(true);
            m_pPlayer->get_Component<SkinnedMeshAnimator>()->set_AnimIndex(0);

            m_pPlayer->add_Component<PlayerController>(1);
            m_pPlayer->get_Component<PlayerController>()->Init(renderer);
            m_pPlayer->get_Transform().lock()->set_Pos(0.0f, 0.0f, 0.0f);
            m_pPlayer->get_Transform().lock()->set_RotateToDeg(0.0f, 0.0f, 0.0f);

            // カメラのフォーカスオブジェクトに設定
            m_pCamera->get_Component<Camera3D>()->set_FocusObject(m_pPlayer);
        }

        /* ディレクションライトの生成(Cubuで分かりやすく) */
        {
            Material* mat = new Material;
            mat->m_DiffuseMap.Texture = Master::m_pResourceManager->LoadWIC_Texture(L"Resource/Texture/Light_Img.png");
            mat->m_DiffuseColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);
            mat->m_SpecularPower = 1.0f;

            CreateUtilityMeshInfo mesh;
            mesh.pRenderer = &renderer;
            mesh.Type = UTILITY_MESH_TYPE::CUBU;
            mesh.ObjTag = "DirLight";
            mesh.MatNum = 1;
            mesh.MaterialData = new InputMaterial();
            mesh.MaterialData->pMat = mat;

            auto obj = MeshFactory::CreateUtilityMesh(mesh);
            //auto obj = Instantiate(std::move(std::make_shared<GameObject>()));
            obj->set_Tag("DirLight");
            obj->set_StatusFlag(OBJECT_STATUS_BITFLAG::IS_ACTIVE);
            auto light = obj->add_Component<DirectionalLight>();
            light->set_LightColor(VEC3(1.0f, 1.0f, 1.0f));
            light->set_Intensity(3.0f);
            light->set_Player(m_pPlayer);
            light->Init(renderer);

            obj->get_Transform().lock()->set_Pos(VEC3(0.0f, 1000.0f, -1000.0f));
            obj->get_Transform().lock()->set_Scale(VEC3(30.0, 30.0, 80.0));
            obj->get_Transform().lock()->set_RotateToRad(VEC3(0.85f, 1.6f, 0.0f));
        }

        /* 壁 */
        {
            Material* mat = new Material;
            mat->m_DiffuseMap.Texture = Master::m_pResourceManager->LoadWIC_Texture(L"Resource/Texture/外壁W040.jpg");
            mat->m_NormalMap.Texture = Master::m_pResourceManager->LoadWIC_Texture(L"Resource/Texture/外壁W040_n.png");
            mat->m_DiffuseColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);
            mat->m_SpecularColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);
            mat->m_SpecularPower = 20.0f;

            CreateUtilityMeshInfo mesh;
            mesh.pRenderer = &renderer;
            mesh.Type = UTILITY_MESH_TYPE::PLANE;
            mesh.MatNum = 1;
            mesh.MaterialData = new InputMaterial();
            mesh.MaterialData->pMat = mat;
            mesh.IsActive = true;
            mesh.ShaderType = SHADER_TYPE::DEFERRED_STD_STATIC_N;
            mesh.IsNormalMap = true;
            mesh.ObjTag = "Wall";

            auto obj = MeshFactory::CreateUtilityMesh(mesh);
            obj->get_Transform().lock()->set_Scale(100.0f, 100.0f, 100.0f);
            obj->get_Transform().lock()->set_Pos(-300.0f, 80.0f, 600.0f);
            obj->get_Transform().lock()->set_RotateToDeg(-60.0f, 0.0f, 0.0f);
        }

        /* アリ モデルの生成 */
        {
            Material mat[1];
            mat[0].m_DiffuseMap.Texture = Master::m_pResourceManager->LoadWIC_Texture(L"Resource/Model/Enemy/trader_ant_lowpoly.fbm/new_bake_ant.png");
            mat[0].m_NormalMap.Texture = Master::m_pResourceManager->LoadWIC_Texture(L"Resource/Model/Enemy/trader_ant_lowpoly.fbm/new_bake_ant_n.png");
            mat[0].m_DiffuseColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);
            mat[0].m_SpecularPower = 100.0f;
            mat[0].m_SpecularColor = VEC4(0.5f, 0.5f, 0.5f, 1.0f);

            CreateModelInfo model;
            model.pRenderer = &renderer;
            model.Path = "Resource/Model/Enemy/ant_lowpoly.fbx";
            model.ObjTag = "Ant1";
            model.IsAnim = true;
            model.MatNum = 1;
            model.MaterialData = new InputMaterial();
            model.MaterialData->MatIndex = 0;
            model.MaterialData->pMat = mat;
            model.ShaderType = SHADER_TYPE::DEFERRED_STD_SKINNED_N;

            for (int i = 0; i < 2; i++)
            {
                model.ObjTag = "Ant_" + std::to_string(i + 1);   // タグ

                m_pAnt[i] = MeshFactory::CreateModel(model);
                m_pAnt[i]->get_Component<Transform>()->set_Scale(0.1f, 0.1f, 0.1f);
                m_pAnt[i]->get_Component<Transform>()->set_RotateToDeg(0.0f, 180, 0.0);
                m_pAnt[i]->get_Component<Transform>()->set_Pos(0.0f, 0.0f, 0.0);
                m_pAnt[i]->get_Component<SkinnedMeshAnimator>()->set_IsAnim(true);
                m_pAnt[i]->get_Component<SkinnedMeshAnimator>()->set_AnimIndex(0);
            }
        }

        /* B-2 モデルの生成 */
        {

            Material mat[1];
            mat[0].m_DiffuseMap.Texture = Master::m_pResourceManager->LoadWIC_Texture(L"Resource/Model/b-2/textures/ggg_diffuseOriginal.jpeg");
            mat[0].m_SpecularMap.Texture = Master::m_pResourceManager->LoadWIC_Texture(L"Resource/Model/b-2/textures/ggg_metallic.jpeg");
            mat[0].m_NormalMap.Texture = Master::m_pResourceManager->LoadWIC_Texture(L"Resource/Model/b-2/textures/ggg_normal.jpeg");
            mat[0].m_DiffuseColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);
            mat[0].m_SpecularColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);
            mat[0].m_SpecularPower = 100.0f;

            CreateModelInfo model;
            model.pRenderer = &renderer;
            model.Path = "Resource/Model/b-2/B-2_NonBone.fbx";
            model.ObjTag = "B-2";
            model.IsAnim = false;
            model.MatNum = 1;
            model.MaterialData = new InputMaterial();
            model.MaterialData->MatIndex = 0;
            model.MaterialData->pMat = mat;
            model.ShaderType = SHADER_TYPE::DEFERRED_STD_STATIC_N;
            for (int i = 0; i < 3; i++)
            {
                model.ObjTag = "B-2_" + std::to_string(i + 1);
                m_pBomber[i] = MeshFactory::CreateModel(model);
                m_pBomber[i]->get_Component<Transform>()->set_Scale(0.05f, 0.05f, 0.05f);
            }
        }

        /* クレイモア モデルの生成 */
        {

            Material mat[1];
            mat[0].m_DiffuseMap.Texture = Master::m_pResourceManager->LoadWIC_Texture(L"Resource/Model/Claymore/Mat_Base_Color.png");
            mat[0].m_SpecularMap.Texture = Master::m_pResourceManager->LoadWIC_Texture(L"Resource/Model/Claymore/Mat_Roughness.png");
            mat[0].m_NormalMap.Texture = Master::m_pResourceManager->LoadWIC_Texture(L"Resource/Model/Claymore/Mat_Normal_DirectX.png");
            mat[0].m_DiffuseColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);
            mat[0].m_SpecularColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);
            mat[0].m_SpecularPower = 200.0f;

            CreateModelInfo model;
            model.pRenderer = &renderer;
            model.Path = "Resource/Model/Claymore/Claymore.fbx";
            model.ObjTag = "Claymore";
            model.IsAnim = false;
            model.MatNum = 1;
            model.MaterialData = new InputMaterial();
            model.MaterialData->MatIndex = 0;
            model.MaterialData->pMat = mat;
            model.ShaderType = SHADER_TYPE::DEFERRED_STD_STATIC_N;
            auto obj = MeshFactory::CreateModel(model);
            obj->get_Component<Transform>()->set_Scale(2.0f, 2.0f, 2.0f);
            obj->get_Component<Transform>()->set_Pos(0.0f, 100.0f, 400.0f);
        }      
        
        /* 建物 モデルの生成 */
        {
            Material mat[1];
            mat[0].m_DiffuseMap.Texture = Master::m_pResourceManager->LoadWIC_Texture(L"Resource/Model/Building/01/texture/building5-lo.jpg");
            mat[0].m_NormalMap.Texture = Master::m_pResourceManager->LoadWIC_Texture(L"Resource/Texture/DefaultN_Map.png");
            mat[0].m_DiffuseColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);
            mat[0].m_SpecularColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);
            mat[0].m_SpecularPower = 150.0f;

            CreateModelInfo model;
            model.pRenderer = &renderer;
            model.Path = "Resource/Model/Building/01/building01.fbx";
            model.ObjTag = "Building";
            model.IsAnim = false;
            model.MatNum = 1;
            model.MaterialData = new InputMaterial();
            model.MaterialData->MatIndex = 0;
            model.MaterialData->pMat = mat;
            model.ShaderType = SHADER_TYPE::DEFERRED_STD_STATIC;
            m_pTempObj = MeshFactory::CreateModel(model);
            m_pTempObj->get_Component<Transform>()->set_Scale(0.5f, 0.5f, 0.5f);
            m_pTempObj->get_Component<Transform>()->set_Pos(300.0f, 0.0f, 0.0f);
            m_pTempObj->get_Component<Transform>()->set_RotateToDeg(0.0f, 0.0f, 0.0f);
        }

        /* 地面の生成 */
        {
            Material* mat = new Material;
            mat->m_DiffuseMap.Texture = Master::m_pResourceManager->LoadWIC_Texture(L"Resource/Texture/aerial_grass_rock_diff_4k.png");
            mat->m_NormalMap.Texture = Master::m_pResourceManager->LoadWIC_Texture(L"Resource/Texture/aerial_grass_rock_nor_dx_4k.png");
            mat->m_SpecularColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);
            mat->m_SpecularPower = 50.0f;

            CreateUtilityMeshInfo mesh;
            mesh.pRenderer = &renderer;
            mesh.Type = UTILITY_MESH_TYPE::PLANE;
            mesh.ObjTag = "Ground";
            mesh.MatNum = 1;
            mesh.MaterialData = new InputMaterial();
            mesh.MaterialData->pMat = mat;
            mesh.ShaderType = SHADER_TYPE::DEFERRED_STD_STATIC_N;
            mesh.IsNormalMap = true;

            auto obj = MeshFactory::CreateUtilityMesh(mesh);
            obj->get_Transform().lock()->set_Scale(1000.0f, 500.0f, 1000.0f);
            obj->get_Transform().lock()->set_Pos(0.0f, 0.0f, 0.0f);
            obj->get_Transform().lock()->set_RotateToDeg(0.0f, 0.0f, 0.0f);
        }

        /* スカイボックスの生成 */
        {

            Material* mat = new Material;
            mat->m_DiffuseMap.Texture = Master::m_pResourceManager->LoadDDS_CubeMap_Texture(L"Resource/Texture/cloudy_skybox.dds");
            CreateSkyboxInfo skyInfo;
            skyInfo.pRenderer = &renderer;
            skyInfo.ObjTag = "Skybox";
            skyInfo.MatNum = 1;
            skyInfo.MaterialData = new InputMaterial();
            skyInfo.MaterialData->pMat = mat;
            skyInfo.ShaderType = SHADER_TYPE::POST_SKYBOX;
            skyInfo.IsActive = false;

            auto obj = MeshFactory::CreateSkybox(skyInfo);
            obj->get_Transform().lock()->set_Scale(1.0f, 1.0f, 1.0f);
            obj->get_Transform().lock()->set_Pos(0.0f, 0.0f, 0.0f);
        }

        /* ビルボードの生成 */
        {
            Material* mat = new Material;
            mat->m_DiffuseMap.Texture = Master::m_pResourceManager->LoadWIC_Texture(L"Resource/Texture/Particle/Flame3_1.png");
            mat->m_SpecularColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);
            mat->m_SpecularPower = 100.0f;
            mat->m_BlendMode = BLEND_MODE::ALPHA;
            CreateBillboradInfo billboard;
            billboard.pRenderer = &renderer;
            billboard.Type = BILLBOARD_USAGE_TYPE::SIMPLE;
            billboard.ShaderType = SHADER_TYPE::FORWARD_UNLIT_STATIC;
            billboard.IsActive = false;
            billboard.MatNum = 1;
            billboard.MaterialData = new InputMaterial();

            for (int i = 0; i < 30; i++)
            {
                VEC3 pos;
                pos.x = static_cast<float>(rand() % 2000) - 1000.0f;
                pos.y = static_cast<float>(rand() % 100) - 50.0f;;
                pos.z = static_cast<float>(rand() % 2000) - 1000.0f;

                VEC3 scl;
                scl.x = static_cast<float>(rand() % 50) - 25.0f;
                scl.y = static_cast<float>(rand() % 50) - 25.0f;;
                scl.z = static_cast<float>(rand() % 50) - 25.0f;

                VEC3 col;
                col.x = static_cast<float>(rand() % 255) / 255.0f;
                col.y = static_cast<float>(rand() % 255) / 255.0f;
                col.z = static_cast<float>(rand() % 255) / 255.0f;

                mat->m_DiffuseColor = VEC4(0.5, 0.5, 0.5, 1.0f);
                billboard.MaterialData->pMat = mat;
                /*auto obj = MeshFactory::CreateBillboard(billboard);
                obj->get_Transform().lock()->set_Pos(pos);
                obj->get_Transform().lock()->set_Scale(50, 50, 50);
                obj->set_Tag("Billboard" + std::to_string(i));
            */
            }
        }

        /* ポイントライトの生成 (Cubuで分かりやすく)*/
        {
            Material *mat = new Material;
            mat->m_DiffuseMap.Texture = Master::m_pResourceManager->LoadWIC_Texture(L"Resource/Texture/外壁W040.jpg");
            mat->m_NormalMap.Texture = Master::m_pResourceManager->LoadWIC_Texture(L"Resource/Texture/外壁W040_n.png");
            mat->m_DiffuseColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);
            mat->m_SpecularColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);
            mat->m_SpecularPower = 20.0f;

            CreateUtilityMeshInfo mesh;
            mesh.pRenderer = &renderer;
            mesh.Type = UTILITY_MESH_TYPE::CUBU;
            mesh.MatNum = 1;
            mesh.MaterialData = new InputMaterial();
            mesh.MaterialData->pMat = mat;
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
                light->set_Intensity(10.0f);
                light->Init(renderer);
            }
        }

    }

    m_pCamera->get_Component<Camera3D>()->Update(renderer);


    // ライトにカメラのTransformを持たせる
    Master::m_pLightManager->set_CameraTransform(m_pCamera->get_Transform());

    // パイプラインの作成
    if (!renderer.CreateRenerererPipeline(RENDER_PIPELINE_STATE::DEFAULT))
    {
        return false;
    }

    return true;
}


//*---------------------------------------------------------------------------------------
//* @:SceneManager Class 
//*【?】更新
//* 引数：1.RendererEngine
//* 返値：void
//*----------------------------------------------------------------------------------------
void SceneManager::Update(RendererEngine& renderer)
{
    //// シーンの更新
    //int newState = m_SceneStateMap[m_CrntSceneState]->Update(renderer);

    //// シーン状態が変わったら現在のシーンを終了し、新しいシーンを呼ぶ
    //if (newState != m_CrntSceneState)
    //{
    //    m_CrntSceneState = newState;
    //}

    
    static float counter = 0.0f;
    counter += 0.01f;


    // オブジェクト更新
    Master::m_pGameObjectManager->ObjectUpdate(renderer);

    //-----------------------------------------------------------------------------
    // ■ アリの移動処理
    //-----------------------------------------------------------------------------
    for (int i = 0; i < 2; i++)
    {
        auto antTransform = m_pAnt[i]->get_Transform().lock();
        if (antTransform)
        {
            VEC3 pos = VEC3();
            pos.y = 15.0f;
            VEC3 crntPos = antTransform->get_VEC3ToPos();
            VEC3 crntRot = antTransform->get_VEC3ToRotateToRad();

            if (i % 2 == 0) {
                pos.z = sin(counter) * 300.0f;
            }
            else{
                pos.x = cos(counter) * 300.0f;
            }

            VEC3 velocity = pos - crntPos;

            // 移動ベクトルから、Y軸周りの回転角度を求める
            float targetAngle = atan2(velocity.x, velocity.z);

            targetAngle = Lerp(crntRot.y, targetAngle, 0.1f);

            antTransform->set_Pos(pos);
            antTransform->set_RotateToRad(0, targetAngle, 0);
        }
    }
    
    //-----------------------------------------------------------------------------
    // ■ 爆撃機の移動処理
    //-----------------------------------------------------------------------------
    static VEC3 B2PrevPos[3]{};
    VEC3 center = VEC3(0.0f, 0.0f, 0.0f);
    float radius = 700.0f;
    for (int i = 0; i < 3; i++)
    {
        auto B2Transform = m_pBomber[i]->get_Transform().lock();
        if (B2Transform)
        {
            float intervalDist = 200.0f * i;

            VEC3 pos = VEC3(0.0f, 0.0f, 0.0f);
            pos.x = center.x + (radius - intervalDist) * cos(counter);
            pos.y = 300.0f + i * 100.0f;
            pos.z = center.z + (radius - intervalDist) * sin(counter);

            // 速度 = 今の座標 - 前の座標
            VEC3 velocity = pos - B2PrevPos[i];

            // 移動ベクトルから、Y軸周りの回転角度を求める
            float targetAngle = atan2(velocity.x, velocity.z);

            // 前の座標として保持
            B2PrevPos[i] = pos;

            B2Transform->set_Pos(pos);
            B2Transform->set_RotateToRad(0.0f, targetAngle, 0.0f);
        }
    }

    m_pCamera->get_Component<Camera3D>()->Update(renderer);

    // カメラ更新
    auto viewMatrix = m_pCamera->get_Component<Camera3D>()->get_ViewMatrix();

    // ビュー変換し定数バッファへ送る
    if (!renderer.SetupViewTransform(viewMatrix)) {
        return;
    };

    Master::m_pEditorManager->Update(renderer);

    auto camCom = m_pCamera->get_Component<Camera3D>();
    float farClip = camCom->get_Far();
    float nearClip = camCom->get_Near();
    float fov = camCom->get_Fov();

    // プロジェクション変換行列の設定 一回でいい
    renderer.SetupProjectionTransform(renderer.get_ScreenWidth(), renderer.get_ScreenHeight(), farClip, nearClip, fov);
}


//*---------------------------------------------------------------------------------------
//* @:SceneManager Class 
//*【?】描画
//* 引数：1.RendererEngine
//* 返値：void
//*----------------------------------------------------------------------------------------
void SceneManager::Draw(RendererEngine& renderer)
{
    auto camCom = m_pCamera->get_Component<Camera3D>();
    float farClip   = camCom->get_Far();
    float nearClip  = camCom->get_Near();
    float fov       = camCom->get_Fov();

    // プロジェクション変換行列の設定 一回でいいけど、今回はfovなど編集できるようにしているので...
    renderer.SetupProjectionTransform(renderer.get_ScreenWidth(), renderer.get_ScreenHeight(), fov, nearClip, farClip);

    // レンダリングパイプラインの実行
    renderer.ExecuteDefaultRendererPipeline(RENDER_PIPELINE_STATE::DEFAULT);

    //// シーンの描画
    //m_SceneStateMap[m_CrntSceneState]->Draw(renderer);
}


//*---------------------------------------------------------------------------------------
//* @:SceneManager Class 
//*【?】終了
//* 引数：1.RendererEngine
//* 返値：void
//*----------------------------------------------------------------------------------------
void SceneManager::Term(RendererEngine &renderer)
{
}


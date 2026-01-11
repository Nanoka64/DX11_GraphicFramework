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
    m_CrntSceneState(0),
    m_PointLightRange(0),
    m_LightDir({ 0,0,0 }),
    m_LightPos({ 0,1,0 })
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
            m_pCamera->add_Component<Camera3D>();
            m_pCamera->get_Transform().lock()->set_Pos(0.0f, 800.0f, -1000.0f);
            m_pCamera->set_LayerRank(100);
            m_pCamera->set_StatusFlag(OBJECT_STATUS_BITFLAG::IS_ACTIVE);
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
            m_pPlayer->get_Transform().lock()->set_Pos(0.0f, 0.0f, -50.0f);
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
            light->set_Intensity(0.1f);
            light->set_Player(m_pPlayer);
            light->Init(renderer);

            obj->get_Transform().lock()->set_Pos(VEC3(0.0f, 1000.0f, -1000.0f));
            obj->get_Transform().lock()->set_Scale(VEC3(10.0, 10.0, 15.0));
            obj->get_Transform().lock()->set_RotateToRad(VEC3(0.0f, -0.0f, 0.0f));
        }

        /* ポイントライトの生成 (Cubuで分かりやすく)*/
        {
            Material* mat = new Material;
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
            obj->get_Transform().lock()->set_Scale(30.0f, 30.0f, 30.0f);
            obj->get_Transform().lock()->set_Pos(0.0f, 0.0f, -10.0f);
            obj->get_Transform().lock()->set_RotateToDeg(90.0f, 0.0f, 0.0f);
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
            model.Path = "Resource/Model/Enemy/trader_ant_lowpoly.fbx";
            model.ObjTag = "Ant1";
            model.IsAnim = true;
            model.MatNum = 1;
            model.MaterialData = new InputMaterial();
            model.MaterialData->MatIndex = 0;
            model.MaterialData->pMat = mat;
            model.ShaderType = SHADER_TYPE::DEFERRED_STD_SKINNED_N;
            auto obj = MeshFactory::CreateModel(model);
            obj->get_Component<Transform>()->set_Scale(0.1f, 0.1f, 0.1f);
            obj->get_Component<Transform>()->set_RotateToDeg(0.0f, 180, 0.0);
            obj->get_Component<Transform>()->set_Pos(0.0f, 0.0f, -100.0);
            obj->get_Component<SkinnedMeshAnimator>()->set_IsAnim(true);
            obj->get_Component<SkinnedMeshAnimator>()->set_AnimIndex(0);

            model.ObjTag = "Ant2";
            obj = MeshFactory::CreateModel(model);
            obj->get_Component<Transform>()->set_Scale(0.1f, 0.1f, 0.1f);
            obj->get_Component<Transform>()->set_Pos(0.0f, 0.0f, -100.0);
            obj->get_Component<Transform>()->set_RotateToDeg(0.0f, 90, 0.0);
            obj->get_Component<SkinnedMeshAnimator>()->set_IsAnim(true);
            obj->get_Component<SkinnedMeshAnimator>()->set_AnimIndex(0);

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
            model.Path = "Resource/Model/b-2/test_b-2.fbx";
            model.ObjTag = "B-2";
            model.IsAnim = false;
            model.MatNum = 1;
            model.MaterialData = new InputMaterial();
            model.MaterialData->MatIndex = 0;
            model.MaterialData->pMat = mat;
            model.ShaderType = SHADER_TYPE::DEFERRED_STD_SKINNED_N;
            auto obj = MeshFactory::CreateModel(model);
            obj->get_Component<Transform>()->set_Scale(0.15f, 0.15f, 0.15f);
            obj->get_Component<Transform>()->set_Pos(0.0f, 150.0f, 0.0f);
            obj->set_LayerRank(0);
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
            obj->get_Component<Transform>()->set_Scale(1.0f, 1.0f, 1.0f);
            obj->get_Component<Transform>()->set_Pos(0.0f, 00.0f, 10.0f);
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
            auto obj = MeshFactory::CreateModel(model);
            obj->get_Component<Transform>()->set_Scale(100.0f, 100.0f, 50.0f);
            obj->get_Component<Transform>()->set_Pos(300.0f, 0.0f, 0.0f);
            obj->get_Component<Transform>()->set_RotateToDeg(90.0f, 0.0f, 0.0f);
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
                auto obj = MeshFactory::CreateBillboard(billboard);
                obj->get_Transform().lock()->set_Pos(pos);
                obj->get_Transform().lock()->set_Scale(50, 50, 50);
                obj->set_Tag("Billboard" + std::to_string(i));
            }
        }
    }

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

    static float a = 0.f;
    a += 0.01f;

    // 光強度
    static float intensity = 0.1f;

    static VEC3 pLigPos{ 0,0,0 };
    static VEC3 ligCol{ 1,1,1 };
    static float pointIntensity = 10.0f;

    // ライトのデバッグ ******************************************************
    auto lig = Master::m_pGameObjectManager->get_ObjectByTag("PointLight0");
    lig->get_Component<Transform>()->set_Pos(pLigPos);
    lig->get_Component<PointLight>()->set_Range(m_PointLightRange);
    lig->get_Component<PointLight>()->set_Intensity(pointIntensity);
    lig->get_Component<PointLight>()->set_LightColor(ligCol);

    auto dlig = Master::m_pGameObjectManager->get_ObjectByTag("DirLight");
    auto rad = dlig->get_Component<Transform>();
    rad->set_RotateToRad(m_LightDir);
    rad->set_Pos(m_LightPos);
    dlig->get_Component<DirectionalLight>()->set_Intensity(intensity);

    auto b_2Obj = Master::m_pGameObjectManager->get_ObjectByTag("B-2");
    rad = b_2Obj->get_Component<Transform>();
    rad->set_RotateToRad(0.0, 0.0, sin(a) * 0.5f);

    // ライトのデバッグ

    Master::m_pDebugger->BeginDebugWindow(  U8ToChar(u8"ライトの設定"));
    if (Master::m_pDebugger->DG_TreeNode(U8ToChar(u8"ディレクションライト")))
    {
        Master::m_pDebugger->DG_DragVec3(       U8ToChar(u8"位置"), &m_LightPos, 1.0f, -3000.0f, 3000.0f);
        Master::m_pDebugger->DG_DragVec3(       U8ToChar(u8"方向"), &m_LightDir, 0.005f, -3.0f, 3.0f);
        Master::m_pDebugger->DG_SliderFloat(    U8ToChar(u8"強度"), 1, &intensity, 0.0f, 100.0f);
        Master::m_pDebugger->DG_TreePop();
    }
    if (Master::m_pDebugger->DG_TreeNode(U8ToChar(u8"ポイントライト")))
    {
        Master::m_pDebugger->DG_DragVec3(       U8ToChar(u8"位置"), &pLigPos, 1.0f, -10000.0f, 10000.0f);
        Master::m_pDebugger->DG_SliderFloat(    U8ToChar(u8"範囲"), 1, &m_PointLightRange, 0.0f, 10000.0f);
        Master::m_pDebugger->DG_SliderFloat(    U8ToChar(u8"強度"), 1, &pointIntensity, 0.0f, 1000.0f);
        Master::m_pDebugger->DG_ColorEdit3(     U8ToChar(u8"カラー"), &ligCol);
        Master::m_pDebugger->DG_TreePop();
    }
    Master::m_pDebugger->EndDebugWindow();

    // オブジェクト更新
    Master::m_pGameObjectManager->ObjectUpdate(renderer);
    
    // カメラ更新
    auto viewMatrix = m_pCamera->get_Component<Camera3D>()->get_ViewMatrix();

    // ビュー変換
    if (!renderer.SetupViewTransform(viewMatrix)) {
        return;
    };
}


//*---------------------------------------------------------------------------------------
//* @:SceneManager Class 
//*【?】描画
//* 引数：1.RendererEngine
//* 返値：void
//*----------------------------------------------------------------------------------------
void SceneManager::Draw(RendererEngine& renderer)
{
    static float blurIntensity = 1.0f;

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

    //SAFE_DELETE(m_pGaussianBlur);
}


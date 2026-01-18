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
bool SceneManager::Init(RendererEngine& renderer)
{
    // ステートマシンの作成
    SceneFactory::Create(m_StateMachine, renderer);

    // マテリアルの作成 (今後CSVで読み込むようにする)
    {
        /* プレイヤー */
        {
            Material mat;
            mat.m_DiffuseMap.Texture = Master::m_pResourceManager->LoadWIC_Texture(L"Resource/Texture/外壁S050.jpg");
            mat.m_NormalMap.Texture = Master::m_pResourceManager->LoadWIC_Texture(L"Resource/Texture/DefaultN_Map.png");
            mat.m_DiffuseColor = VEC4(0.4f, 0.4f, 0.6f, 1.0f);
            mat.m_SpecularPower = 150.0f;
            mat.m_SpecularColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);

            // マテリアル登録
            Master::m_pResourceManager->RegisterMaterialData("PlayerModel", mat);
        }
        /* 兵士 */
        {
            Material mat[2];
            mat[0].m_DiffuseMap.Texture = Master::m_pResourceManager->LoadWIC_Texture(L"Resource/Model/Player/textures/Soldier_Body_diffuse.png");
            mat[0].m_NormalMap.Texture = Master::m_pResourceManager->LoadWIC_Texture(L"Resource/Model/Player/textures/Soldier_Body_normal.png");
            mat[0].m_SpecularMap.Texture = Master::m_pResourceManager->LoadWIC_Texture(L"Resource/Model/Player/textures/Soldier_Body_specular.png");
            mat[0].m_DiffuseColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);
            mat[0].m_SpecularPower = 50.0f;
            mat[0].m_SpecularColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);

            // マテリアル登録
            Master::m_pResourceManager->RegisterMaterialData("soldier_body", mat[0]);

            mat[1].m_DiffuseMap.Texture = Master::m_pResourceManager->LoadWIC_Texture(L"Resource/Model/Player/textures/Soldier_head_diffuse.png");
            mat[1].m_NormalMap.Texture = Master::m_pResourceManager->LoadWIC_Texture(L"Resource/Model/Player/textures/Soldier_head_normal.png");
            mat[1].m_SpecularMap.Texture = Master::m_pResourceManager->LoadWIC_Texture(L"Resource/Model/Player/textures/Soldier_head_specular.png");
            mat[1].m_DiffuseColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);
            mat[1].m_SpecularPower = 50.0f;
            mat[1].m_SpecularColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);

            // マテリアル登録
            Master::m_pResourceManager->RegisterMaterialData("soldier_head", mat[1]);
        }
        /* ディレクションライト*/
        {
            Material mat;
            mat.m_DiffuseMap.Texture = Master::m_pResourceManager->LoadWIC_Texture(L"Resource/Texture/Light_Img.png");
            mat.m_DiffuseColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);
            mat.m_SpecularPower = 1.0f;

            // マテリアル登録
            Master::m_pResourceManager->RegisterMaterialData("DirLight", mat);
        }
        /* 壁 */
        {
            Material mat;
            mat.m_DiffuseMap.Texture = Master::m_pResourceManager->LoadWIC_Texture(L"Resource/Texture/Magic2.png");
            mat.m_NormalMap.Texture = Master::m_pResourceManager->LoadWIC_Texture(L"Resource/Texture/外壁W040_n.png");
            mat.m_DiffuseColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);
            mat.m_SpecularColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);
            mat.m_SpecularPower = 20.0f;

            // マテリアル登録
            Master::m_pResourceManager->RegisterMaterialData("Wall", mat);
        }
        /* アリ */
        {
            Material mat;
            mat.m_DiffuseMap.Texture = Master::m_pResourceManager->LoadWIC_Texture(L"Resource/Model/Enemy/trader_ant_lowpoly.fbm/new_bake_ant.png");
            mat.m_NormalMap.Texture = Master::m_pResourceManager->LoadWIC_Texture(L"Resource/Model/Enemy/trader_ant_lowpoly.fbm/new_bake_ant_n.png");
            mat.m_DiffuseColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);
            mat.m_SpecularPower = 100.0f;
            mat.m_SpecularColor = VEC4(0.5f, 0.5f, 0.5f, 1.0f);

            // マテリアル登録
            Master::m_pResourceManager->RegisterMaterialData("Ant", mat);
        }
        /* B-2 */
        {
            Material mat;
            mat.m_DiffuseMap.Texture = Master::m_pResourceManager->LoadWIC_Texture(L"Resource/Model/b-2/textures/ggg_diffuseOriginal.jpeg");
            mat.m_SpecularMap.Texture = Master::m_pResourceManager->LoadWIC_Texture(L"Resource/Model/b-2/textures/ggg_metallic.jpeg");
            mat.m_NormalMap.Texture = Master::m_pResourceManager->LoadWIC_Texture(L"Resource/Model/b-2/textures/ggg_normal.jpeg");
            mat.m_DiffuseColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);
            mat.m_SpecularColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);
            mat.m_SpecularPower = 100.0f;

            // マテリアル登録
            Master::m_pResourceManager->RegisterMaterialData("B_2", mat);
        }
        /* クレイモア */
        {
            Material mat;
            mat.m_DiffuseMap.Texture = Master::m_pResourceManager->LoadWIC_Texture(L"Resource/Model/Claymore/Mat_Base_Color.png");
            mat.m_SpecularMap.Texture = Master::m_pResourceManager->LoadWIC_Texture(L"Resource/Model/Claymore/Mat_Roughness.png");
            mat.m_NormalMap.Texture = Master::m_pResourceManager->LoadWIC_Texture(L"Resource/Model/Claymore/Mat_Normal_DirectX.png");
            mat.m_DiffuseColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);
            mat.m_SpecularColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);
            mat.m_SpecularPower = 200.0f;

            // マテリアル登録
            Master::m_pResourceManager->RegisterMaterialData("Claymore", mat);
        }
        /* 建物 */
        {
            Material mat;
            mat.m_DiffuseMap.Texture = Master::m_pResourceManager->LoadWIC_Texture(L"Resource/Model/Building/01/texture/building5-lo.jpg");
            mat.m_NormalMap.Texture = Master::m_pResourceManager->LoadWIC_Texture(L"Resource/Texture/DefaultN_Map.png");
            mat.m_DiffuseColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);
            mat.m_SpecularColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);
            mat.m_SpecularPower = 150.0f;

            // マテリアル登録
            Master::m_pResourceManager->RegisterMaterialData("Building", mat);
        }
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
        /* 地面 */
        {
            Material mat;
            mat.m_DiffuseMap.Texture = Master::m_pResourceManager->LoadWIC_Texture(L"Resource/Texture/aerial_grass_rock_diff_4k.png");
            mat.m_NormalMap.Texture = Master::m_pResourceManager->LoadWIC_Texture(L"Resource/Texture/aerial_grass_rock_nor_dx_4k.png");
            mat.m_SpecularColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);
            mat.m_SpecularPower = 50.0f;

            // マテリアル登録
            Master::m_pResourceManager->RegisterMaterialData("Ground", mat);
        }
        /* スカイボックス */
        {
            Material mat;
            mat.m_DiffuseMap.Texture = Master::m_pResourceManager->LoadDDS_CubeMap_Texture(L"Resource/Texture/cloudy_skybox.dds");

            // マテリアル登録
            Master::m_pResourceManager->RegisterMaterialData("SkyBox", mat);
        }
        /* ビルボード */
        {
            Material mat;
            mat.m_DiffuseMap.Texture = Master::m_pResourceManager->LoadWIC_Texture(L"Resource/Texture/Particle/Weak_1024.png");
            mat.m_DiffuseColor = VEC4(0.1f, 0.1f, 0.1f, 1.0f);
            mat.m_SpecularColor = VEC4(0.1f, 0.1f, 0.1f, 1.0f);
            mat.m_SpecularPower = 100.0f;
            mat.m_BlendMode = BLEND_MODE::ALPHA;

            // マテリアル登録
            Master::m_pResourceManager->RegisterMaterialData("Billboard", mat);
        }
        /* ポイントライト */
        {
            Material mat;
            mat.m_DiffuseMap.Texture = Master::m_pResourceManager->LoadWIC_Texture(L"Resource/Texture/外壁W040.jpg");
            mat.m_NormalMap.Texture = Master::m_pResourceManager->LoadWIC_Texture(L"Resource/Texture/外壁W040_n.png");
            mat.m_DiffuseColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);
            mat.m_SpecularColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);
            mat.m_SpecularPower = 20.0f;

            // マテリアル登録
            Master::m_pResourceManager->RegisterMaterialData("PointLight", mat);
        }
    }

    {}

    // オブジェクトの生成
    {
        /* カメラの作成 */
        {
            m_pCameraObj = Instantiate(std::move(std::make_shared<GameObject>()));
            if (m_pCameraObj == nullptr)
            {
                return false;
            }
            m_pCameraObj->Init(renderer);
            m_pCameraObj->set_Tag("Camera");
            m_pCameraObj->clear_StatusFlag(OBJECT_STATUS_BITFLAG::IS_ACTIVE);
            m_pCameraObj->get_Transform().lock()->set_Pos(0.0f, 800.0f, -1000.0f);
            m_pCameraComp = m_pCameraObj->add_Component<Camera3D>(); // カメラコンポーネントの追加
        }

        /* プレイヤー モデルの生成 */
        {
            // マテリアル取得
            auto matPtr = Master::m_pResourceManager->LoadMaterial("PlayerModel");

            SetupMaterialInfo matInfo[1];
            matInfo[0].Index = 0;
            matInfo[0].pMaterialData = matPtr;

            CreateModelInfo model;
            model.pRenderer = &renderer;
            model.Path = "Resource/Model/Player2/AL_Standard.fbx";
            model.ObjTag = "Player";
            model.IsAnim = true;
            model.MatNum = 1;
            model.SetupMaterial = matInfo;
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
            m_pCameraComp->set_FocusObject(m_pPlayer);
        }

        /* 兵士 */
        {
            // マテリアル取得
            auto matPtr1 = Master::m_pResourceManager->LoadMaterial("soldier_body");
            auto matPtr2 = Master::m_pResourceManager->LoadMaterial("soldier_head");

            SetupMaterialInfo matInfo[3];
            matInfo[0].Index = 0;   
            matInfo[0].pMaterialData = matPtr1; // 体

            matInfo[1].Index = 1;
            matInfo[1].pMaterialData = matPtr1; // ヘルメット（体）

            matInfo[2].Index = 2;
            matInfo[2].pMaterialData = matPtr2; // 頭

            CreateModelInfo model;
            model.pRenderer = &renderer;
            model.Path = "Resource/Model/Player/chara.fbx";
            model.ObjTag = "Soldier";
            model.IsAnim = true;
            model.MatNum = 3;
            model.SetupMaterial = matInfo;
            model.ShaderType = SHADER_TYPE::DEFERRED_STD_SKINNED_N;
            auto obj = MeshFactory::CreateModel(model);
            obj->get_Component<SkinnedMeshAnimator>()->set_AnimIndex(-1);
        }

        /* ディレクションライトの生成(Cubuで分かりやすく) */
        {
            // マテリアル取得
            auto matPtr = Master::m_pResourceManager->LoadMaterial("DirLight");

            SetupMaterialInfo matInfo[1];
            matInfo[0].Index = 0;
            matInfo[0].pMaterialData = matPtr;

            CreateUtilityMeshInfo mesh;
            mesh.pRenderer = &renderer;
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
            light->set_Intensity(3.0f);
            light->set_LightCameraTrackingObj(m_pCameraObj);
            light->Init(renderer);

            obj->get_Transform().lock()->set_Pos(VEC3(0.0f, 1000.0f, -1000.0f));
            obj->get_Transform().lock()->set_Scale(VEC3(30.0, 30.0, 80.0));
            obj->get_Transform().lock()->set_RotateToRad(VEC3(0.85f, 1.6f, 0.0f));
        }

        /* スフィア */
        {
            // マテリアル取得
            auto matPtr = Master::m_pResourceManager->LoadMaterial("Wall");

            SetupMaterialInfo matInfo[1];
            matInfo[0].Index = 0;
            matInfo[0].pMaterialData = matPtr;

            CreateUtilityMeshInfo mesh;
            mesh.pRenderer = &renderer;
            mesh.Type = UTILITY_MESH_TYPE::SPHERE;
            mesh.MatNum = 1;
            mesh.MaterialData = matInfo;
            mesh.IsActive = true;
            mesh.ShaderType = SHADER_TYPE::DEFERRED_STD_STATIC_N;
            mesh.IsNormalMap = true;
            mesh.ObjTag = "Sphere";

            auto obj = MeshFactory::CreateUtilityMesh(mesh);
            obj->get_Transform().lock()->set_Scale(100.0f, 100.0f, 100.0f);
            obj->get_Transform().lock()->set_Pos(-300.0f, 80.0f, 600.0f);
            obj->get_Transform().lock()->set_RotateToDeg(-60.0f, 0.0f, 0.0f);
        }

        /* アリ モデルの生成 */
        {
            // マテリアル取得
            auto matPtr = Master::m_pResourceManager->LoadMaterial("Ant");

            SetupMaterialInfo matInfo[1];
            matInfo[0].Index = 0;
            matInfo[0].pMaterialData = matPtr;

            CreateModelInfo model;
            model.pRenderer = &renderer;
            model.Path = "Resource/Model/Enemy/ant_lowpoly.fbx";
            model.ObjTag = "Ant1";
            model.IsAnim = true;
            model.MatNum = 1;
            model.SetupMaterial = matInfo;
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
            // マテリアル取得
            auto matPtr = Master::m_pResourceManager->LoadMaterial("B_2");

            SetupMaterialInfo matInfo[1];
            matInfo[0].Index = 0;
            matInfo[0].pMaterialData = matPtr;

            CreateModelInfo model;
            model.pRenderer = &renderer;
            model.Path = "Resource/Model/b-2/B-2_NonBone.fbx";
            model.ObjTag = "B-2";
            model.IsAnim = false;
            model.MatNum = 1;
            model.SetupMaterial = matInfo;
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
            // マテリアル取得
            auto matPtr = Master::m_pResourceManager->LoadMaterial("Claymore");

            SetupMaterialInfo matInfo[1];
            matInfo[0].Index = 0;
            matInfo[0].pMaterialData = matPtr;

            CreateModelInfo model;
            model.pRenderer = &renderer;
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
            auto matPtr = Master::m_pResourceManager->LoadMaterial("Building");

            SetupMaterialInfo matInfo[1];
            matInfo[0].Index = 0;
            matInfo[0].pMaterialData = matPtr;

            CreateModelInfo model;
            model.pRenderer = &renderer;
            model.Path = "Resource/Model/Building/01/building01.fbx";
            model.ObjTag = "Building";
            model.IsAnim = false;
            model.MatNum = 1;
            model.SetupMaterial = matInfo;
            model.ShaderType = SHADER_TYPE::DEFERRED_STD_STATIC;
            m_pTempObj = MeshFactory::CreateModel(model);
            m_pTempObj->get_Component<Transform>()->set_Scale(0.5f, 0.5f, 0.5f);
            m_pTempObj->get_Component<Transform>()->set_Pos(300.0f, 0.0f, 0.0f);
            m_pTempObj->get_Component<Transform>()->set_RotateToDeg(0.0f, 0.0f, 0.0f);
        }

        /* クモ モデルの生成 */
        {
            // マテリアル取得
            auto matPtr1 = Master::m_pResourceManager->LoadMaterial("Spider_1");
            auto matPtr2 = Master::m_pResourceManager->LoadMaterial("Spider_2");
            auto matPtr3 = Master::m_pResourceManager->LoadMaterial("Spider_3");
            auto matPtr4 = Master::m_pResourceManager->LoadMaterial("Spider_4");

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
            model.pRenderer = &renderer;
            model.Path = "Resource/Model/fbx/Spider_3.fbx";
            model.ObjTag = "Spider";
            model.IsAnim = true;
            model.MatNum = 4;
            model.IsActive = false;

            model.SetupMaterial = matInfo;

            model.ShaderType = SHADER_TYPE::DEFERRED_STD_SKINNED_N;
            m_pTempObj = MeshFactory::CreateModel(model);
            m_pTempObj->get_Component<SkinnedMeshAnimator>()->set_IsAnim(true);
            m_pTempObj->get_Component<Transform>()->set_Scale(0.5f, 0.5f, 0.5f);
            m_pTempObj->get_Component<Transform>()->set_Pos(0.0f, 0.0f, 0.0f);
            m_pTempObj->get_Component<Transform>()->set_RotateToDeg(0.0f, 0.0f, 0.0f);
        }

        /* 地面の生成 */
        {
            // マテリアル取得
            auto matPtr = Master::m_pResourceManager->LoadMaterial("Ground");

            SetupMaterialInfo matInfo[1];
            matInfo[0].Index = 0;
            matInfo[0].pMaterialData = matPtr;

            CreateUtilityMeshInfo mesh;
            mesh.pRenderer = &renderer;
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
        }

        /* スカイボックスの生成 */
        {
            // マテリアル取得
            auto matPtr = Master::m_pResourceManager->LoadMaterial("SkyBox");

            SetupMaterialInfo matInfo[1];
            matInfo[0].Index = 0;
            matInfo[0].pMaterialData = matPtr;

            CreateSkyboxInfo skyInfo;
            skyInfo.pRenderer = &renderer;
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
            auto matPtr = Master::m_pResourceManager->LoadMaterial("Billboard");

            SetupMaterialInfo matInfo[1];
            matInfo[0].Index = 0;
            matInfo[0].pMaterialData = matPtr;


            CreateBillboradInfo billboard;
            billboard.pRenderer = &renderer;
            billboard.Type = BILLBOARD_USAGE_TYPE::SIMPLE;
            billboard.ShaderType = SHADER_TYPE::FORWARD_UNLIT_STATIC;
            billboard.IsActive = false;
            billboard.MatNum = 1;
            billboard.MaterialData = matInfo;

            for (int i = 0; i < 10; i++)
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
            auto matPtr = Master::m_pResourceManager->LoadMaterial("PointLight");

            SetupMaterialInfo matInfo[1];
            matInfo[0].Index = 0;
            matInfo[0].pMaterialData = matPtr;

            CreateUtilityMeshInfo mesh;
            mesh.pRenderer = &renderer;
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
                light->Init(renderer);
            }
        }
    }

    // ライトにカメラのTransformを持たせる
    Master::m_pLightManager->set_CameraTransform(m_pCameraComp->get_OwnerObj().lock()->get_Transform());

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

    // カメラ操作の更新
    m_pCameraComp->ViewProcessUpdate();

    // カメラ更新
    auto viewMatrix = m_pCameraComp->get_ViewMatrix();

    // ビュー変換し定数バッファへ送る
    if (!renderer.SetupViewTransform(viewMatrix)) {
        return;
    };

    Master::m_pEditorManager->Update(renderer);
}


//*---------------------------------------------------------------------------------------
//* @:SceneManager Class 
//*【?】描画
//* 引数：1.RendererEngine
//* 返値：void
//*----------------------------------------------------------------------------------------
void SceneManager::Draw(RendererEngine& renderer)
{
    float farClip   = m_pCameraComp->get_Far();
    float nearClip  = m_pCameraComp->get_Near();
    float fov       = m_pCameraComp->get_Fov();

    float screen_width = static_cast<float>(renderer.get_ScreenWidth());
    float screen_height = static_cast<float>(renderer.get_ScreenHeight());

    // プロジェクション変換行列の設定 一回でいいけど、今回はfovなど編集できるようにしているので...
    renderer.SetupProjectionTransform(screen_width, screen_height, fov, nearClip, farClip);

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


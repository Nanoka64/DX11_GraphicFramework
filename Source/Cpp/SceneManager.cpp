#include "pch.h"
#include "SceneManager.h"
#include "RendererManager.h"
#include "DirectWriteManager.h"
#include "ResourceManager.h"
#include "GameObjectManager.h"
#include "InputFactory.h"
#include "Cubu.h"
#include "Quad.h"
#include "SceneFactory.h"
#include "Camera.h"
#include "Component_3DCamera.h"
#include "Component_SkinnedMeshAnimator.h"
#include "Component_ModelMeshResource.h"
#include "Component_ModelMeshRenderer.h"
#include "Component_MeshRenderer.h"
#include "MeshFactory.h"
#include "Component_DirectionalLight.h"
#include "Component_PointLight.h"

using namespace VECTOR4;
using namespace VECTOR3;
using namespace VECTOR2;
using namespace Tool::UV;
using namespace Input;

//*---------------------------------------------------------------------------------------
//* @:SceneManager Class 
//*【?】コンストラクタ
//* 引数：なし
//*----------------------------------------------------------------------------------------
SceneManager::SceneManager():
    m_pCamera(),
    m_testTriangle(VECTOR3::VEC3(0,0,0),1.0f),
    m_StateMachine(this)
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
//* 引数：1.RendererManager
//* 返値：bool
//*----------------------------------------------------------------------------------------
bool SceneManager::Init(RendererManager &renderer)
{
    // ステートマシンの作成
    SceneFactory::Create(m_StateMachine, renderer);

    std::vector<std::weak_ptr<GameObject>> objectList;

    // オブジェクトの生成
    {
        {
            /* カメラの生成 */
            std::weak_ptr<GameObject> pCam = Instantiate(std::move(std::make_shared<GameObject>()));
            pCam.lock()->Init(renderer);
            pCam.lock()->set_Tag("Camera");
            pCam.lock()->add_Component<Camera3D>();
            pCam.lock()->get_Transform().lock()->set_Pos(0.0f, 0.0f, -10.0f);
        }

        {
            /* ディレクションライトの生成 */
            std::weak_ptr<GameObject> pDLight = Instantiate(std::move(std::make_shared<GameObject>()));
            pDLight.lock()->get_Transform().lock()->set_RotateToRad(VEC3(0.01f, -0.01f, 0.01f));
            auto light = pDLight.lock()->add_Component<DirectionalLight>();
            pDLight.lock()->set_Tag("DirLight");
            light->CreateCBuffer(renderer.get_Device());
            light->set_CameraTransform(GameObjectManager::Instance().get_ObjectByTag("Camera").lock()->get_Transform());
            light->set_LightColor(VEC3(1.0f, 1.0f, 1.0f));
            light->Init(renderer);
        }

        {
            /* ポイントライトの生成 */
            std::weak_ptr<GameObject> pDLight = Instantiate(std::move(std::make_shared<GameObject>()));
            pDLight.lock()->get_Transform().lock()->set_Pos(VEC3(0.0f, 0.0f, 0.0f));
            pDLight.lock()->set_Tag("PointLight");
            auto light = pDLight.lock()->add_Component<PointLight>();
            light->CreateCBuffer(renderer.get_Device());
            light->set_LightColor(VEC3(1.0f, 1.0f, 1.0f));
            light->set_Range(2500.0f);
            light->Init(renderer);
        }

        {
            /* プレイヤー モデルの生成 */
            MATERIAL mat[1];
            mat[0].DiffuseColor = VEC4(1.0, 0.0, 1.0, 1.0);

            CreateModelInfo model;
            model.pRenderer = &renderer;
            model.Path = "Resource/Model/Player2/AL_Standard.fbx";
            model.ObjTag = "Player";
            model.IsAnim = true;
            model.MatNum = 1;
            model.MaterialData = new InputMaterial();
            model.MaterialData->MatIndex = 0;
            model.MaterialData->pMat = mat;
            auto obj = MeshFactory::CreateModel(model);
            obj.lock()->get_Component<Transform>()->set_Scale(10.0f, 10.0f, 10.0f);
            obj.lock()->get_Component<SkinnedMeshAnimator>()->set_IsAnim(false);
            obj.lock()->get_Component<SkinnedMeshAnimator>()->set_AnimIndex(0);
        }

        {
            /* アリ モデルの生成 */
            MATERIAL mat[1];
            mat[0].Diffuse.Texture = ResourceManager::Instance().LoadTexture(L"Resource/Model/Enemy/trader_ant_lowpoly.fbm/new_bake_ant.png");
            mat[0].DiffuseColor = VEC4(0.0f, 0.0f, 0.0f, 0.0f);

            CreateModelInfo model;
            model.pRenderer = &renderer;
            model.Path   = "Resource/Model/Enemy/trader_ant_lowpoly.fbx";
            model.ObjTag = "Model";
            model.IsAnim = true;
            model.MatNum = 1;
            model.MaterialData = new InputMaterial();
            model.MaterialData->MatIndex = 0;
            model.MaterialData->pMat = mat;
            auto obj = MeshFactory::CreateModel(model);
            obj.lock()->get_Component<Transform>()->set_Scale(0.5f, 0.5f, 0.5f);
            obj.lock()->get_Component<SkinnedMeshAnimator>()->set_IsAnim(true);
            obj.lock()->get_Component<SkinnedMeshAnimator>()->set_AnimIndex(0);
        }

        {
            /* QUADの生成 */

            for (int i = -1; i < 2; i++)
            {
                MATERIAL* mat = new MATERIAL;
                mat->Diffuse.Texture = ResourceManager::Instance().LoadTexture(L"Resource/Texture/外壁W040.jpg");

                CreateUtilityMeshInfo mesh;
                mesh.pRenderer = &renderer;
                mesh.Type = UTILITY_MESH_TYPE::QUAD;
                mesh.ObjTag = "Quad" + std::to_string(i);
                mesh.MatNum = 1;
                mesh.MaterialData = new InputMaterial();
                mesh.MaterialData->pMat = mat;

                auto obj = MeshFactory::CreateUtilityMesh(mesh);
                obj.lock()->get_Transform().lock()->set_Scale(1000, 10, 1000);
                obj.lock()->get_Transform().lock()->set_Pos((i * 1000), -100, 0);
                obj.lock()->get_Transform().lock()->set_RotateToDeg(0, 0, (i * 90));
            }
        }

        {
            MATERIAL* mat = new MATERIAL;
            mat->Diffuse.Texture = ResourceManager::Instance().LoadTexture(L"Resource/Texture/Wood022_2K-JPG_Color.jpg");

            CreateUtilityMeshInfo mesh;
            mesh.pRenderer = &renderer;
            mesh.Type = UTILITY_MESH_TYPE::CUBU;
            mesh.ObjTag = "Cubu";
            mesh.MatNum = 1;
            mesh.MaterialData = new InputMaterial();
            mesh.MaterialData->pMat = mat;

            auto obj = MeshFactory::CreateUtilityMesh(mesh);
            obj.lock()->get_Transform().lock()->set_Scale(100, 100, 100);
            obj.lock()->get_Transform().lock()->set_Pos(0.0, 100, 0);
        }
    }

    // 参照を持たせる
    m_pCamera = GameObjectManager::Instance().get_ObjectByTag("Camera");

 //   if (!m_testTriangle.CreateVertexBuffer(renderer))
 //   {
 //       return false;
 //   }

 //   // アリモデル読み込み
 //   if (m_TestModel3.Setup(renderer, "Resource/Model/Enemy/trader_ant_lowpoly.fbx") == false) return false;
 //   m_TestModel3.set_TextureMap(TEXTURE_MAP_DIFFUSE, 0, L"Resource/Model/Enemy/trader_ant_lowpoly.fbm/new_bake_ant.png");
 //   m_TestModel3.set_AnimIndex(0);
 //   
 //   // キャラモデル↓
 //   //if (m_TestModel.Setup(renderer, "Resource/Model/Player/chara.fbx") == false)return false;   // swat
 //   if (m_TestModel.Setup(renderer, "Resource/Model/Player2/AL_Standard.fbx") == false)return false; // 動くほう
 //   //m_TestModel.set_Scale(VEC3(0.025f, 0.025f, 0.025f));

 //   // テクスチャマップ設定
 //   m_TestModel.set_TextureMap(TEXTURE_MAP_DIFFUSE, 0, L"Resource/Model/Player/Swat.fbm/Soldier_Body_diffuse.png");
 //   m_TestModel.set_TextureMap(TEXTURE_MAP_NORMAL,  0, L"Resource/Model/Player/Swat.fbm/Soldier_Body_normal.png");
 //   m_TestModel.set_TextureMap(TEXTURE_MAP_SPECULAR,0, L"Resource/Model/Player/Swat.fbm/Soldier_Body_specular.png");
 //   m_TestModel.set_TextureMap(TEXTURE_MAP_DIFFUSE, 1, L"Resource/Model/Player/Swat.fbm/Soldier_Body_diffuse.png");
 //   m_TestModel.set_TextureMap(TEXTURE_MAP_NORMAL,  1, L"Resource/Model/Player/Swat.fbm/Soldier_Body_normal.png");
 //   m_TestModel.set_TextureMap(TEXTURE_MAP_SPECULAR,1, L"Resource/Model/Player/Swat.fbm/Soldier_Body_specular.png");
 //   m_TestModel.set_TextureMap(TEXTURE_MAP_DIFFUSE, 2, L"Resource/Model/Player/Swat.fbm/Soldier_head_diffuse.png");
 //   m_TestModel.set_TextureMap(TEXTURE_MAP_NORMAL, 2, L"Resource/Model/Player/Swat.fbm/Soldier_head_normal.png");
 //   m_TestModel.set_TextureMap(TEXTURE_MAP_SPECULAR, 2, L"Resource/Model/Player/Swat.fbm/Soldier_head_specular.png");
 //   m_TestModel.set_AnimIndex(0);


 //   //// B-2モデル ↓
 //   //if (m_TestModel2.Setup(renderer, "Resource/Model/b-2/test_b-2.fbx") == false)return false;
 //   //m_TestModel2.set_TextureMap(TEXTURE_MAP_DIFFUSE,  0, L"Resource/Model/b-2/textures/ggg_diffuseOriginal.jpeg");
 //   //m_TestModel2.set_TextureMap(TEXTURE_MAP_NORMAL,   0, L"Resource/Model/b-2/textures/ggg_normal.jpeg");
 //   //m_TestModel2.set_TextureMap(TEXTURE_MAP_SPECULAR, 0, L"Resource/Model/b-2/textures/ggg_metallic.jpeg");   
 //   //
 //   // 蜘蛛モデル ↓
 //   if (m_TestModel2.Setup(renderer, "Resource/Model/fbx/Spider.fbx") == false)return false;
 //   m_TestModel2.set_TextureMap(TEXTURE_MAP_DIFFUSE,  0, L"Resource/Model/fbx/textures/Spinnen_Bein_tex.jpg");
 //   m_TestModel2.set_TextureMap(TEXTURE_MAP_DIFFUSE,  4, L"Resource/Model/fbx/textures/SH3.png");
 //   m_TestModel2.set_TextureMap(TEXTURE_MAP_NORMAL,   0, L"Resource/Model/fbx/textures/haar_detail_NRM.jpg");

 //   // モデルの回転設定
 //   //m_TestModel3.set_RotateToDeg(VEC3(90.f, 0.f, 0.f));
 //   //m_TestModel3.set_Pos(VEC3(0.f, 2.f, 0.f));
 //   //m_TestModel3.set_ParentMtx(&m_TestModel2);

 //   // キューブ生成
	//m_pCubu = new Cubu();
 //   m_pCubu->get_Transform().lock()->set_Pos(0.0f, 2.0f, 0.0f);
 //   m_pCubu->get_Transform().lock()->set_Scale(1.0f, 1.0f, 1.0f);
 //   SpriteUV uv[6];
 //   uv[0] = MakeSpriteUV(0.0f, 0.0f, 2048.0f, 2048.0f, 2048.0f, 2048.0f);    // 前
 //   uv[1] = MakeSpriteUV(0.0f, 0.0f, 2048.0f, 2048.0f, 2048.0f, 2048.0f);    // 右
 //   uv[2] = MakeSpriteUV(0.0f, 0.0f, 2048.0f, 2048.0f, 2048.0f, 2048.0f);    // 上
 //   uv[3] = MakeSpriteUV(0.0f, 0.0f, 2048.0f, 2048.0f, 2048.0f, 2048.0f);    // 下
 //   uv[4] = MakeSpriteUV(0.0f, 0.0f, 2048.0f, 2048.0f, 2048.0f, 2048.0f);    // 左
 //   uv[5] = MakeSpriteUV(0.0f, 0.0f, 2048.0f, 2048.0f, 2048.0f, 2048.0f);    // 裏
 //   MATERIAL* mat = new MATERIAL();
 //   mat->Diffuse.Texture = ResourceManager::Instance().LoadTexture(L"Resource/Texture/外壁W048.jpg");
 //   if (!m_pCubu->Setup(renderer, mat,1))return false;

 //   // 床用クワッド生成
 //   m_pQuad = new Quad();
 //   mat = new MATERIAL();
 //   mat->Diffuse.Texture = ResourceManager::Instance().LoadTexture(L"Resource/Texture/WoodFloor051_2K-PNG_Color.png");
 //   if (!m_pQuad->Setup(renderer, mat, 1))return false;
 //   m_pQuad->get_Component<Transform>()->set_Pos(VEC3(0.0f, -5.0f, 0.0f));
 //   m_pQuad->get_Component<Transform>()->set_Scale(VEC3(50.0f, 25.0f, 50.0f));
 //   m_pQuad->get_Component<Transform>()->set_RotateToDeg(VEC3(0.0f, 0.0f, 0.0f));

    return true;
}


//*---------------------------------------------------------------------------------------
//* @:SceneManager Class 
//*【?】更新
//* 引数：1.RendererManager
//* 返値：void
//*----------------------------------------------------------------------------------------
void SceneManager::Update(RendererManager& renderer)
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

    auto obj = GameObjectManager::Instance().get_ObjectByTag("Model");
    obj.lock()->get_Component<Transform>()->set_Pos(0, 0, sin(a) * 1000.0f);

    auto lig = GameObjectManager::Instance().get_ObjectByTag("PointLight");
    lig.lock()->get_Component<Transform>()->set_Pos(sin(a) * 1000.0f, 0, 0);


    // オブジェクト更新
    GameObjectManager::Instance().ObjectUpdate(renderer);
}


//*---------------------------------------------------------------------------------------
//* @:SceneManager Class 
//*【?】描画
//* 引数：1.RendererManager
//* 返値：void
//*----------------------------------------------------------------------------------------
void SceneManager::Draw(RendererManager& renderer)
{
    // オブジェクト描画
    GameObjectManager::Instance().ObjectRender(renderer);


    // カメラ更新
    auto viewMatrix = m_pCamera.lock()->get_Component<Camera3D>()->get_ViewMatrix();

    // ビュー変換
    if (!renderer.SetupViewTransform(viewMatrix)) {
        return;
    };

    //// シーンの描画
    //m_SceneStateMap[m_CrntSceneState]->Draw(renderer);
}


//*---------------------------------------------------------------------------------------
//* @:SceneManager Class 
//*【?】終了
//* 引数：1.RendererManager
//* 返値：void
//*----------------------------------------------------------------------------------------
void SceneManager::Term(RendererManager &renderer)
{
}


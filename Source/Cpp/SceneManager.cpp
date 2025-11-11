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
            /* ディレクションライトの生成(Cubuで分かりやすく) */
            MATERIAL *mat = new MATERIAL;
            mat->Diffuse.Texture = ResourceManager::Instance().LoadTexture(L"Resource/Texture/Wood022_2K-JPG_Color.jpg");
            mat->DiffuseColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);

            CreateUtilityMeshInfo mesh;
            mesh.pRenderer = &renderer;
            mesh.Type = UTILITY_MESH_TYPE::CUBU;
            mesh.ObjTag = "DirLight";
            mesh.MatNum = 1;
            mesh.MaterialData = new InputMaterial();
            mesh.MaterialData->pMat = mat;

            auto obj = MeshFactory::CreateUtilityMesh(mesh);
            auto light = obj.lock()->add_Component<DirectionalLight>();
            light->CreateCBuffer(renderer.get_Device());
            light->set_CameraTransform(GameObjectManager::Instance().get_ObjectByTag("Camera").lock()->get_Transform());
            light->set_LightColor(VEC3(1.0f, 1.0f, 1.0f));
            light->set_Intensity(0.1f);
            light->Init(renderer);

            obj.lock()->get_Transform().lock()->set_Pos(VEC3(0.0f, 1000.0f, -1000.0f));
            obj.lock()->get_Transform().lock()->set_Scale(VEC3(40, 40, 100));
            obj.lock()->get_Transform().lock()->set_RotateToRad(VEC3(1.0f, -1.0f, 1.0f));
        }

        {
            /* ポイントライトの生成 (Cubuで分かりやすく)*/
            MATERIAL *mat = new MATERIAL;
            mat->Diffuse.Texture = ResourceManager::Instance().LoadTexture(L"Resource/Texture/Wood022_2K-JPG_Color.jpg");
            mat->DiffuseColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);

            CreateUtilityMeshInfo mesh;
            mesh.pRenderer = &renderer;
            mesh.Type = UTILITY_MESH_TYPE::CUBU;
            mesh.ObjTag = "PointLight";
            mesh.MatNum = 1;
            mesh.MaterialData = new InputMaterial();
            mesh.MaterialData->pMat = mat;

            auto obj = MeshFactory::CreateUtilityMesh(mesh);
            obj.lock()->get_Transform().lock()->set_Pos(VEC3(0.0f, 0.0f, 0.0f));
            obj.lock()->get_Transform().lock()->set_Scale(VEC3(100, 100, 100));
            auto light = obj.lock()->add_Component<PointLight>();
            light->CreateCBuffer(renderer.get_Device());
            light->set_LightColor(VEC3(1.0f, 1.0f, 1.0f));
            light->set_Range(3000.0f);
            light->Init(renderer);
        }


        {
            /* プレイヤー モデルの生成 */
            MATERIAL mat[1];
            mat[0].Normal.Texture = ResourceManager::Instance().LoadTexture(L"Resource/Texture/DefaultN_Map.png");
            mat[0].DiffuseColor = VEC4(1.0, 0.0, 1.0, 1.0);
            mat[0].SpecularPower = 16.0f;
            mat[0].SpecularColor = VEC4(0.0f, 0.0f, 1.0f, 1.0f);

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
            mat[0].Normal.Texture = ResourceManager::Instance().LoadTexture(L"Resource/Model/Enemy/trader_ant_lowpoly.fbm/new_bake_ant_n.png");
            mat[0].DiffuseColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);
            mat[0].SpecularPower = 1.0f;
            mat[0].SpecularColor = VEC4(0.0f, 0.0f, 0.0f, 1.0f);

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
            /* B-2 モデルの生成 */
            MATERIAL mat[1];
            mat[0].Diffuse.Texture  = ResourceManager::Instance().LoadTexture(L"Resource/Model/b-2/textures/ggg_diffuseOriginal.jpeg");
            mat[0].Specular.Texture = ResourceManager::Instance().LoadTexture(L"Resource/Model/b-2/textures/ggg_metallic.jepg");
            mat[0].Normal.Texture   = ResourceManager::Instance().LoadTexture(L"Resource/Model/b-2/textures/ggg_normal.jpeg");
            mat[0].DiffuseColor     = VEC4(1.0f, 1.0f, 1.0f, 1.0f);
            mat[0].SpecularPower    = 12.0f;
            mat[0].SpecularColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);

            CreateModelInfo model;
            model.pRenderer = &renderer;
            model.Path = "Resource/Model/b-2/test_b-2.fbx";
            model.ObjTag = "B-2";
            model.IsAnim = false;
            model.MatNum = 1;
            model.MaterialData = new InputMaterial();
            model.MaterialData->MatIndex = 0;
            model.MaterialData->pMat = mat;
            auto obj = MeshFactory::CreateModel(model);
            obj.lock()->get_Component<Transform>()->set_Scale(0.5f, 0.5f, 0.5f);
        }        

        {
            /* QUADの生成 */

            for (int i = -1; i < 2; i++)
            {
                MATERIAL* mat = new MATERIAL;
                mat->Diffuse.Texture = ResourceManager::Instance().LoadTexture(L"Resource/Texture/外壁W040.jpg");
                mat->Normal.Texture = ResourceManager::Instance().LoadTexture(L"Resource/Texture/外壁W040_n.png");
                mat->SpecularColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);
                mat->SpecularPower = 0.5f;

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
            // CUBE
            MATERIAL* mat = new MATERIAL;
            mat->Diffuse.Texture = ResourceManager::Instance().LoadTexture(L"Resource/Texture/Wood022_2K-JPG_Color.jpg");
            mat->Diffuse.Texture = ResourceManager::Instance().LoadTexture(L"Resource/Texture/Wood022_2K-JPG_Color.jpg");
            mat->SpecularColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);
            mat->SpecularPower = 0.5f;

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

    // 光強度
    static float intensity = 1.0f;


    auto obj = GameObjectManager::Instance().get_ObjectByTag("Model");
    obj.lock()->get_Component<Transform>()->set_Pos(0, 0, sin(a) * 1000.0f);

    auto lig = GameObjectManager::Instance().get_ObjectByTag("PointLight");
    lig.lock()->get_Component<Transform>()->set_Pos(sin(a) * 1000.0f, (cos(a) * 1000.0f) * -1, 0);
    lig.lock()->get_Component<PointLight>()->set_Range(m_PointLightRange);

    auto dlig = GameObjectManager::Instance().get_ObjectByTag("DirLight");
    auto rad = dlig.lock()->get_Component<Transform>();
    rad->set_RotateToRad(m_LightDir);
    dlig.lock()->get_Component<DirectionalLight>()->set_Intensity(intensity);

    auto b_2Obj = GameObjectManager::Instance().get_ObjectByTag("B-2");
    rad = b_2Obj.lock()->get_Component<Transform>();
    rad->set_RotateToRad(0.0, 0.0, sin(a));

    Debugger::Instance().BeginDebugWindow("Light");
    Debugger::Instance().DG_SliderFloat("dir", &m_LightDir, -1.0f, 1.0f);
    Debugger::Instance().DG_SliderFloat("DirLig_Intensity",1, &intensity, 0.0f, 3.0f);
    Debugger::Instance().DG_SliderFloat("PointLig_Range", 1, &m_PointLightRange, 0.0f, 10000.0f);
    Debugger::Instance().EndDebugWindow();;


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


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
#include "Component_SkinnedMeshAnimator.h"
#include "Component_ModelMeshResource.h"
#include "Component_ModelMeshRenderer.h"
#include "Component_MeshRenderer.h"
#include "MeshFactory.h"
#include "Component_DirectionalLight.h"
#include "Component_PointLight.h"
#include "Component_SpriteRenderer.h"
#include "Component_BillboardRenderer.h"
#include "DX_RenderTarget.h"


using namespace VECTOR4;
using namespace VECTOR3;
using namespace VECTOR2;
using namespace Tool::UV;
using namespace Input;

using namespace GIGA_Engine;


/// <summary>
/// ※ 魔導書にあった関数
/// ガウシアン関数を利用して重みテーブルを計算する
/// </summary>
/// <param name="weightsTbl">重みテーブルの記録先</param>
/// <param name="sizeOfWeightsTbl">重みテーブルのサイズ</param>
/// <param name="sigma">分散具合。この数値が大きくなると分散具合が強くなる</param>
void CalcWeightsTableFromGaussian(float* weightsTbl, int sizeOfWeightsTbl, float sigma)
{
    if (sigma <= 0.0f)
    {
        return;
    }

    // 重みの合計を記録する変数を定義する
    float total = 0;

    // ここからガウス関数を用いて重みを計算している
    // ループ変数のxが基準テクセルからの距離
    for (int x = 0; x < sizeOfWeightsTbl; x++)
    {
        weightsTbl[x] = expf(-0.5f * (float)(x * x) / sigma);
        total += 2.0f * weightsTbl[x];
    }

    // 重みの合計で除算することで、重みの合計を1にしている
    for (int i = 0; i < sizeOfWeightsTbl; i++)
    {
        weightsTbl[i] /= total;
    }
}



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
        {
            /* カメラの生成 */
            std::weak_ptr<GameObject> pCam = Instantiate(std::move(std::make_shared<GameObject>()));
            pCam.lock()->Init(renderer);
            pCam.lock()->set_Tag("Camera");
            pCam.lock()->add_Component<Camera3D>();
            pCam.lock()->get_Transform().lock()->set_Pos(0.0f, 800.0f, -1000.0f);
        }

        {
            /* ディレクションライトの生成(Cubuで分かりやすく) */
            MATERIAL *mat = new MATERIAL;
            mat->Diffuse.Texture = ResourceManager::Instance().LoadWIC_Texture(L"Resource/Texture/Wood022_2K-JPG_Color.jpg");
            mat->DiffuseColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);
            mat->SpecularPower = 1.0f;

            CreateUtilityMeshInfo mesh;
            mesh.pRenderer = &renderer;
            mesh.Type = UTILITY_MESH_TYPE::CUBU;
            mesh.ObjTag = "DirLight";
            mesh.MatNum = 1;
            mesh.MaterialData = new InputMaterial();
            mesh.MaterialData->pMat = mat;

            auto obj = MeshFactory::CreateUtilityMesh(mesh);
            auto light = obj.lock()->add_Component<DirectionalLight>();
            light->set_LightColor(VEC3(1.0f, 1.0f, 1.0f));
            light->set_Intensity(0.1f);
            light->Init(renderer);

            obj.lock()->get_Transform().lock()->set_Pos(VEC3(0.0f, 1000.0f, -1000.0f));
            obj.lock()->get_Transform().lock()->set_Scale(VEC3(40, 40, 100));
            obj.lock()->get_Transform().lock()->set_RotateToRad(VEC3(1.0f, -1.0f, 1.0f));
        }

        {
            /* ポイントライトの生成 (Cubuで分かりやすく)*/
            MATERIAL* mat = new MATERIAL;
            mat->Diffuse.Texture = ResourceManager::Instance().LoadWIC_Texture(L"Resource/Texture/Wood022_2K-JPG_Color.jpg");
            mat->DiffuseColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);
            mat->SpecularColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);
            mat->SpecularPower = 1.0f;

            CreateUtilityMeshInfo mesh;
            mesh.pRenderer = &renderer;
            mesh.Type = UTILITY_MESH_TYPE::CUBU;
            mesh.MatNum = 1;
            mesh.MaterialData = new InputMaterial();
            mesh.MaterialData->pMat = mat;
            mesh.IsActive = true;

            for (int i = 0; i < 30; i++)
            {
                VEC3 pt;
                pt.x = static_cast<float>(rand() % 10000) - 5000.0f;
                pt.y = 700.0f;
                pt.z = static_cast<float>(rand() % 10000) - 5000.0f;
                VEC3 col;
                col.x = static_cast<float>(rand() % 255) / 255.0f;
                col.y = static_cast<float>(rand() % 255) / 255.0f;
                col.z = static_cast<float>(rand() % 255) / 255.0f;

                auto obj = MeshFactory::CreateUtilityMesh(mesh);
                obj.lock()->get_Transform().lock()->set_Pos(pt);
                obj.lock()->get_Transform().lock()->set_Scale(VEC3(25, 25, 25));
                obj.lock()->set_Tag("PointLight" + std::to_string(i));
                auto light = obj.lock()->add_Component<PointLight>();
                light->set_LightColor(col);
                light->set_Range(1000.0f);
                light->set_Intensity(10.0f);
                light->Init(renderer);
            }
        }

        {
            /* プレイヤー モデルの生成 */
            MATERIAL mat[1];
            mat[0].Normal.Texture = ResourceManager::Instance().LoadWIC_Texture(L"Resource/Texture/DefaultN_Map.png");
            mat[0].DiffuseColor = VEC4(1.0, 0.0, 1.0, 1.0);
            mat[0].SpecularPower = 30.0f;
            mat[0].SpecularColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);

            CreateModelInfo model;
            model.pRenderer = &renderer;
            model.Path = "Resource/Model/Player2/AL_Standard.fbx";
            model.ObjTag = "Player";
            model.IsAnim = true;
            model.MatNum = 1;
            model.MaterialData = new InputMaterial();
            model.MaterialData->MatIndex = 0;
            model.MaterialData->pMat = mat;
            model.ShaderType = SHADER_TYPE::DEFFERD_STANDARD_SKINNED;
            auto obj = MeshFactory::CreateModel(model);
            obj.lock()->get_Component<Transform>()->set_Scale(1.0f, 1.0f, 1.0f);
            obj.lock()->get_Component<SkinnedMeshAnimator>()->set_IsAnim(true);
            obj.lock()->get_Component<SkinnedMeshAnimator>()->set_AnimIndex(0);
        }

        {
            /* アリ モデルの生成 */
            MATERIAL mat[1];
            mat[0].Diffuse.Texture = ResourceManager::Instance().LoadWIC_Texture(L"Resource/Model/Enemy/trader_ant_lowpoly.fbm/new_bake_ant.png");
            mat[0].Normal.Texture = ResourceManager::Instance().LoadWIC_Texture(L"Resource/Model/Enemy/trader_ant_lowpoly.fbm/new_bake_ant_n.png");
            mat[0].DiffuseColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);
            mat[0].SpecularPower = 50.0f;
            mat[0].SpecularColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);

            CreateModelInfo model;
            model.pRenderer = &renderer;
            model.Path   = "Resource/Model/Enemy/trader_ant_lowpoly.fbx";
            model.ObjTag = "Model";
            model.IsAnim = true;
            model.MatNum = 1;
            model.MaterialData = new InputMaterial();
            model.MaterialData->MatIndex = 0;
            model.MaterialData->pMat = mat;
            model.ShaderType = SHADER_TYPE::DEFFERD_STANDARD_SKINNED;
            auto obj = MeshFactory::CreateModel(model);
            obj.lock()->get_Component<Transform>()->set_Scale(1.0,1.0,1.0);
            obj.lock()->get_Component<Transform>()->set_RotateToDeg(0.0f,180,0.0);
            obj.lock()->get_Component<SkinnedMeshAnimator>()->set_IsAnim(true);
            obj.lock()->get_Component<SkinnedMeshAnimator>()->set_AnimIndex(0);
        }

        {
            /* B-2 モデルの生成 */
            MATERIAL mat[1];
            mat[0].Diffuse.Texture  = ResourceManager::Instance().LoadWIC_Texture(L"Resource/Model/b-2/textures/ggg_diffuseOriginal.jpeg");
            mat[0].Specular.Texture = ResourceManager::Instance().LoadWIC_Texture(L"Resource/Model/b-2/textures/ggg_metallic.jepg");
            mat[0].Normal.Texture   = ResourceManager::Instance().LoadWIC_Texture(L"Resource/Model/b-2/textures/ggg_normal.jpeg");
            mat[0].DiffuseColor     = VEC4(1.0f, 1.0f, 1.0f, 1.0f);
            mat[0].SpecularPower    = 200.0f;
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
            model.ShaderType = SHADER_TYPE::DEFFERD_STANDARD_SKINNED;
            auto obj = MeshFactory::CreateModel(model);
            obj.lock()->get_Component<Transform>()->set_Scale(0.5f, 0.5f, 0.5f);
            obj.lock()->get_Component<Transform>()->set_Pos(0.0f, 500.0f, 0.0f);
            obj.lock()->set_LayerRank(0);
        }

        {
            /* クレイモア モデルの生成 */
            MATERIAL mat[1];
            mat[0].Diffuse.Texture  = ResourceManager::Instance().LoadWIC_Texture(L"Resource/Model/Claymore/Mat_Base_Color.png");
            mat[0].Specular.Texture = ResourceManager::Instance().LoadWIC_Texture(L"Resource/Model/Claymore/Mat_Roughness.png");
            mat[0].Normal.Texture   = ResourceManager::Instance().LoadWIC_Texture(L"Resource/Model/Claymore/Mat_Normal_DirectX.png");
            mat[0].DiffuseColor     = VEC4(1.0f, 1.0f, 1.0f, 1.0f);
            mat[0].SpecularPower    = 100.0f;
            mat[0].SpecularColor    = VEC4(1.0f, 1.0f, 1.0f, 1.0f);

            CreateModelInfo model;
            model.pRenderer = &renderer;
            model.Path = "Resource/Model/Claymore/Claymore.fbx";
            model.ObjTag = "Claymore";
            model.IsAnim = false;
            model.MatNum = 1;
            model.MaterialData = new InputMaterial();
            model.MaterialData->MatIndex = 0;
            model.MaterialData->pMat = mat;
            model.ShaderType = SHADER_TYPE::DEFFERD_STANDARD_SIMPLE;
            auto obj = MeshFactory::CreateModel(model);
            obj.lock()->get_Component<Transform>()->set_Scale(1.0f, 1.0f, 1.0f);
            obj.lock()->get_Component<Transform>()->set_Pos(0.0f, 100.0f, 1000.0f);
            obj.lock()->set_LayerRank(0);
        }        

        {
            /* QUADの生成 */

/*            for (int i = -1; i < 2; i++)
            {
                MATERIAL* mat = new MATERIAL;
                mat->Diffuse.Texture = ResourceManager::Instance().LoadWIC_Texture(L"Resource/Texture/外壁W050.jpg");
                mat->Normal.Texture = ResourceManager::Instance().LoadWIC_Texture(L"Resource/Texture/DefaultN_Map.png");
                mat->SpecularColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);
                mat->SpecularPower = 30.0f;

                CreateUtilityMeshInfo mesh;
                mesh.pRenderer = &renderer;
                mesh.Type = UTILITY_MESH_TYPE::QUAD;
                mesh.ObjTag = "Quad" + std::to_string(i);
                mesh.MatNum = 1;
                mesh.MaterialData = new InputMaterial();
                mesh.MaterialData->pMat = mat;

                auto obj = MeshFactory::CreateUtilityMesh(mesh);
                obj.lock()->get_Transform().lock()->set_Scale(1000.0f, 1.0f, 1000.0f);
                obj.lock()->get_Transform().lock()->set_Pos((i * 1000.0f), 0.0f, 0.0f);
                obj.lock()->get_Transform().lock()->set_RotateToDeg(0.0f,  0.0f, (i * 90.0f));
            }   */ 
            
            {
                // 地面
                MATERIAL* mat = new MATERIAL;
                mat->Diffuse.Texture = ResourceManager::Instance().LoadWIC_Texture(L"Resource/Texture/WoodFloor051_2K-PNG_Color.png");
                mat->Normal.Texture = ResourceManager::Instance().LoadWIC_Texture(L"Resource/Texture/DefaultN_Map.png");
                mat->SpecularColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);
                mat->SpecularPower = 100.0f;

                CreateUtilityMeshInfo mesh;
                mesh.pRenderer = &renderer;
                mesh.Type = UTILITY_MESH_TYPE::PLANE;
                mesh.ObjTag = "Ground";
                mesh.MatNum = 1;
                mesh.MaterialData = new InputMaterial();
                mesh.MaterialData->pMat = mat;

                auto obj = MeshFactory::CreateUtilityMesh(mesh);
                obj.lock()->get_Transform().lock()->set_Scale(10000.0f, 10000.0f, 10000.0f);
                obj.lock()->get_Transform().lock()->set_Pos(0.0f, 0.0f, 0.0f);
                obj.lock()->get_Transform().lock()->set_RotateToDeg(0.0f,  0.0f, 0.0f);
            }
        }
   
        {
            // SkyDorm
            MATERIAL* mat = new MATERIAL;
            mat->Diffuse.Texture = ResourceManager::Instance().LoadDDS_CubeMap_Texture(L"Resource/Texture/cloudy_skybox.dds");
            mat->DiffuseColor = VEC4(3.0f, 3.0f, 3.0f, 1.0f);
            mat->SpecularColor = VEC4(0.0f, 0.0f, 0.0f, 0.0f);
            mat->SpecularPower = 1.0f;

            CreateUtilityMeshInfo mesh;
            mesh.pRenderer = &renderer;
            mesh.Type = UTILITY_MESH_TYPE::CUBU;
            mesh.ObjTag = "SkyDorm";
            mesh.MatNum = 1;
            mesh.MaterialData = new InputMaterial();
            mesh.MaterialData->pMat = mat;
            mesh.ShaderType = SHADER_TYPE::SKYBOX;
            mesh.IsActive = false;

            auto obj = MeshFactory::CreateUtilityMesh(mesh);
            obj.lock()->get_Transform().lock()->set_Scale(1, 1, 1);
            obj.lock()->get_Transform().lock()->set_Pos(0.0, 0.0, 0.0);
        } 

        {
            // ビルボード
            MATERIAL* mat = new MATERIAL;
            mat->Diffuse.Texture = ResourceManager::Instance().LoadWIC_Texture(L"Resource/Texture/0191.png");
            mat->SpecularColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);
            mat->DiffuseColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);
            mat->SpecularPower = 100.0f;
            mat->BlendMode = BLEND_MODE::ALPHA;
            CreateBillboradInfo billboard;
            billboard.pRenderer     = &renderer;
            billboard.Type          = BILLBOARD_USAGE_TYPE::SIMPLE;
            billboard.ShaderType    = SHADER_TYPE::FOWARD_NO_LIGHTING_SIMPLE;
            billboard.ObjTag        = "Billboard";
            billboard.IsActive      = false;
            billboard.MatNum        = 1;
            billboard.MaterialData  = new InputMaterial();
            billboard.MaterialData->pMat = mat;
            auto obj = MeshFactory::CreateBillboard(billboard);
            obj.lock()->get_Transform().lock()->set_Scale(1280.0f, 720.0f, 500.0f);
            obj.lock()->get_Transform().lock()->set_Pos(0.0f, 500.0f, 0.0f);
        }
    }


    // 参照を持たせる
    m_pCamera = Master::m_pGameObjectManager->get_ObjectByTag("Camera");

    // ライトにカメラのTransformを持たせる
    Master::m_pLightManager->set_CameraTransform(m_pCamera.lock()->get_Transform());


    bool result = true;


    //========================================================================================
    //
    //
    /* レンダリングターゲット作成 */
    //
    //
    //========================================================================================
    // ****************************************************************
    // アルベド
    // ****************************************************************
    m_pAlbedo_RT = new DX_RenderTarget();
    result = m_pAlbedo_RT->Create(
        renderer,
        renderer.get_ScreenWidth(),
        renderer.get_ScreenHeight(),
        0,
        1,
        DXGI_FORMAT_R8G8B8A8_UNORM,
        DXGI_FORMAT_UNKNOWN
    );
    if (result == false)return false;

    // ****************************************************************
    // 法線
    // ****************************************************************
    m_pNormal_RT = new DX_RenderTarget();
    result = m_pNormal_RT->Create(
        renderer,
        renderer.get_ScreenWidth(),
        renderer.get_ScreenHeight(),
        1,
        1,
        DXGI_FORMAT_R8G8B8A8_UNORM,
        DXGI_FORMAT_UNKNOWN
    );
    if (result == false)return false;

    // ****************************************************************
    // スペキュラ
    // ****************************************************************
    m_pSpecular_RT = new DX_RenderTarget();
    result = m_pSpecular_RT->Create(
        renderer,
        renderer.get_ScreenWidth(),
        renderer.get_ScreenHeight(),
        1,
        1,
        DXGI_FORMAT_R8G8B8A8_UNORM,
        DXGI_FORMAT_UNKNOWN
    );
    if (result == false)return false;

    // ****************************************************************
    // デプス
    // ****************************************************************
    m_pDepth_RT = new DX_RenderTarget();
    result = m_pDepth_RT->Create(
        renderer,
        renderer.get_ScreenWidth(),
        renderer.get_ScreenHeight(),
        1,
        1,
        DXGI_FORMAT_UNKNOWN,
        DXGI_FORMAT_D32_FLOAT
    );
    if (result == false)return false;

    // ****************************************************************
    // シーン最終合成用
    // ****************************************************************
    m_pSceneFinal_RT = new DX_RenderTarget();
    result = m_pSceneFinal_RT->Create(
        renderer,
        renderer.get_ScreenWidth(),
        renderer.get_ScreenHeight(),
        1,
        1,
        DXGI_FORMAT_R32G32B32A32_FLOAT,
        DXGI_FORMAT_D32_FLOAT
    );
    if (result == false)return false;
    
    // ****************************************************************
    // 水平ブラー
    // ****************************************************************
    m_pHorizontalBlur = new DX_RenderTarget();
    result = m_pHorizontalBlur->Create(
        renderer,
        renderer.get_ScreenWidth() / 2.0f,
        renderer.get_ScreenHeight(),
        1,
        1,
        DXGI_FORMAT_R8G8B8A8_UNORM,
        DXGI_FORMAT_D32_FLOAT
    );
    if (result == false)return false;
    


    // ****************************************************************
    // 垂直ブラー
    // ****************************************************************
    m_pVerticalBlur = new DX_RenderTarget();
    result = m_pVerticalBlur->Create(
        renderer,        
        renderer.get_ScreenWidth() / 2.0f,
        renderer.get_ScreenHeight() / 2.0f,
        1,
        1,
        DXGI_FORMAT_R8G8B8A8_UNORM,
        DXGI_FORMAT_D32_FLOAT
    );
    if (result == false)return false;
    
    // ガウシアンブラー用の重みテーブルを計算する
    CalcWeightsTableFromGaussian(m_weights, NUM_WEIGHTS, 4.0f);
    
    //========================================================================================
    //
    //
    /* レンダリングターゲット用スプライト */
    //
    //
    //========================================================================================
    /*************************************
    * アルベド用
    *************************************/
    CreateSpriteInfo sprite;
    sprite.pRenderer = &renderer;
    sprite.Type = SPRITE_USAGE_TYPE::RENDER_TARGET;
    sprite.ShaderType = SHADER_TYPE::FOWARD_STANDARD_UI_SPRITE;
    sprite.IsActive = false;    // ２重更新されてしまうのでobjマネージャ側では何もしないように


    sprite.ObjTag = "RenderTarget1";
    sprite.Width = 0.5f;
    sprite.Height = 0.5f;
    sprite.pTextureMap[0] = ResourceManager::Instance().Convert_SRVToTexture("RT1", m_pAlbedo_RT->get_SRV_ComPtr());
    auto obj = MeshFactory::CreateSprite(sprite);    
    obj.lock()->get_Transform().lock()->set_Pos(0.5, 0.5, 0.0);
    sprite.pTextureMap.clear();

    /*************************************
    * 法線用
    *************************************/
    sprite.ObjTag = "RenderTarget2";
    sprite.Width = 0.5;
    sprite.Height = 0.5f;
    sprite.pTextureMap[0] = ResourceManager::Instance().Convert_SRVToTexture("RT2", m_pNormal_RT->get_SRV_ComPtr());
    obj = MeshFactory::CreateSprite(sprite);    
    obj.lock()->get_Transform().lock()->set_Pos(-0.5, 0.5, 0.0);
    sprite.pTextureMap.clear();

    /*************************************
    * スペキュラ用
    *************************************/
    sprite.ObjTag = "RenderTarget3";
    sprite.pTextureMap[0] = ResourceManager::Instance().Convert_SRVToTexture("RT3", m_pSpecular_RT->get_SRV_ComPtr());
    obj = MeshFactory::CreateSprite(sprite);    
    obj.lock()->get_Transform().lock()->set_Pos(-0.5, -0.5, 0.0);
    sprite.pTextureMap.clear();

    /*************************************
    * Z値用
    *************************************/
    sprite.ObjTag = "RenderTarget4";
    sprite.pTextureMap[0] = ResourceManager::Instance().Convert_SRVToTexture("RT4", m_pDepth_RT->get_DepthSRV_ComPtr());
    obj = MeshFactory::CreateSprite(sprite);  
    sprite.pTextureMap.clear();

    /*************************************
    * ライティングパス出力用
    *************************************/
    sprite.ObjTag = "DefferdRenderTarget";
    sprite.Width = 1.0f;
    sprite.Height = 1.0f;
    sprite.pTextureMap[0] = ResourceManager::Instance().Convert_SRVToTexture("RT1", m_pAlbedo_RT->get_SRV_ComPtr());
    sprite.pTextureMap[1] = ResourceManager::Instance().Convert_SRVToTexture("RT2", m_pNormal_RT->get_SRV_ComPtr());
    sprite.pTextureMap[2] = ResourceManager::Instance().Convert_SRVToTexture("RT3", m_pSpecular_RT->get_SRV_ComPtr());
    sprite.pTextureMap[3] = ResourceManager::Instance().Convert_SRVToTexture("RT4", m_pDepth_RT->get_DepthSRV_ComPtr());
    sprite.ShaderType = SHADER_TYPE::DEFFERD_STANDARD_RT_SPRITE;
    obj = MeshFactory::CreateSprite(sprite);
    //obj.lock()->get_Transform().lock()->set_Pos(0.5, -0.5, 0.0);
    sprite.pTextureMap.clear();    
                   
    /*************************************
    * 水平ブラー用スプライト
    *************************************/
    CreateSpriteInfo horizontalBlurSprite;
    horizontalBlurSprite.pRenderer = &renderer;
    horizontalBlurSprite.pPSConstantBuffers = new ConstantBufferInfo(); // VS定数バッファにブラー用の重みテーブルをセット
    horizontalBlurSprite.pPSConstantBuffers->SetSlot = 7;               // スロット7にセット
    horizontalBlurSprite.pPSConstantBuffers->pUserExpandConstantBuffer = &m_weights;
    horizontalBlurSprite.pPSConstantBuffers->UserExpandConstantBufferSize = sizeof(m_weights);
    horizontalBlurSprite.PSConstBufferNum = 1;
    horizontalBlurSprite.IsActive = false;
    horizontalBlurSprite.ObjTag = "HorizontalBlurSprite";
    horizontalBlurSprite.Width = 1.0f;      // サイズの変更はRTだけでいい
    horizontalBlurSprite.Height = 1.0f;                                                    
    horizontalBlurSprite.pTextureMap[0] = ResourceManager::Instance().Convert_SRVToTexture("RT_SceneFinal", m_pSceneFinal_RT->get_SRV_ComPtr());
    horizontalBlurSprite.Type = SPRITE_USAGE_TYPE::RENDER_TARGET;
    horizontalBlurSprite.ShaderType = SHADER_TYPE::GAUSSIAN_BLUR_HORIZONTAL;
    obj = MeshFactory::CreateSprite(horizontalBlurSprite);


    /*************************************
    * 垂直ブラー用スプライト
    *************************************/
    CreateSpriteInfo verticalBlurSprite;
    verticalBlurSprite.pRenderer = &renderer;
    verticalBlurSprite.pPSConstantBuffers = new ConstantBufferInfo();   // VS定数バッファにブラー用の重みテーブルをセット
    verticalBlurSprite.pPSConstantBuffers->SetSlot = 7;                 // スロット7にセット
    verticalBlurSprite.pPSConstantBuffers->pUserExpandConstantBuffer = &m_weights;
    verticalBlurSprite.pPSConstantBuffers->UserExpandConstantBufferSize = sizeof(m_weights);
    verticalBlurSprite.PSConstBufferNum = 1;
    verticalBlurSprite.IsActive = false;
    verticalBlurSprite.ObjTag = "VerticalBlurSprite";
    verticalBlurSprite.Width = 1.0f;
    verticalBlurSprite.Height = 1.0f;
    verticalBlurSprite.pTextureMap[0] = ResourceManager::Instance().Convert_SRVToTexture("RT_HorizontalBlur", m_pHorizontalBlur->get_SRV_ComPtr());
    verticalBlurSprite.Type = SPRITE_USAGE_TYPE::RENDER_TARGET;
    verticalBlurSprite.ShaderType = SHADER_TYPE::GAUSSIAN_BLUR_VERTICAL;
    obj = MeshFactory::CreateSprite(verticalBlurSprite);

    /*************************************
    * 最終的に合成してフレームバッファにコピーする用のスプライト
    *************************************/
    CreateSpriteInfo copyToFrameBufferSprite;
    copyToFrameBufferSprite.pRenderer = &renderer;
    copyToFrameBufferSprite.IsActive = false;
    copyToFrameBufferSprite.ObjTag = "CopyToFrameBufferSprite";
    copyToFrameBufferSprite.Width = 1.0f;
    copyToFrameBufferSprite.Height = 1.0f;
    copyToFrameBufferSprite.pTextureMap[0] = ResourceManager::Instance().Convert_SRVToTexture("RT_VerticalBlur", m_pVerticalBlur->get_SRV_ComPtr());
    copyToFrameBufferSprite.Type = SPRITE_USAGE_TYPE::RENDER_TARGET;
    copyToFrameBufferSprite.ShaderType = SHADER_TYPE::FOWARD_STANDARD_UI_SPRITE;
    obj = MeshFactory::CreateSprite(copyToFrameBufferSprite);


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


    auto obj = Master::m_pGameObjectManager->get_ObjectByTag("Model");
    obj.lock()->get_Component<Transform>()->set_Pos(0, 0, sin(a) * 1000.0f);



    auto cam = Master::m_pGameObjectManager->get_ObjectByTag("Camera");
    VEC3 camPos = cam.lock()->get_Component<Transform>()->get_VEC3ToPos();


    static VEC3 pLigPos{};

    auto lig = Master::m_pGameObjectManager->get_ObjectByTag("PointLight0");
    //lig.lock()->get_Component<Transform>()->set_Pos((cos(a) * 1000.0f) * -1, 200.0f, 0.0f);
    //lig.lock()->get_Component<Transform>()->set_Pos(pLigPos);
    //lig.lock()->get_Component<PointLight>()->set_Range(m_PointLightRange);
    //lig.lock()->get_Component<PointLight>()->set_Intensity(10.0);

    auto dlig = Master::m_pGameObjectManager->get_ObjectByTag("DirLight");
    auto rad = dlig.lock()->get_Component<Transform>();
    rad->set_RotateToRad(m_LightDir);
    dlig.lock()->get_Component<DirectionalLight>()->set_Intensity(intensity);

    auto b_2Obj = Master::m_pGameObjectManager->get_ObjectByTag("B-2");
    rad = b_2Obj.lock()->get_Component<class Transform>();
    rad->set_RotateToRad(0.0, 0.0, sin(a));



    //auto sphereObj = Master::m_pGameObjectManager->get_ObjectByTag("sphere");
    //rad = sphereObj.lock()->get_Component<Transform>();
    //rad->set_RotateToRad(0.0, 0.0, 0.0f);



    //auto cubuObj = GameObjectManager::Instance().get_ObjectByTag("Cubu");
    //rad = cubuObj.lock()->get_Component<Transform>();
    //rad->set_RotateToRad(cos(a), sin(a), sin(a));

    auto skyObj = Master::m_pGameObjectManager->get_ObjectByTag("SkyDorm");
    auto tf = skyObj.lock()->get_Component<Transform>();
    tf->set_Pos(camPos);

    // ライトのデバッグ
    Master::m_pDebugger->BeginDebugWindow("Light");
    Master::m_pDebugger->DG_DragVec3("dir", &m_LightDir, 0.005f, -1.0f, 1.0f);
    Master::m_pDebugger->DG_SliderFloat("DirLig_Intensity",1, &intensity, 0.0f, 100.0f);
    Master::m_pDebugger->DG_SliderFloat("PointLig_Range", 1, &m_PointLightRange, 0.0f, 10000.0f);
    Master::m_pDebugger->DG_DragVec3("PointLig_Pos", &pLigPos, 1.0f, -10000.0f, 10000.0f);
    Master::m_pDebugger->EndDebugWindow();

    auto objList = Master::m_pGameObjectManager->get_ObjectList();

    // オブジェクトデバッグ
    Master::m_pDebugger->BeginDebugWindow("GameObject");
    Master::m_pDebugger->DG_TextValue("Num : %d", (int)objList.size());
    for (auto& obj : objList)
    {
        Master::m_pDebugger->DG_TextValue("name : %s", obj->get_Tag().c_str());
    }
    Master::m_pDebugger->EndDebugWindow();

    // オブジェクト更新
    Master::m_pGameObjectManager->ObjectUpdate(renderer);

    // カメラ更新
    auto viewMatrix = m_pCamera.lock()->get_Component<Camera3D>()->get_ViewMatrix();

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
    // ポストエフェクトデバッグ
    {
        static float blurIntensity = 1.0f;
        // ブラー強度
        Master::m_pDebugger->BeginDebugWindow("PostEffect");
        Master::m_pDebugger->DG_SliderFloat("BlurIntencity", 1, &blurIntensity, 0.01f, 100.0f);
        Master::m_pDebugger->DG_TextValue("weight0:%f.2", m_weights[0]);
        Master::m_pDebugger->DG_TextValue("weight1:%f.2", m_weights[1]);
        Master::m_pDebugger->DG_TextValue("weight2:%f.2", m_weights[2]);
        Master::m_pDebugger->DG_TextValue("weight3:%f.2", m_weights[3]);
        Master::m_pDebugger->DG_TextValue("weight4:%f.2", m_weights[4]);
        Master::m_pDebugger->DG_TextValue("weight5:%f.2", m_weights[5]);
        Master::m_pDebugger->DG_TextValue("weight6:%f.2", m_weights[6]);
        Master::m_pDebugger->DG_TextValue("weight7:%f.2", m_weights[7]);
        Master::m_pDebugger->EndDebugWindow();

        // ガウシアンブラー用の重みテーブルを計算する
        CalcWeightsTableFromGaussian(m_weights, NUM_WEIGHTS, blurIntensity);
    }
    static float t = 0.f;
    t += 0.1f;

    // Gバッファ
    DX_RenderTarget* gbuffer[] = {
        m_pAlbedo_RT ,
        m_pNormal_RT,
        m_pSpecular_RT,
        m_pDepth_RT
    };

    // プロジェクション変換行列の設定
    renderer.SetupProjectionTransform(1920,1080);

    // ************************************************************************
    // 
    // Gパス
    // 
    // ************************************************************************
    // ビューポートの設定
    renderer.set_ViewPort(0, 0, renderer.get_ScreenWidth(), renderer.get_ScreenHeight());

    // レンダリングターゲットの設定とクリア
    renderer.RegisterRenderTargets(ARRAYSIZE(gbuffer), gbuffer);
    renderer.ClearRenderTargetViews(ARRAYSIZE(gbuffer), gbuffer);

    // オブジェクト描画
    Master::m_pGameObjectManager->ObjectRender(renderer);

    // レンダリングターゲット解除
    renderer.ReleaseRenderTargetSetNull();


    // ************************************************************************
    // 
    // ライティングパス
    //
    // ************************************************************************
    // 最終合成用レンダリングターゲットに変更
    renderer.RegisterRenderTarget(m_pSceneFinal_RT->get_RTV(), m_pSceneFinal_RT->get_DSV());
    renderer.ClearRenderTargetView(m_pSceneFinal_RT);

    {
        // ターゲット描画
        //auto renderSpriteObj = Master::m_pGameObjectManager->get_ObjectByTag("RenderTarget1").lock();
        //auto renderSpriteObj2 =Master::m_pGameObjectManager->get_ObjectByTag("RenderTarget2").lock();
        //auto renderSpriteObj3 =Master::m_pGameObjectManager->get_ObjectByTag("RenderTarget3").lock();
        //auto renderSpriteObj4 =Master::m_pGameObjectManager->get_ObjectByTag("RenderTarget4").lock();
        //auto sprite = renderSpriteObj->get_Component<SpriteRenderer>();
        //auto sprite2 = renderSpriteObj2->get_Component<SpriteRenderer>();
        //auto sprite3 = renderSpriteObj3->get_Component<SpriteRenderer>();
        //auto sprite4 = renderSpriteObj4->get_Component<SpriteRenderer>();
        //sprite->Draw(renderer);
        //sprite2->Draw(renderer);
        //sprite3->Draw(renderer);
        //sprite4->Draw(renderer);
    }

    // ライトの更新
    Master::m_pLightManager->Update();

    // ディファードスプライト
    auto defferdRTSpriteObj = Master::m_pGameObjectManager->get_ObjectByTag("DefferdRenderTarget").lock();
    auto defferd = defferdRTSpriteObj->get_Component<SpriteRenderer>();
    Master::m_pBlendManager->DeviceToSetBlendState(BLEND_MODE::NONE);
    defferd->Draw(renderer);

    // レンダリングターゲット解除
    renderer.ReleaseRenderTargetSetNull();


    // ************************************************************************
    // 
    // ブラー掛け
    // 
    // ************************************************************************

    // ビューポートの設定
    renderer.set_ViewPort(0, 0, renderer.get_ScreenWidth() / 2.0f, renderer.get_ScreenHeight());

    // 水平ブラー用レンダリングターゲットに変更
    renderer.RegisterRenderTarget(m_pHorizontalBlur->get_RTV(), nullptr);
    renderer.ClearRenderTargetView(m_pHorizontalBlur);
    // 水平ブラー
    auto horizontalBlurSpriteObj = Master::m_pGameObjectManager->get_ObjectByTag("HorizontalBlurSprite").lock();
    auto horizontalBlurSprite = horizontalBlurSpriteObj->get_Component<SpriteRenderer>();
    horizontalBlurSprite->setToGPU_ExtendUserPS_CBuffer(renderer, 0, &m_weights);
    horizontalBlurSprite->Draw(renderer);
    // レンダリングターゲット解除
    renderer.ReleaseRenderTargetSetNull();

    // ビューポートの設定
    renderer.set_ViewPort(0, 0, renderer.get_ScreenWidth() / 2.0f, renderer.get_ScreenHeight() / 2.0f);

    // 垂直ブラー用レンダリングターゲットに変更
    renderer.RegisterRenderTarget(m_pVerticalBlur->get_RTV(), nullptr);
    renderer.ClearRenderTargetView(m_pVerticalBlur);

    // 垂直ブラー
    auto verticalBlurSpriteObj = Master::m_pGameObjectManager->get_ObjectByTag("VerticalBlurSprite").lock();
    auto verticalBlurSprite = verticalBlurSpriteObj->get_Component<SpriteRenderer>();
    verticalBlurSprite->setToGPU_ExtendUserPS_CBuffer(renderer, 0, &m_weights);
    verticalBlurSprite->Draw(renderer);
    // レンダリングターゲット解除
    renderer.ReleaseRenderTargetSetNull();

    // ************************************************************************
    // 
    // フレームバッファ
    // 
    // ************************************************************************
    // レンダリングターゲットをフレームバッファに変更
    renderer.ChangeRenderTargetFrameBuffer();

    // ビューポートの設定
    renderer.set_ViewPort(0, 0, renderer.get_ScreenWidth(), renderer.get_ScreenHeight());

    auto copyToFrameBufferSpriteObj = Master::m_pGameObjectManager->get_ObjectByTag("CopyToFrameBufferSprite").lock();
    auto copyToFrameBufferSprite = copyToFrameBufferSpriteObj->get_Component<SpriteRenderer>();
    copyToFrameBufferSprite->Draw(renderer);


    // ************************************************************************
    // 
    // フォワードの場合はこの下に記述（ポストエフェクトはかからないよ）
    // 
    // ************************************************************************
    // Gbuffer作成時の深度バッファを設定
    renderer.ChangeRenderTargetFrameBuffer(m_pDepth_RT->get_DSV());
    
    renderer.TestSet_DepthStencilState();

    auto skyDorm = Master::m_pGameObjectManager->get_ObjectByTag("SkyDorm").lock();
    skyDorm->get_Component<MeshRenderer>()->Draw(renderer);   

    auto billboard = Master::m_pGameObjectManager->get_ObjectByTag("Billboard").lock();
    //billboard->get_Component<BillboardRenderer>()->Draw(renderer);
    renderer.ClearRenderTargetView(m_pDepth_RT);


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


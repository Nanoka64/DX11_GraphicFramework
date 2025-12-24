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
#include "Component_SkyRenderer.h"
#include "DX_RenderTarget.h"
#include "GaussianBlur.h"


using namespace VECTOR4;
using namespace VECTOR3;
using namespace VECTOR2;
using namespace Tool::UV;
using namespace Input;

using namespace GIGA_Engine;



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
            MATERIAL* mat = new MATERIAL;
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
            mat->Diffuse.Texture = ResourceManager::Instance().LoadWIC_Texture(L"Resource/Texture/外壁W040.jpg");
            mat->Normal.Texture = ResourceManager::Instance().LoadWIC_Texture(L"Resource/Texture/外壁W040_n.png");
            mat->DiffuseColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);
            mat->SpecularColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);
            mat->SpecularPower = 100.0f;

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
                light->set_Intensity(20.0f);
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
            model.ShaderType = SHADER_TYPE::DEFERRED_STD_SKINNED_N;
            auto obj = MeshFactory::CreateModel(model);
            obj.lock()->get_Component<Transform>()->set_Scale(10.0f, 10.0f, 10.0f);
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
            model.Path = "Resource/Model/Enemy/trader_ant_lowpoly.fbx";
            model.ObjTag = "Model";
            model.IsAnim = true;
            model.MatNum = 1;
            model.MaterialData = new InputMaterial();
            model.MaterialData->MatIndex = 0;
            model.MaterialData->pMat = mat;
            model.ShaderType = SHADER_TYPE::DEFERRED_STD_SKINNED_N;
            auto obj = MeshFactory::CreateModel(model);
            obj.lock()->get_Component<Transform>()->set_Scale(1.0, 1.0, 1.0);
            obj.lock()->get_Component<Transform>()->set_RotateToDeg(0.0f, 180, 0.0);
            obj.lock()->get_Component<SkinnedMeshAnimator>()->set_IsAnim(true);
            obj.lock()->get_Component<SkinnedMeshAnimator>()->set_AnimIndex(0);
        }

        {
            /* B-2 モデルの生成 */
            MATERIAL mat[1];
            mat[0].Diffuse.Texture = ResourceManager::Instance().LoadWIC_Texture(L"Resource/Model/b-2/textures/ggg_diffuseOriginal.jpeg");
            mat[0].Specular.Texture = ResourceManager::Instance().LoadWIC_Texture(L"Resource/Model/b-2/textures/ggg_metallic.jepg");
            mat[0].Normal.Texture = ResourceManager::Instance().LoadWIC_Texture(L"Resource/Model/b-2/textures/ggg_normal.jpeg");
            mat[0].DiffuseColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);
            mat[0].SpecularPower = 150.0f;
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
            model.ShaderType = SHADER_TYPE::DEFERRED_STD_SKINNED_N;
            auto obj = MeshFactory::CreateModel(model);
            obj.lock()->get_Component<Transform>()->set_Scale(0.5f, 0.5f, 0.5f);
            obj.lock()->get_Component<Transform>()->set_Pos(0.0f, 500.0f, 0.0f);
            obj.lock()->set_LayerRank(0);
        }

        {
            /* クレイモア モデルの生成 */
            MATERIAL mat[1];
            mat[0].Diffuse.Texture = ResourceManager::Instance().LoadWIC_Texture(L"Resource/Model/Claymore/Mat_Base_Color.png");
            mat[0].Specular.Texture = ResourceManager::Instance().LoadWIC_Texture(L"Resource/Model/Claymore/Mat_Roughness.png");
            mat[0].Normal.Texture = ResourceManager::Instance().LoadWIC_Texture(L"Resource/Model/Claymore/Mat_Normal_DirectX.png");
            mat[0].DiffuseColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);
            mat[0].SpecularPower = 100.0f;
            mat[0].SpecularColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);

            CreateModelInfo model;
            model.pRenderer = &renderer;
            model.Path = "Resource/Model/Claymore/Claymore.fbx";
            model.ObjTag = "Claymore";
            model.IsAnim = false;
            model.MatNum = 1;
            model.MaterialData = new InputMaterial();
            model.MaterialData->MatIndex = 0;
            model.MaterialData->pMat = mat;
            model.ShaderType = SHADER_TYPE::DEFERRED_STD_STATIC;
            auto obj = MeshFactory::CreateModel(model);
            obj.lock()->get_Component<Transform>()->set_Scale(1.0f, 1.0f, 1.0f);
            obj.lock()->get_Component<Transform>()->set_Pos(0.0f, 100.0f, 1000.0f);
            obj.lock()->set_LayerRank(0);
        }

        {
            // 地面
            MATERIAL* mat = new MATERIAL;
            mat->Diffuse.Texture = ResourceManager::Instance().LoadWIC_Texture(L"Resource/Texture/aerial_grass_rock_diff_4k.png");
            mat->Normal.Texture = ResourceManager::Instance().LoadWIC_Texture(L"Resource/Texture/aerial_grass_rock_nor_dx_4k.png");
            mat->SpecularColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);
            mat->SpecularPower = 100.0f;

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
            obj.lock()->get_Transform().lock()->set_Scale(20000.0f, 20000.0f, 20000.0f);
            obj.lock()->get_Transform().lock()->set_Pos(0.0f, 0.0f, 0.0f);
            obj.lock()->get_Transform().lock()->set_RotateToDeg(0.0f, 0.0f, 0.0f);
        }

        {
            // スカイボックスの作成
            MATERIAL* mat = new MATERIAL;
            mat->Diffuse.Texture = ResourceManager::Instance().LoadDDS_CubeMap_Texture(L"Resource/Texture/cloudy_skybox.dds");
            CreateSkyboxInfo skyInfo;
            skyInfo.pRenderer = &renderer;
            skyInfo.ObjTag = "Skybox";
            skyInfo.MatNum = 1;
            skyInfo.MaterialData = new InputMaterial();
            skyInfo.MaterialData->pMat = mat;
            skyInfo.ShaderType = SHADER_TYPE::POST_SKYBOX;
            skyInfo.IsActive = false;

            auto obj = MeshFactory::CreateSkybox(skyInfo);
            obj.lock()->get_Transform().lock()->set_Scale(1.0f, 1.0f, 1.0f);
            obj.lock()->get_Transform().lock()->set_Pos(0.0f, 0.0f, 0.0f);
        }

        {
            // ビルボード
            MATERIAL* mat = new MATERIAL;
            mat->Diffuse.Texture = ResourceManager::Instance().LoadWIC_Texture(L"Resource/Texture/Weak_1024.png");
            mat->SpecularColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);
            mat->SpecularPower = 100.0f;
            mat->BlendMode = BLEND_MODE::ALPHA;
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
                pos.x = static_cast<float>(rand() % 8000) - 4000.0f;
                pos.y = static_cast<float>(rand() % 5000) - 2500.0f;;
                pos.z = static_cast<float>(rand() % 8000) - 4000.0f;

                VEC3 scl;
                scl.x = static_cast<float>(rand() % 50) - 25.0f;
                scl.y = static_cast<float>(rand() % 50) - 25.0f;;
                scl.z = static_cast<float>(rand() % 50) - 25.0f;

                VEC3 col;
                col.x = static_cast<float>(rand() % 255) / 255.0f;
                col.y = static_cast<float>(rand() % 255) / 255.0f;
                col.z = static_cast<float>(rand() % 255) / 255.0f;

                mat->DiffuseColor = VEC4(0.5, 0.5, 0.5, 1.0f);
                billboard.MaterialData->pMat = mat;
                auto obj = MeshFactory::CreateBillboard(billboard);
                obj.lock()->get_Transform().lock()->set_Pos(pos);
                obj.lock()->get_Transform().lock()->set_Scale(2500, 2500, 2000);
                obj.lock()->set_Tag("Billboard" + std::to_string(i));
            }
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
    // 輝度抽出用
    // ****************************************************************
    m_pLuminance_RT = new DX_RenderTarget();
    result = m_pLuminance_RT->Create(
        renderer,
        renderer.get_ScreenWidth(),     // シーンと同じに
        renderer.get_ScreenHeight(),
        1,
        1,
        DXGI_FORMAT_R32G32B32A32_FLOAT,
        DXGI_FORMAT_D32_FLOAT
    );
    if (result == false)return false;

    
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
    sprite.ShaderType = SHADER_TYPE::FORWARD_UNLIT_UI_SPRITE;
    sprite.IsActive = false;    // ２重更新されてしまうのでobjマネージャ側では何もしないように


    sprite.ObjTag = "RenderTarget1";
    sprite.Width = 0.5f;
    sprite.Height = 0.5f;
    sprite.pTextureMap[0] = ResourceManager::Instance().Convert_SRVToTexture("RT1", m_pAlbedo_RT->get_SRV_ComPtr(), m_pAlbedo_RT->get_Width(),m_pAlbedo_RT->get_Height());
    auto obj = MeshFactory::CreateSprite(sprite);    
    obj.lock()->get_Transform().lock()->set_Pos(0.5, 0.5, 0.0);
    sprite.pTextureMap.clear();

    /*************************************
    * 法線用
    *************************************/
    sprite.ObjTag = "RenderTarget2";
    sprite.Width = 0.5;
    sprite.Height = 0.5f;
    sprite.pTextureMap[0] = ResourceManager::Instance().Convert_SRVToTexture("RT2", m_pNormal_RT->get_SRV_ComPtr(), m_pNormal_RT->get_Width(), m_pNormal_RT->get_Height());
    obj = MeshFactory::CreateSprite(sprite);    
    obj.lock()->get_Transform().lock()->set_Pos(-0.5, 0.5, 0.0);
    sprite.pTextureMap.clear();

    /*************************************
    * スペキュラ用
    *************************************/
    sprite.ObjTag = "RenderTarget3";
    sprite.pTextureMap[0] = ResourceManager::Instance().Convert_SRVToTexture("RT3", m_pSpecular_RT->get_SRV_ComPtr(), m_pSpecular_RT->get_Width(), m_pSpecular_RT->get_Height());
    obj = MeshFactory::CreateSprite(sprite);    
    obj.lock()->get_Transform().lock()->set_Pos(-0.5, -0.5, 0.0);
    sprite.pTextureMap.clear();

    /*************************************
    * Z値用
    *************************************/
    sprite.ObjTag = "RenderTarget4";
    sprite.pTextureMap[0] = ResourceManager::Instance().Convert_SRVToTexture("RT4", m_pDepth_RT->get_DepthSRV_ComPtr(), m_pDepth_RT->get_Width(), m_pDepth_RT->get_Height());
    obj = MeshFactory::CreateSprite(sprite);  
    sprite.pTextureMap.clear();

    /*************************************
    * ライティングパス出力用
    *************************************/
    sprite.ObjTag = "DefferdRenderTarget";
    sprite.Width = 1.0f;
    sprite.Height = 1.0f;
    sprite.pTextureMap[0] = ResourceManager::Instance().Convert_SRVToTexture("RT1");
    sprite.pTextureMap[1] = ResourceManager::Instance().Convert_SRVToTexture("RT2");
    sprite.pTextureMap[2] = ResourceManager::Instance().Convert_SRVToTexture("RT3");
    sprite.pTextureMap[3] = ResourceManager::Instance().Convert_SRVToTexture("RT4");
    sprite.ShaderType = SHADER_TYPE::DEFERRED_STD_RT_SPRITE;
    obj = MeshFactory::CreateSprite(sprite);
    //obj.lock()->get_Transform().lock()->set_Pos(0.5, -0.5, 0.0);
    sprite.pTextureMap.clear();    
                   
    /*************************************
    * 輝度抽出用スプライト
    *************************************/
    CreateSpriteInfo luminanceSprite;
    luminanceSprite.pRenderer      = &renderer;
    luminanceSprite.IsActive       = false;
    luminanceSprite.ObjTag         = "LuminanceSprite";
    luminanceSprite.Width          = 1.0f;
    luminanceSprite.Height         = 1.0f;
    luminanceSprite.pTextureMap[0] = ResourceManager::Instance().Convert_SRVToTexture(
        "RT_SceneFinal", 
        m_pSceneFinal_RT->get_SRV_ComPtr(), 
        m_pSceneFinal_RT->get_Width(), 
        m_pSceneFinal_RT->get_Height()
    );
    luminanceSprite.Type           = SPRITE_USAGE_TYPE::RENDER_TARGET;
    luminanceSprite.ShaderType     = SHADER_TYPE::POST_LUMINANCE_FILTER;
    obj = MeshFactory::CreateSprite(luminanceSprite);

    
    /*************************************************************************
    * 輝度抽出テクスチャにガウスブラーを掛ける
    *************************************************************************/
    m_pGaussianBlur = new GaussianBlur();
    m_pGaussianBlur->Setup(
        renderer, 
        ResourceManager::Instance().Convert_SRVToTexture(
            "RT_Luminance",
            m_pLuminance_RT->get_SRV_ComPtr(), 
            m_pLuminance_RT->get_Width(), 
            m_pLuminance_RT->get_Height())
    );

    /*************************************************************************
    * 最終合成用スプライト
    *************************************************************************/
    CreateSpriteInfo finalSprite;
    finalSprite.pRenderer = &renderer;
    finalSprite.IsActive = false;
    finalSprite.ObjTag = "FinalSprite";
    finalSprite.Width = 1.0f;
    finalSprite.Height = 1.0f;
    finalSprite.pTextureMap[0] = ResourceManager::Instance().Convert_SRVToTexture(
        "AfterBlur",
        m_pGaussianBlur->get_AfterBlurTexture(),
        m_pLuminance_RT->get_Width(),
        m_pLuminance_RT->get_Height()
    );
    finalSprite.Type = SPRITE_USAGE_TYPE::RENDER_TARGET;
    finalSprite.ShaderType = SHADER_TYPE::FORWARD_UNLIT_UI_SPRITE;
    MeshFactory::CreateSprite(finalSprite);


    /*************************************************************************
    * フレームバッファにコピーする用のスプライト
    *************************************************************************/
    CreateSpriteInfo copyToFrameBufferSprite;
    copyToFrameBufferSprite.pRenderer = &renderer;
    copyToFrameBufferSprite.IsActive = false;
    copyToFrameBufferSprite.ObjTag = "CopyToFrameBufferSprite";
    copyToFrameBufferSprite.Width = 1.0f;
    copyToFrameBufferSprite.Height = 1.0f;
    copyToFrameBufferSprite.pTextureMap[0] = ResourceManager::Instance().Convert_SRVToTexture("RT_SceneFinal");
    copyToFrameBufferSprite.Type = SPRITE_USAGE_TYPE::RENDER_TARGET;
    copyToFrameBufferSprite.ShaderType = SHADER_TYPE::FORWARD_UNLIT_UI_SPRITE;
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
    //rad->set_RotateToRad(0.0, 0.0, sin(a));



    //auto sphereObj = Master::m_pGameObjectManager->get_ObjectByTag("sphere");
    //rad = sphereObj.lock()->get_Component<Transform>();
    //rad->set_RotateToRad(0.0, 0.0, 0.0f);



    //auto cubuObj = GameObjectManager::Instance().get_ObjectByTag("Cubu");
    //rad = cubuObj.lock()->get_Component<Transform>();
    //rad->set_RotateToRad(cos(a), sin(a), sin(a));

    // ライトのデバッグ
    Master::m_pDebugger->BeginDebugWindow("Light");
    Master::m_pDebugger->DG_DragVec3("dir", &m_LightDir, 0.005f, -3.0f, 3.0f);
    Master::m_pDebugger->DG_SliderFloat("DirLig_Intensity",1, &intensity, 0.0f, 100.0f);
    Master::m_pDebugger->DG_SliderFloat("PointLig_Range", 1, &m_PointLightRange, 0.0f, 10000.0f);
    Master::m_pDebugger->DG_DragVec3("PointLig_Pos", &pLigPos, 1.0f, -10000.0f, 10000.0f);
    Master::m_pDebugger->EndDebugWindow();

    auto objList = Master::m_pGameObjectManager->get_ObjectList();

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
    static float blurIntensity = 1.0f;
    // ポストエフェクトデバッグ
    {
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
    }

    {
        Master::m_pDebugger->BeginDebugWindow("RenderTarget");
        
        Master::m_pDebugger->DG_BulletText("Albed");
        Master::m_pDebugger->DG_Image(m_pAlbedo_RT->get_SRV(), VEC2(400, 200));
        Master::m_pDebugger->DG_Separator();
        
        Master::m_pDebugger->DG_BulletText("Normal");
        Master::m_pDebugger->DG_Image(m_pNormal_RT->get_SRV(), VEC2(400, 200));       
        Master::m_pDebugger->DG_Separator();
        
        Master::m_pDebugger->DG_BulletText("Specular");
        Master::m_pDebugger->DG_Image(m_pSpecular_RT->get_SRV(), VEC2(400, 200));  
        Master::m_pDebugger->DG_Separator();

        Master::m_pDebugger->DG_BulletText("Luminance");
        Master::m_pDebugger->DG_Image(m_pLuminance_RT->get_SRV(), VEC2(400, 200));
        Master::m_pDebugger->DG_Separator();

        Master::m_pDebugger->DG_BulletText("Final");
        Master::m_pDebugger->DG_Image(m_pSceneFinal_RT->get_SRV(), VEC2(400, 200));
        Master::m_pDebugger->DG_Separator();

        Master::m_pDebugger->EndDebugWindow();
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
    // 輝度抽出
    //
    // ************************************************************************
    // 輝度抽出用レンダリングターゲットに変更
    renderer.RegisterRenderTarget(m_pLuminance_RT->get_RTV(), m_pLuminance_RT->get_DSV());
    renderer.ClearRenderTargetView(m_pLuminance_RT);

    // 輝度スプライト
    auto luminanceSpriteObj = Master::m_pGameObjectManager->get_ObjectByTag("LuminanceSprite").lock();
    auto luminance = luminanceSpriteObj->get_Component<SpriteRenderer>();
    luminance->Draw(renderer);

    // レンダリングターゲット解除
    renderer.ReleaseRenderTargetSetNull();


    // ************************************************************************
    // 
    // 取り出した輝度にブラーを掛けて、ブルーム効果にする
    //
    // ************************************************************************
    m_pGaussianBlur->ExcuteOnGPU(renderer, blurIntensity);

    // 加算モード
    Master::m_pBlendManager->DeviceToSetBlendState(BLEND_MODE::ADD);

    // 最終合成用レンダリングターゲットに変更
    // ビューポートの設定
    renderer.set_ViewPort(0, 0, renderer.get_ScreenWidth(), renderer.get_ScreenHeight());
    renderer.RegisterRenderTarget(m_pSceneFinal_RT->get_RTV(), nullptr);    // 深度テストなし

    // ブラー合成スプライト
    auto finalSpriteObj = Master::m_pGameObjectManager->get_ObjectByTag("FinalSprite").lock();
    auto finalSprite = finalSpriteObj->get_Component<SpriteRenderer>();
    finalSprite->Draw(renderer);

    // レンダリングターゲット解除
    renderer.ReleaseRenderTargetSetNull();

    Master::m_pBlendManager->DeviceToSetBlendState(BLEND_MODE::NONE);

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

    // スカイボックス用のデプスステンシル登録
    renderer.RegisterDepthStencilState(renderer.get_DepthTestDisabled_DSS(), 0);

    for (int i = 0; i < 30; i++)
    {
        auto billboard = Master::m_pGameObjectManager->get_ObjectByTag("Billboard" + std::to_string(i)).lock();
        billboard->get_Component<BillboardRenderer>()->Draw(renderer);
    }
    
    auto skybox = Master::m_pGameObjectManager->get_ObjectByTag("Skybox").lock();
    skybox->get_Component<SkyRenderer>()->Draw(renderer);
    
    renderer.ClearRenderTargetView(m_pDepth_RT);

    // デプスステンシル解除
    renderer.RegisterDepthStencilState(NULL, 0);

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


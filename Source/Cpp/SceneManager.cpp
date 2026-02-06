#include "pch.h"
#include "SceneManager.h"
#include "SceneStateEnums.h"
#include "RendererEngine.h"
#include "DirectWriteManager.h"
#include "ResourceManager.h"
#include "GameObjectManager.h"
#include "InputFactory.h"
#include "SceneFactory.h"
#include "MeshFactory.h"
#include "Component_3DCamera.h"
#include "Component_PlayerController.h"
#include "Component_SkinnedMeshAnimator.h"
#include "Component_ModelMeshResource.h"
#include "Component_ModelMeshRenderer.h"
#include "Component_MeshRenderer.h"
#include "Component_DirectionalLight.h"
#include "Component_PointLight.h"
#include "Component_SpriteRenderer.h"
#include "Component_BillboardRenderer.h"
#include "Component_SkyRenderer.h"
#include "Component_AssultRifle.h"
#include "Component_BoxCollider.h"
#include "Component_SphereCollider.h"
#include "Component_TrailRenderer.h"
#include "Component_DecalRenderer.h"


using namespace VECTOR4;
using namespace VECTOR3;
using namespace VECTOR2;
using namespace Tool::UV;
using namespace Tool;
using namespace Input;

using namespace GIGA_Engine;

using namespace SceneStateEnums;

//*---------------------------------------------------------------------------------------
//* @:SceneManager Class 
//*【?】コンストラクタ
//* 引数：なし
//*----------------------------------------------------------------------------------------
SceneManager::SceneManager():
    m_pPlayer(),
    m_StateMachine(this),
    m_CrntSceneState(0),
    m_IsClose(false)
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
 
    // オブジェクトの生成
    {

        ///* 兵士 */
        //{
        //    // マテリアル取得
        //    auto matPtr1 = Master::m_pResourceManager->FindMaterial("Soldier_body");
        //    auto matPtr2 = Master::m_pResourceManager->FindMaterial("Soldier_head");

        //    SetupMaterialInfo matInfo[3];
        //    matInfo[0].Index = 0;   
        //    matInfo[0].pMaterialData = matPtr1; // 体

        //    matInfo[1].Index = 1;
        //    matInfo[1].pMaterialData = matPtr1; // ヘルメット（体）

        //    matInfo[2].Index = 2;
        //    matInfo[2].pMaterialData = matPtr2; // 頭

        //    CreateModelInfo model;
        //    model.pRenderer = &renderer;
        //    model.Path = "Resource/Model/Player/chara.fbx";
        //    model.ObjTag = "Soldier";
        //    model.IsAnim = true;
        //    model.MatNum = 3;
        //    model.SetupMaterial = matInfo;
        //    model.ShaderType = SHADER_TYPE::DEFERRED_STD_SKINNED_N;
        //    auto obj = MeshFactory::CreateModel(model);
        //    obj->get_Component<SkinnedMeshAnimator>()->set_AnimIndex(-1);
        //}

        //std::shared_ptr<GameObject> sphireObj;

        ///* スフィア */
        //{
        //    // マテリアル取得
        //    auto matPtr = Master::m_pResourceManager->FindMaterial("Canonn");

        //    SetupMaterialInfo matInfo[1];
        //    matInfo[0].Index = 0;
        //    matInfo[0].pMaterialData = matPtr;

        //    CreateUtilityMeshInfo mesh;
        //    mesh.pRenderer = &renderer;
        //    mesh.Type = UTILITY_MESH_TYPE::SPHERE;
        //    mesh.MatNum = 1;
        //    mesh.MaterialData = matInfo;
        //    mesh.IsActive = true;
        //    mesh.ShaderType = SHADER_TYPE::DEFERRED_STD_STATIC_N;
        //    mesh.IsNormalMap = false;
        //    mesh.ObjTag = "Canonn";

        //    sphireObj = MeshFactory::CreateUtilityMesh(mesh);
        //    sphireObj->get_Transform().lock()->set_Scale(10.0f, 10.0f, 10.0f);
        //    sphireObj->get_Transform().lock()->set_Pos(0.0f, 1000.0f, 0.0f);
        //    sphireObj->get_Transform().lock()->set_RotateToDeg(0.0f, 0.0f, 0.0f);

        //    auto light = sphireObj->add_Component<PointLight>();
        //    light->Start(renderer);
        //}


        ///* アリ モデルの生成 */
        //{
        //    // マテリアル取得
        //    auto matPtr = Master::m_pResourceManager->FindMaterial("Ant");

        //    SetupMaterialInfo matInfo[1];
        //    matInfo[0].Index = 0;
        //    matInfo[0].pMaterialData = matPtr;

        //    CreateModelInfo model;
        //    model.pRenderer = &renderer;
        //    model.Path = "Resource/Model/Enemy/ant_lowpoly.fbx";
        //    model.ObjTag = "Ant1";
        //    model.IsAnim = true;
        //    model.MatNum = 1;
        //    model.SetupMaterial = matInfo;
        //    model.ShaderType = SHADER_TYPE::DEFERRED_STD_SKINNED_N;

        //    for (int i = 0; i < 2; i++)
        //    {
        //        model.ObjTag = "Ant_" + std::to_string(i + 1);   // タグ

        //        m_pAnt[i] = MeshFactory::CreateModel(model);
        //        m_pAnt[i]->get_Component<Transform>()->set_Scale(0.1f, 0.1f, 0.1f);
        //        m_pAnt[i]->get_Component<Transform>()->set_RotateToDeg(0.0f, 180, 0.0);
        //        m_pAnt[i]->get_Component<Transform>()->set_Pos(0.0f, 0.0f, 0.0);
        //        m_pAnt[i]->get_Component<SkinnedMeshAnimator>()->set_IsAnim(true);
        //        m_pAnt[i]->get_Component<SkinnedMeshAnimator>()->set_AnimIndex(0);

        //        // コライダーの追加
        //        auto collider = m_pAnt[i]->add_Component<BoxCollider>();
        //        collider->set_Size(VEC3(50, 15, 10));
        //        collider->set_Center(VEC3(0, 10, 0));

        //        // コライダーの登録
        //        Master::m_pCollisionManager->RegisterCollider(collider);
        //    }
        //}

        ///* B-2 モデルの生成 */
        //{
        //    // マテリアル取得
        //    auto matPtr = Master::m_pResourceManager->FindMaterial("B_2");

        //    SetupMaterialInfo matInfo[1];
        //    matInfo[0].Index = 0;
        //    matInfo[0].pMaterialData = matPtr;

        //    CreateModelInfo model;
        //    model.pRenderer = &renderer;
        //    model.Path = "Resource/Model/b-2/B-2_NonBone.fbx";
        //    model.ObjTag = "B-2";
        //    model.IsAnim = false;
        //    model.MatNum = 1;
        //    model.SetupMaterial = matInfo;
        //    model.ShaderType = SHADER_TYPE::DEFERRED_STD_STATIC_N;
        //    for (int i = 0; i < 3; i++)
        //    {
        //        model.ObjTag = "B-2_" + std::to_string(i + 1);
        //        m_pBomber[i] = MeshFactory::CreateModel(model);
        //        m_pBomber[i]->get_Component<Transform>()->set_Scale(0.05f, 0.05f, 0.05f);
        //    }
        //}

        ///* クレイモア モデルの生成 */
        //{
        //    // マテリアル取得
        //    auto matPtr = Master::m_pResourceManager->FindMaterial("Claymore");

        //    SetupMaterialInfo matInfo[1];
        //    matInfo[0].Index = 0;
        //    matInfo[0].pMaterialData = matPtr;

        //    CreateModelInfo model;
        //    model.pRenderer = &renderer;
        //    model.Path = "Resource/Model/Claymore/Claymore.fbx";
        //    model.ObjTag = "Claymore";
        //    model.IsAnim = false;
        //    model.MatNum = 1;
        //    model.SetupMaterial = matInfo;
        //    model.ShaderType = SHADER_TYPE::DEFERRED_STD_STATIC_N;
        //    auto obj = MeshFactory::CreateModel(model);
        //    obj->get_Component<Transform>()->set_Scale(2.0f, 2.0f, 2.0f);
        //    obj->get_Component<Transform>()->set_Pos(0.0f, 100.0f, 400.0f);
        //}

        ///* 建物 モデルの生成 */
        //{
        //    // マテリアル取得
        //    auto matPtr = Master::m_pResourceManager->FindMaterial("Building");

        //    SetupMaterialInfo matInfo[1];
        //    matInfo[0].Index = 0;
        //    matInfo[0].pMaterialData = matPtr;

        //    CreateModelInfo model;
        //    model.pRenderer = &renderer;
        //    model.Path = "Resource/Model/Building/01/building01.fbx";
        //    model.ObjTag = "Building";
        //    model.IsAnim = false;
        //    model.MatNum = 1;
        //    model.SetupMaterial = matInfo;
        //    model.ShaderType = SHADER_TYPE::DEFERRED_STD_STATIC;
        //    m_pTempObj = MeshFactory::CreateModel(model);
        //    m_pTempObj->get_Component<Transform>()->set_Scale(0.5f, 0.5f, 0.5f);
        //    m_pTempObj->get_Component<Transform>()->set_Pos(300.0f, 0.0f, 0.0f);
        //    m_pTempObj->get_Component<Transform>()->set_RotateToDeg(0.0f, 0.0f, 0.0f);

        //    // コライダーの追加
        //    auto collider = m_pTempObj->add_Component<BoxCollider>();
        //    collider->set_Size(VEC3(170.0f, 300.0f, 170.0f));
        //    collider->set_Center(VEC3(0.0f, 300.0f, 0.0f));
        //    collider->set_IsStatic(true);

        //    // コライダーの登録
        //    Master::m_pCollisionManager->RegisterCollider(m_pTempObj->get_Component<BoxCollider>());
        //}

        ///* マザーシップの生成 */
        //{
        //    // マテリアル取得
        //    auto matPtr1 = Master::m_pResourceManager->FindMaterial("MotherShip");

        //    SetupMaterialInfo matInfo[1];
        //    matInfo[0].Index = 0;
        //    matInfo[0].pMaterialData = matPtr1;

        //    CreateModelInfo model;
        //    model.pRenderer = &renderer;
        //    model.Path = "Resource/Model/Enemy/MotherShip.fbx";
        //    model.ObjTag = "MotherShip";
        //    model.IsAnim = false;
        //    model.MatNum = 1;
        //    model.IsActive = true;

        //    model.SetupMaterial = matInfo;

        //    model.ShaderType = SHADER_TYPE::DEFERRED_STD_STATIC;
        //    m_pTempObj = MeshFactory::CreateModel(model);
        //    m_pTempObj->get_Component<Transform>()->set_Scale(0.5f, 0.5f, 0.5f);
        //    m_pTempObj->get_Component<Transform>()->set_Pos(0.0f, 2000.0f, 0.0f);
        //    m_pTempObj->get_Component<Transform>()->set_RotateToDeg(0.0f, 0.0f, 0.0f);
        //}

        ///* クモ モデルの生成 */
        //{
        //    // マテリアル取得
        //    auto matPtr1 = Master::m_pResourceManager->FindMaterial("Spider_1");
        //    auto matPtr2 = Master::m_pResourceManager->FindMaterial("Spider_2");
        //    auto matPtr3 = Master::m_pResourceManager->FindMaterial("Spider_3");
        //    auto matPtr4 = Master::m_pResourceManager->FindMaterial("Spider_4");

        //    SetupMaterialInfo matInfo[4];
        //    matInfo[0].Index = 0;
        //    matInfo[0].pMaterialData = matPtr1;
        //    matInfo[1].Index = 1;
        //    matInfo[1].pMaterialData = matPtr2;
        //    matInfo[2].Index = 2;
        //    matInfo[2].pMaterialData = matPtr3;
        //    matInfo[3].Index = 3;
        //    matInfo[3].pMaterialData = matPtr4;

        //    CreateModelInfo model;
        //    model.pRenderer = &renderer;
        //    model.Path = "Resource/Model/fbx/Spider_3.fbx";
        //    model.ObjTag = "Spider";
        //    model.IsAnim = true;
        //    model.MatNum = 4;
        //    model.IsActive = false;

        //    model.SetupMaterial = matInfo;

        //    model.ShaderType = SHADER_TYPE::DEFERRED_STD_SKINNED_N;
        //    m_pTempObj = MeshFactory::CreateModel(model);
        //    m_pTempObj->get_Component<SkinnedMeshAnimator>()->set_IsAnim(true);
        //    m_pTempObj->get_Component<Transform>()->set_Scale(0.5f, 0.5f, 0.5f);
        //    m_pTempObj->get_Component<Transform>()->set_Pos(0.0f, 0.0f, 0.0f);
        //    m_pTempObj->get_Component<Transform>()->set_RotateToDeg(0.0f, 0.0f, 0.0f);
        //}

        ///* 地面の生成 */
        //{
        //    // マテリアル取得
        //    auto matPtr = Master::m_pResourceManager->FindMaterial("Ground");

        //    SetupMaterialInfo matInfo[1];
        //    matInfo[0].Index = 0;
        //    matInfo[0].pMaterialData = matPtr;

        //    CreateUtilityMeshInfo mesh;
        //    mesh.pRenderer = &renderer;
        //    mesh.Type = UTILITY_MESH_TYPE::PLANE;
        //    mesh.ObjTag = "Ground";
        //    mesh.MatNum = 1;
        //    mesh.MaterialData = matInfo;
        //    mesh.ShaderType = SHADER_TYPE::DEFERRED_STD_STATIC_N;
        //    mesh.IsNormalMap = true;

        //    auto obj = MeshFactory::CreateUtilityMesh(mesh);
        //    obj->get_Transform().lock()->set_Scale(1000.0f, 500.0f, 1000.0f);
        //    obj->get_Transform().lock()->set_Pos(0.0f, 0.0f, 0.0f);
        //    obj->get_Transform().lock()->set_RotateToDeg(0.0f, 0.0f, 0.0f);

        //    // コライダーの追加
        //    auto collider = obj->add_Component<BoxCollider>();
        //    collider->set_Size(VEC3(1000, 1, 1000));
        //    collider->set_Center(VEC3(0, 0, 0));
        //    collider->set_IsStatic(true);

        //    // コライダーの登録
        //    Master::m_pCollisionManager->RegisterCollider(collider);
        //}

        ///* スカイボックスの生成 */
        //{
        //    // マテリアル取得
        //    auto matPtr = Master::m_pResourceManager->FindMaterial("SkyBox");

        //    SetupMaterialInfo matInfo[1];
        //    matInfo[0].Index = 0;
        //    matInfo[0].pMaterialData = matPtr;

        //    CreateSkyboxInfo skyInfo;
        //    skyInfo.pRenderer = &renderer;
        //    skyInfo.ObjTag = "Skybox";
        //    skyInfo.MatNum = 1;
        //    skyInfo.MaterialData = matInfo;
        //    skyInfo.ShaderType = SHADER_TYPE::POST_SKYBOX;
        //    skyInfo.IsActive = false;

        //    auto obj = MeshFactory::CreateSkybox(skyInfo);
        //    obj->get_Transform().lock()->set_Scale(1.0f, 1.0f, 1.0f);
        //    obj->get_Transform().lock()->set_Pos(0.0f, 0.0f, 0.0f);
        //}

        ///* ビルボードの生成 */
        //{
        //    // マテリアル取得
        //    auto matPtr = Master::m_pResourceManager->FindMaterial("Billboard");

        //    SetupMaterialInfo matInfo[1];
        //    matInfo[0].Index = 0;
        //    matInfo[0].pMaterialData = matPtr;


        //    CreateBillboradInfo billboard;
        //    billboard.pRenderer = &renderer;
        //    billboard.Type = BILLBOARD_USAGE_TYPE::SIMPLE;
        //    billboard.ShaderType = SHADER_TYPE::FORWARD_UNLIT_STATIC;
        //    billboard.IsActive = true;
        //    billboard.MatNum = 1;
        //    billboard.MaterialData = matInfo;
        //    billboard.IsTransparent = true; // 透明度があり

        //    for (int i = 0; i < 10; i++)
        //    {
        //        VEC3 pos;
        //        pos.x = static_cast<float>(rand() % 2000) - 1000.0f;
        //        pos.y = static_cast<float>(rand() % 200) - 100.0f;;
        //        pos.z = static_cast<float>(rand() % 2000) - 1000.0f;

        //        VEC3 scl;
        //        scl.x = static_cast<float>(rand() % 50) - 25.0f;
        //        scl.y = static_cast<float>(rand() % 50) - 25.0f;;
        //        scl.z = static_cast<float>(rand() % 50) - 25.0f;

        //        VEC3 col;
        //        col.x = static_cast<float>(rand() % 255) / 255.0f;
        //        col.y = static_cast<float>(rand() % 255) / 255.0f;
        //        col.z = static_cast<float>(rand() % 255) / 255.0f;

        //        //mat->m_DiffuseColor = VEC4(0.5, 0.5, 0.5, 1.0f);
        //        auto obj = MeshFactory::CreateBillboard(billboard);
        //        obj->get_Transform().lock()->set_Pos(pos);
        //        obj->get_Transform().lock()->set_Scale(50, 50, 50);
        //        obj->set_Tag("Billboard" + std::to_string(i));
        //    }
        //}

        ///* ポイントライトの生成 (Cubuで分かりやすく)*/
        //{
        //    // マテリアル取得
        //    auto matPtr = Master::m_pResourceManager->FindMaterial("PointLight");

        //    SetupMaterialInfo matInfo[1];
        //    matInfo[0].Index = 0;
        //    matInfo[0].pMaterialData = matPtr;

        //    CreateUtilityMeshInfo mesh;
        //    mesh.pRenderer = &renderer;
        //    mesh.Type = UTILITY_MESH_TYPE::CUBU;
        //    mesh.MatNum = 1;
        //    mesh.MaterialData = matInfo;
        //    mesh.IsActive = true;
        //    mesh.ShaderType = SHADER_TYPE::DEFERRED_STD_STATIC_N;
        //    mesh.IsNormalMap = true;

        //    for (int i = 0; i < 30; i++)
        //    {
        //        VEC3 pt;
        //        pt.x = static_cast<float>(rand() % 1000) - 500.0f;
        //        pt.y = 50.0f;
        //        pt.z = static_cast<float>(rand() % 1000) - 500.0f;
        //        VEC3 col;
        //        col.x = static_cast<float>(rand() % 255) / 255.0f;
        //        col.y = static_cast<float>(rand() % 255) / 255.0f;
        //        col.z = static_cast<float>(rand() % 255) / 255.0f;

        //        auto obj = MeshFactory::CreateUtilityMesh(mesh);
        //        obj->get_Transform().lock()->set_Pos(pt);
        //        obj->get_Transform().lock()->set_Scale(VEC3(10, 10, 10));
        //        obj->set_Tag("PointLight" + std::to_string(i));
        //        auto light = obj->add_Component<PointLight>();
        //        light->set_LightColor(col);
        //        light->set_Range(100.0f);
        //        light->set_Intensity(25.0f);
        //        light->Start(renderer);
        //    }
        //}

        ///* UIの作成 */
        //{
        //    auto matPtr = Master::m_pResourceManager->FindMaterial("Billboard");

        //    CreateSpriteInfo spriteInitData;;
        //    spriteInitData.pRenderer = &renderer;
        //    spriteInitData.ObjTag = "UI";
        //    spriteInitData.Type = SPRITE_USAGE_TYPE::NORMAL;
        //    spriteInitData.ShaderType = SHADER_TYPE::FORWARD_UNLIT_UI_SPRITE;
        //    spriteInitData.IsActive = false;
        //    spriteInitData.Width = 0.0f;
        //    spriteInitData.Height = 0.0f;
        //    spriteInitData.pTextureMap[0] = Master::m_pResourceManager->LoadWIC_Texture(L"Resource/Texture/Light_Img.png");
        //    auto obj = MeshFactory::CreateSprite(spriteInitData);
        //}
    }

    /* デカールの生成*/
    {
        Material mat[1];
        mat[0].m_DiffuseMap.Texture = Master::m_pResourceManager->LoadWIC_Texture(L"Resource/Texture/Wall_W033.jpg");
        mat[0].m_DiffuseColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);
        mat[0].m_SpecularColor = VEC4(1.0f, 1.0f, 1.0f, 1.0f);
        mat[0].m_SpecularPower = 100.0f;
        mat[0].m_EmissivePower = 1.0f;
        mat[0].m_EmissiveColor = VEC3(1.0f, 1.0f, 1.0f);

        // マテリアル登録
        Master::m_pResourceManager->RegisterMaterialData("Decal", mat[0]);
        auto matPtr = Master::m_pResourceManager->FindMaterial("Decal");

        SetupMaterialInfo matInfo[1];
        matInfo[0].Index = 0;
        matInfo[0].pMaterialData = matPtr;

        CreateDecalInfo decal;
        decal.pRenderer = &renderer;
        decal.Type = UTILITY_MESH_TYPE::CUBU;
        decal.MatNum = 1;
        decal.MaterialData = matInfo;
        decal.IsActive = false;
        decal.ShaderType = SHADER_TYPE::DEFERRED_STD_DECAL;
        decal.IsNormalMap = false;
        decal.IsDynamic = true;

        VEC3 pt;
        pt.x = 10.0f;
        pt.y = 10.0f;
        pt.z = 10.0f;

        auto obj = MeshFactory::CreateDecal(decal);
        obj->get_Component<DecalRenderer>()->Start(renderer);
        obj->get_Transform().lock()->set_Pos(pt);
        obj->get_Transform().lock()->set_Scale(VEC3(1, 1, 1));
        obj->set_Tag("Decal");
    }

    // パイプラインの作成
    if (!renderer.CreateRendererPipeline(RENDER_PIPELINE_STATE::DEFAULT))
    {
        return false;
    }

    // ステートマシンの作成
    SceneFactory::Create(m_StateMachine, SCENE_STATE::SCENE_STATE_TITLE, renderer);
    SceneFactory::Create(m_StateMachine, SCENE_STATE::SCENE_STATE_GAME, renderer);
    SceneFactory::Create(m_StateMachine, SCENE_STATE::SCENE_STATE_RESULT, renderer);
    m_StateMachine.SetCrntState(SCENE_STATE::SCENE_STATE_TITLE);

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
    static float counter = 0.0f;
    counter += 0.01f;

    // シーンステートの実行
    m_StateMachine.Update();

    // オブジェクト更新
    Master::m_pGameObjectManager->ObjectUpdate(renderer);

    ////-----------------------------------------------------------------------------
    //// ■ アリの移動処理
    ////-----------------------------------------------------------------------------
    //for (int i = 0; i < 2; i++)
    //{
    //    auto antTransform = m_pAnt[i]->get_Transform().lock();
    //    if (antTransform)
    //    {
    //        VEC3 pos = VEC3();
    //        pos.y = 15.0f;
    //        VEC3 crntPos = antTransform->get_VEC3ToPos();
    //        VEC3 crntRot = antTransform->get_VEC3ToRotateToRad();

    //        if (i % 2 == 0) {
    //            pos.z = sin(counter) * 300.0f;
    //        }
    //        else{
    //            pos.x = cos(counter) * 300.0f;
    //        }

    //        VEC3 velocity = pos - crntPos;

    //        // 移動ベクトルから、Y軸周りの回転角度を求める
    //        float targetAngle = atan2(velocity.x, velocity.z);

    //        targetAngle = Lerp(crntRot.y, targetAngle, 0.1f);

    //        antTransform->set_Pos(pos);
    //        antTransform->set_RotateToRad(0, targetAngle, 0);
    //    }
    //}
    //
    ////-----------------------------------------------------------------------------
    //// ■ 爆撃機の移動処理
    ////-----------------------------------------------------------------------------
    //static VEC3 B2PrevPos[3]{};
    //VEC3 center = VEC3(0.0f, 0.0f, 0.0f);
    //float radius = 700.0f;
    //for (int i = 0; i < 3; i++)
    //{
    //    auto B2Transform = m_pBomber[i]->get_Transform().lock();
    //    if (B2Transform)
    //    {
    //        float intervalDist = 200.0f * i;

    //        VEC3 pos = VEC3(0.0f, 0.0f, 0.0f);
    //        pos.x = center.x + (radius - intervalDist) * cos(counter);
    //        pos.y = 300.0f + i * 100.0f;
    //        pos.z = center.z + (radius - intervalDist) * sin(counter);

    //        // 速度 = 今の座標 - 前の座標
    //        VEC3 velocity = pos - B2PrevPos[i];

    //        // 移動ベクトルから、Y軸周りの回転角度を求める
    //        float targetAngle = atan2(velocity.x, velocity.z);

    //        // 前の座標として保持
    //        B2PrevPos[i] = pos;

    //        B2Transform->set_Pos(pos);
    //        B2Transform->set_RotateToRad(0.0f, targetAngle, 0.0f);
    //    }
    //}

    // 衝突判定
    Master::m_pCollisionManager->CollisionProcess();

    // 遅延更新
    Master::m_pGameObjectManager->ObjectLateUpdate(renderer);

    // エディタの更新
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
    // レンダリングパイプラインの実行
    renderer.ExecuteDefaultRendererPipeline(RENDER_PIPELINE_STATE::DEFAULT);

    // シーンステートの描画
    m_StateMachine.Draw();


    //Master::m_pDirectWriteManager->DrawString("こんにちは",VEC2(940, 540));
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


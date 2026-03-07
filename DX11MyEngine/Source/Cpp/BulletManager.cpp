#include "pch.h"
#include "RendererEngine.h"
#include "BulletManager.h"
#include "Component_NormalBullet.h"
#include "Component_ExplosionBullet.h"
#include "Component_BoxCollider.h"
#include "Component_SphereCollider.h"
#include "Component_TrailRenderer.h"
#include "MeshFactory.h"
#include "ResourceManager.h"
#include "Component_MoveLogic.h"

using namespace VECTOR3;
using namespace VECTOR2;

//////////////////////////////////////////////////////////////////////////////////////////
//
//						各プールのパラメータ
// 
//////////////////////////////////////////////////////////////////////////////////////////
// 通常弾 =====================================================================
constexpr int NUM_DEFAULT__NORMAL_BULLET    = 100;
constexpr int NUM_MAX__NORMAL_BULLET        = 150;

// 爆発弾 =====================================================================
constexpr int NUM_DEFAULT__EXPLOSION_BULLET = 50;
constexpr int NUM_MAX__EXPLOSION_BULLET     = 100;

// 誘導弾 =====================================================================
constexpr int NUM_DEFAULT__HORMING_BULLET   = 50;
constexpr int NUM_MAX__HORMING_BULLET       = 100;

using namespace BulletData;
using namespace GIGA_Engine;

//*---------------------------------------------------------------------------------------
//*【?】コンストラクタ
//*----------------------------------------------------------------------------------------
BulletManager::BulletManager()
{
}

//*---------------------------------------------------------------------------------------
//*【?】デストラクタ
//*----------------------------------------------------------------------------------------
BulletManager::~BulletManager()
{
    m_BulletObjectPoolMap.clear();
}


//*---------------------------------------------------------------------------------------
//*【?】初期化処理
//*
//* [引数]
//* renderer : 描画エンジンの参照
//*
//* [返値] 
//* true : 成功
//* false: 失敗
//*----------------------------------------------------------------------------------------
bool BulletManager::Init(RendererEngine &renderer)
{
    //////////////////////////////////////////////////////////////////////////////////////////
    //
    //
    //						通常弾のプール
    // 
    //
    //////////////////////////////////////////////////////////////////////////////////////////
    m_BulletObjectPoolMap.emplace(BULLET_TYPE::NORMAL,ObjectPool<GameObject>(
        // 取得時に実行 ******************************************************************************************
        [&renderer](GameObject *obj) {          
            obj->set_StatusFlag(OBJECT_STATUS_BITFLAG::IS_ACTIVE);

            // コライダーの使用をオンに
            auto collider = obj->get_Component<BoxCollider>();
            collider->set_IsEnable(true); 
        // 取得時に実行 ******************************************************************************************
        },
        // 返却時に実行 ******************************************************************************************
        [](GameObject *obj) 
        {
            auto bulletComp = obj->get_Component<NormalBullet>();
            bulletComp->Reset();

            // コライダーの使用をオフに
            auto collider = obj->get_Component<BoxCollider>();
            collider->set_IsEnable(false); 

            // 軌跡データをクリア
            auto trail = obj->get_Component<TrailRenderer>();
            trail->clear_TrailInfoList();
        },
        // 生成時に実行 ******************************************************************************************
        [&renderer]()->GameObject *
        {
            // マテリアル取得
            auto matPtr1 = Master::m_pResourceManager->FindMaterial("Bullet");
            SetupMaterialInfo matInfo[1];
            matInfo[0].Index = 0;
            matInfo[0].pMaterialData = matPtr1;

            // メッシュ作成
            CreateModelInfo model;
            model.pRenderer = &renderer;
            model.Path = "Resource/Model/Weapon/bullet.fbx";
            model.ObjTag = "Bullet_Normal";
            model.IsAnim = false;
            model.MatNum = 1;
            model.SetupMaterial = matInfo;
            model.ShaderType = SHADER_TYPE::DEFERRED_STD_STATIC;
            auto obj = MeshFactory::CreateModel(model);
            if (obj == nullptr) {
                assert(false);
                return nullptr;
            }
            
            obj->clear_StatusFlag(OBJECT_STATUS_BITFLAG::IS_ACTIVE);    // ノンアクティブ
            
            // 動的オブジェクトとして設定
            obj->set_State(OBJECT_STATE::DYNAMIC);

            // バレットコンポーネントの追加
            auto bulletComp = obj->add_Component<NormalBullet>();

            auto moveComp = obj->add_Component<MoveLogic>();
            moveComp->Register(MOVE_BEHAVIOUR_TYPE::LINEAR);
            moveComp->ChangeBehaviour(MOVE_BEHAVIOUR_TYPE::LINEAR);

            // 軌跡
            auto trail = obj->add_Component<TrailRenderer>();
            trail->set_Width(2.0f);
            trail->set_MinVertexDistance(5.0f);
            trail->set_DrawTime(5);
            trail->set_EmissivePower(5.0f);
            trail->set_Color(VECTOR4::VEC4(0.0f, 1.0f, 0.0f, 1.0f));

            // コライダーの追加
            auto collider = obj->add_Component<BoxCollider>();
            collider->set_Size(VEC3(5.0f, 5.0f, 5.0f));
            collider->set_Center(VEC3(0.0f, 2.0f, 0.0f));
            collider->set_IsEnable(false);  // 初期化時は使用フラグオフに

            // カテゴリ
            collider->set_CollisionCategory(COLLISION_CATEGORY::PLAYER_BULLET);
            // 衝突マスク
            collider->set_CollisionBitMask(UINT_CAST(COLLISION_CATEGORY::ENEMY) | UINT_CAST(COLLISION_CATEGORY::BUILDING));

            // コライダーの登録
            Master::m_pCollisionManager->RegisterCollider(collider);

            // 初期化
            bulletComp->Start(renderer);

            return obj.get();
        },
        NUM_DEFAULT__NORMAL_BULLET,
        NUM_MAX__NORMAL_BULLET
    ));

    //////////////////////////////////////////////////////////////////////////////////////////
    //
    //
    //						爆発弾のプール
    // 
    //
    //////////////////////////////////////////////////////////////////////////////////////////
    m_BulletObjectPoolMap.emplace(BULLET_TYPE::EXPLOSION, ObjectPool<GameObject>(
        // 取得時に実行 ******************************************************************************************
        [&renderer](GameObject* obj)   
        {          
            obj->set_StatusFlag(OBJECT_STATUS_BITFLAG::IS_ACTIVE);

            // コライダーの使用をオンに
            auto collider = obj->get_Component<BoxCollider>();
            collider->set_IsEnable(true);
        },
        // 返却時に実行 ******************************************************************************************
        [](GameObject* obj)          
        {                  
            auto bulletComp = obj->get_Component<ExplosionBullet>();
            bulletComp->Reset();

            // コライダーの使用をオフに
            auto collider = obj->get_Component<BoxCollider>();
            collider->set_IsEnable(false);

            // 軌跡データをクリア
            auto trail = obj->get_Component<TrailRenderer>();
            trail->clear_TrailInfoList();
        },
        // 生成時に実行 ******************************************************************************************
        [&renderer]()->GameObject*  
        {
            // マテリアル取得
            auto matPtr1 = Master::m_pResourceManager->FindMaterial("Bullet");
            SetupMaterialInfo matInfo[1];
            matInfo[0].Index = 0;
            matInfo[0].pMaterialData = matPtr1;

            // メッシュ作成
            CreateModelInfo model;
            model.pRenderer = &renderer;
            model.Path = "Resource/Model/Weapon/bullet.fbx";
            model.ObjTag = "Bullet_Explosion";
            model.IsAnim = false;
            model.MatNum = 1;
            model.SetupMaterial = matInfo;
            model.ShaderType = SHADER_TYPE::DEFERRED_STD_STATIC;
            auto obj = MeshFactory::CreateModel(model);
            if (obj == nullptr) {
                assert(false);
                return nullptr;
            }

            // 動的オブジェクトとして設定
            obj->set_State(OBJECT_STATE::DYNAMIC);

            obj->clear_StatusFlag(OBJECT_STATUS_BITFLAG::IS_ACTIVE);    // ノンアクティブ

            // バレットコンポーネントの追加
            auto bulletComp = obj->add_Component<ExplosionBullet>();

            // 移動用コンポーネントの追加
            auto moveComp = obj->add_Component<MoveLogic>();
            moveComp->Register(MOVE_BEHAVIOUR_TYPE::LINEAR);
            moveComp->ChangeBehaviour(MOVE_BEHAVIOUR_TYPE::LINEAR);

            // 軌跡コンポーネントの追加
            auto trail = obj->add_Component<TrailRenderer>();
            trail->set_Width(2.0f);
            trail->set_MinVertexDistance(5.0f);
            trail->set_DrawTime(10.0f);
            trail->set_EmissivePower(10.0f);
            trail->set_Color(VECTOR4::VEC4(0.0f, 1.0f, 0.0f, 1.0f));

            // 衝突用コライダーの追加
            auto collider = obj->add_Component<BoxCollider>();
            collider->set_Size(VEC3(5.0f, 5.0f, 5.0f));
            collider->set_Center(VEC3(0.0f, 2.0f, 0.0f));
            collider->set_IsEnable(false);  // 初期化時は使用フラグオフに
            collider->set_IsTrigger(true);  // トリガーとして設定
            
            // カテゴリ
            collider->set_CollisionCategory(COLLISION_CATEGORY::PLAYER_BULLET);
            // 衝突マスク
            collider->set_CollisionBitMask(UINT_CAST(COLLISION_CATEGORY::ENEMY) | UINT_CAST(COLLISION_CATEGORY::BUILDING));

            // コライダーの登録
            Master::m_pCollisionManager->RegisterCollider(collider);

            // 初期化
            bulletComp->Start(renderer);

            return obj.get();
        },
        NUM_DEFAULT__EXPLOSION_BULLET,
        NUM_MAX__EXPLOSION_BULLET
    ));

    //m_BulletObjectPoolMap.emplace(BULLET_TYPE::HORMING, ObjectPool<GameObject>(
    //    [](GameObject *obj) {},
    //    [](GameObject *obj) {},
    //    []()->GameObject *{ return Instantiate(std::make_shared<GameObject>(), false).get(); },
    //    NUM_DEFAULT__HORMING_BULLET,
    //    NUM_MAX__HORMING_BULLET
    //));

    return true;
}


//*---------------------------------------------------------------------------------------
//*【?】更新処理
//*
//* [引数]
//* renderer : 描画エンジンの参照
//*
//* [返値] なし
//*----------------------------------------------------------------------------------------
void BulletManager::Update(RendererEngine &renderer)
{
    // 取り出したオブジェクトのみ更新させる
    for (auto mapIt = m_ExtractedBulletMap.begin(); mapIt != m_ExtractedBulletMap.end(); )
    {
        // プールが存在するかどうかの確認
        auto poolIt = m_BulletObjectPoolMap.find(mapIt->first);
        if (poolIt == m_BulletObjectPoolMap.end())
        {
            OutputDebugString("指定された弾のプールが存在しません");
            continue;
        }

        auto& pool = poolIt->second;        // プールの取り出し
        auto &bulletArray = mapIt->second;  // 弾配列の取り出し


        for (auto bulletIt = bulletArray.begin(); bulletIt != bulletArray.end(); )
        {
            auto bullet = *bulletIt;

            // アクティブフラグが降りていれば、プールへ返却
            if (bullet->get_IsStatusFlag(OBJECT_STATUS_BITFLAG::IS_ACTIVE) == false) 
            {
                // 返却
                pool.release(bullet);

                // 次の要素へ
                bulletIt = bulletArray.erase(bulletIt); 
            }
            else
            {
                ++bulletIt;
            }
        }

        ++mapIt;
    }

    Master::m_pDebugger->BeginDebugWindow("BulletManager", 0);
    Master::m_pDebugger->DG_BulletText(Tool::U8ToChar(u8"プール"));
    for (int i = 0; i < static_cast<int>(BULLET_TYPE::NUM); i++)
    {
        if (m_BulletObjectPoolMap.empty())break;

        if (Master::m_pDebugger->DG_TreeNode(std::to_string(i)))
        {
            // プール本体の情報 **********************************************************
            // プールが存在するかどうかチェック
            auto it = m_BulletObjectPoolMap.find(static_cast<BULLET_TYPE>(i));
            if (it != m_BulletObjectPoolMap.end())
            {
                Master::m_pDebugger->DG_BulletText(Tool::U8ToChar(u8"プール最大数：%d"), it->second.get_MaxNum());
                Master::m_pDebugger->DG_BulletText(Tool::U8ToChar(u8"プールの現在の生成数：%d"), it->second.get_CrntCreateNum());


                // プールから取り出して使用しているオブジェクトの情報 ********************************************************
                auto& extractedIt = m_ExtractedBulletMap[static_cast<BULLET_TYPE>(i)];
                Master::m_pDebugger->DG_BulletText(Tool::U8ToChar(u8"使用しているオブジェクト数：%d"), extractedIt.size());
                
            }
            else
            {
                Master::m_pDebugger->DG_BulletText(Tool::U8ToChar(u8"プールが存在しません。"));
            }

            // ツリー終了
            Master::m_pDebugger->DG_TreePop();
        }
    }
    Master::m_pDebugger->EndDebugWindow();

}

//*---------------------------------------------------------------------------------------
//*【?】描画処理
//*
//* [引数]
//* renderer : 描画エンジンの参照
//*
//* [返値] なし
//*----------------------------------------------------------------------------------------
void BulletManager::Draw(RendererEngine &renderer)
{

}


//*---------------------------------------------------------------------------------------
//*【?】弾の登録
//*
//* [引数]
//* _bulletType : 登録する弾の種類
//* pBullet : 弾のオブジェクト
//*
//* [返値] なし
//*----------------------------------------------------------------------------------------
void BulletManager::RegisterBullet(BulletData::BULLET_TYPE _bulletType, std::shared_ptr<GameObject> pBullet)
{

}

//*---------------------------------------------------------------------------------------
//*【?】通常弾の発射
//*
//* [引数]
//* &renderer         : 描画エンジンの参照
//* &_transformData   : トランスフォームパラメータ
//* &_param           : パラメータ
//* [返値] なし
//*----------------------------------------------------------------------------------------
void BulletManager::Shot(RendererEngine &renderer, const BulletTransformData &_transformData, const BulletData::NormalBulletData &_param)
{
    auto &pool = m_BulletObjectPoolMap.find(BULLET_TYPE::NORMAL)->second;
    auto obj = pool.get();
    if (obj == nullptr)
    {
        OutputDebugString("プールに空きがありません");
        return;
    }

    // トランスフォームの設定
    auto transform = obj->get_Transform().lock();
    transform->set_Pos(_transformData._pos);
    transform->set_RotateToRad(_transformData._rotRad);
    transform->set_Scale(_transformData._scale);


    // 弾コンポーネントのセットアップ
    auto bulletComp = obj->get_Component<NormalBullet>();
    bulletComp->set_Parameter(_param);
    bulletComp->Setup();

    // 更新リストに登録
    m_ExtractedBulletMap[BULLET_TYPE::NORMAL].push_back(obj);
}

//*---------------------------------------------------------------------------------------
//*【?】爆発弾の発射
//*
//* [引数]
//* &renderer         : 描画エンジンの参照
//* &_transformData   : トランスフォームパラメータ
//* &_param           : パラメータ
//* [返値] なし
//*----------------------------------------------------------------------------------------
void BulletManager::Shot(RendererEngine &renderer, const BulletTransformData &_transformData, const BulletData::ExplosionBulletData &_param)
{
    auto &pool = m_BulletObjectPoolMap.find(BULLET_TYPE::EXPLOSION)->second;
    auto obj = pool.get();
    if (obj == nullptr)
    {
        OutputDebugString("プールに空きがありません");
        return;
    }

    // トランスフォームの設定
    auto transform = obj->get_Transform().lock();
    transform->set_Pos(_transformData._pos);
    transform->set_RotateToRad(_transformData._rotRad);
    transform->set_Scale(_transformData._scale);


    // 弾コンポーネントのセットアップ
    auto bulletComp = obj->get_Component<ExplosionBullet>();
    bulletComp->set_Parameter(_param);
    bulletComp->Setup();

    auto collider = obj->get_Component<BoxCollider>();
    collider->set_Size(VEC3(_transformData._scale));

    // 更新リストに登録
    m_ExtractedBulletMap[BULLET_TYPE::EXPLOSION].push_back(obj);
}

//*---------------------------------------------------------------------------------------
//*【?】ホーミング爆発弾の発射
//*
//* [引数]
//* &renderer         : 描画エンジンの参照
//* &_transformData   : トランスフォームパラメータ
//* &_param           : パラメータ
//* [返値] なし
//*----------------------------------------------------------------------------------------
void BulletManager::Shot(RendererEngine &renderer, const BulletTransformData &_transformData, const BulletData::HormingExplosionBulletData &_param)
{
    auto &pool = m_BulletObjectPoolMap.find(BULLET_TYPE::HORMING)->second;
    auto obj = pool.get();
    if (obj == nullptr)
    {
        OutputDebugString("プールに空きがありません");
        return;
    }

    auto transform = obj->get_Transform().lock();
    transform->set_Pos(_transformData._pos);
    transform->set_RotateToRad(_transformData._rotRad);
    transform->set_Scale(_transformData._scale);

    // 更新リストに登録
    m_ExtractedBulletMap[BULLET_TYPE::HORMING].push_back(obj);
}
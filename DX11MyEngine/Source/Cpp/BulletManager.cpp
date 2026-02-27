#include "pch.h"
#include "BulletManager.h"

//////////////////////////////////////////////////////////////////////////////////////////
//
//						各プールのパラメータ
// 
//////////////////////////////////////////////////////////////////////////////////////////
// 通常弾 =====================================================================
constexpr int NUM_DEFAULT__NORMAL_BULLET    = 200;
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
    m_BulletObjectPoolMap.emplace(BULLET_TYPE::NORMAL,ObjectPool<GameObject>(
        [](GameObject *obj) {},
        [](GameObject *obj) {},
        []()->GameObject *{ return Instantiate(std::make_shared<GameObject>(), false).get(); },
        150,
        200
    ));

    m_BulletObjectPoolMap.emplace(BULLET_TYPE::EXPLOSION, ObjectPool<GameObject>(
        [](GameObject *obj) {},
        [](GameObject *obj) {},
        []()->GameObject *{ return Instantiate(std::make_shared<GameObject>(), false).get(); },
         50,
        100
    ));

    m_BulletObjectPoolMap.emplace(BULLET_TYPE::HORMING, ObjectPool<GameObject>(
        [](GameObject *obj) {},
        [](GameObject *obj) {},
        []()->GameObject *{ return Instantiate(std::make_shared<GameObject>(), false).get(); },
         50,
        100
    ));









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

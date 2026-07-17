#pragma once
#include "ConstantBulletData.h"
#include "ObjectPool.h"


/// <summary>
/// 弾の生成に必要な情報
/// </summary>
struct CreateBulletInfo
{
    RendererEngine* _pRenderer;
    BulletData::BULLET_TYPE _type;


};


class GameObject;

// ---------------------------------------------------------------------------------------
/* --- @:BulletManager Class --- */
//
//  ★★★シングルトン★★★
//
// 【?】弾の管理
//      
//
// ***************************************************************************************
class BulletManager
{
private:
    std::unordered_map<BulletData::BULLET_VISUAL_ARCHETYPE, ObjectPool<GameObject>> m_BulletObjectPoolMap;      // 弾の見た目ごとの連想配列プール
    std::unordered_map < BulletData::BULLET_VISUAL_ARCHETYPE, std::vector<GameObject *>> m_ExtractedBulletMap;  // 取り出した弾オブジェクトを一時的に保持する（役割が終わっていたら返す）
    std::unique_ptr<ObjectPool<GameObject>> m_pExplosionBulletLightPool;                                        // 爆発弾用のライトプール
    std::vector<GameObject *> m_ExtractedExplosionLightArray;                                                   // 取り出した爆発ライトオブジェクトを一時的に保持する（役割が終わっていたら返す）


public:
    BulletManager();
    ~BulletManager();

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="renderer">描画エンジンの参照</param>
    /// <returns>成功/失敗</returns>
    bool Init(RendererEngine &renderer);

    /// <summary>
    /// 更新
    /// </summary>
    /// <param name="renderer">描画エンジンの参照</param>
    void Update(RendererEngine &renderer);  

    /// <summary>
    /// 描画
    /// </summary>
    /// <param name="renderer">描画エンジンの参照</param>
    void Draw(RendererEngine &renderer);  

    /// <summary>
    /// 現在、アクティブ状態の弾をクリアする（プールへ帰す）
    /// </summary>
    void clear_CrntActiveBullet();

    /// <summary>
    /// 弾の登録
    /// </summary>
    /// <param name="pBullet"></param>
    void RegisterBullet(BulletData::BULLET_TYPE _bulletType, std::shared_ptr<GameObject> pBullet);

    void Shot(RendererEngine &renderer, const BulletData::BulletSpawnContext& _context, const BulletData::BulletDataBase&_param);
    //void Shot(RendererEngine &renderer, const BulletData::BulletTransformData& _transformData, const BulletData::ExplosionBulletData &_param);
    //void Shot(RendererEngine &renderer, const BulletData::BulletTransformData& _transformData, const BulletData::HormingExplosionBulletData &_param);

private:
    // コピー禁止
    BulletManager(const BulletManager &) = delete;
    BulletManager &operator=(const BulletManager &) = delete;

};


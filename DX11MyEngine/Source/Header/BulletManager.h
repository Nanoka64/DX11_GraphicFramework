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

/// <summary>
/// 弾のトランスフォーム情報
/// </summary>
struct BulletTransformData {
    VECTOR3::VEC3 _pos;
    VECTOR3::VEC3 _rotRad;
    VECTOR3::VEC3 _scale;
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
    std::unordered_map<BulletData::BULLET_TYPE, ObjectPool<GameObject>> m_BulletObjectPoolMap;      // 弾の連想配列プール
    std::unordered_map < BulletData::BULLET_TYPE, std::vector<GameObject *>> m_ExtractedBulletMap;  // 取り出した弾オブジェクトを一時的に保持する（役割が終わっていたら返す）
    std::unique_ptr<ObjectPool<GameObject>> m_pExplosionBulletLightPool;                            // 爆発弾用のライトプール
    std::vector<GameObject *> m_ExtractedExplosionLightArray;                                           // 取り出した爆発ライトオブジェクトを一時的に保持する（役割が終わっていたら返す）


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
    /// 弾の登録
    /// </summary>
    /// <param name="pBullet"></param>
    void RegisterBullet(BulletData::BULLET_TYPE _bulletType, std::shared_ptr<GameObject> pBullet);

    void Shot(RendererEngine &renderer, const BulletTransformData& _transformData, const BulletData::NormalBulletData &_param);
    void Shot(RendererEngine &renderer, const BulletTransformData& _transformData, const BulletData::ExplosionBulletData &_param);
    void Shot(RendererEngine &renderer, const BulletTransformData& _transformData, const BulletData::HormingExplosionBulletData &_param);

private:
    // コピー禁止
    BulletManager(const BulletManager &) = delete;
    BulletManager &operator=(const BulletManager &) = delete;

};


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
    std::unordered_map<BulletData::BULLET_TYPE, ObjectPool<GameObject>> m_BulletObjectPoolMap;

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
    /// 弾の登録
    /// </summary>
    /// <param name="pBullet"></param>
    void RegisterBullet(BulletData::BULLET_TYPE _bulletType, std::shared_ptr<GameObject> pBullet);


private:
    // コピー禁止
    BulletManager(const BulletManager &) = delete;
    BulletManager &operator=(const BulletManager &) = delete;

};


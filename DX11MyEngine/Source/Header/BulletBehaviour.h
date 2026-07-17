#pragma once
#include "ConstantBulletData.h"


// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:IBulletBehaviour Class --- */
//
//  ★★★抽象クラス★★★
//
// 【?】弾の振る舞い基底クラス
//		
// ***************************************************************************************
class IBulletBehaviour
{
public:
    virtual ~IBulletBehaviour() = default;

    /// <summary> 生成時の処理 </summary>
    /// <param name="bullet"></param>
    /// <param name="renderer"></param>
    virtual void OnSpawn(RendererEngine& renderer,class Bullet& bullet) {}

    /// <summary> 飛行時の更新処理 </summary>
    /// <param name="_bullet"></param>
    /// <param name="_renderer"></param>
    /// <param name="_deltaTime"></param>
    virtual void OnFlightUpdate(
        RendererEngine& _renderer,
        class Bullet& _bullet,
        float _deltaTime) {
    }

    /// <summary> ヒット時の処理 </summary>
    /// <param name="_bullet"></param>
    /// <param name="_renderer"></param>
    /// <param name="_hit"></param>
    /// <returns></returns>
    virtual BulletData::BulletHitResult OnHit(
        RendererEngine& _renderer,
        class Bullet& _bullet,
        const class CollisionInfo& _hit) = 0;

    /// <summary> 終了時の処理 </summary>
    /// <param name="_bullet"></param>
    /// <param name="_reason"></param>
    virtual void OnEnd(
        class Bullet& _bullet,
        BulletData::BULLET_END_REASON _reason) {
    }

    /// <summary> パラメータ等のリセット </summary>
    virtual void Reset() {}
};


// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:ExplosionBehaviour Class --- */
//
//  ★継承：IBulletBehaviour ★
// 
// 【?】爆発弾の振る舞い
//		
// ***************************************************************************************
class ExplosionBehaviour : public IBulletBehaviour
{
private:

public:
    void OnSpawn(RendererEngine& renderer, class Bullet& _bullet) override;
    void OnFlightUpdate(RendererEngine& renderer, class Bullet& _bullet, float _deltaTime)override;
    BulletData::BulletHitResult OnHit(RendererEngine& renderer, class Bullet& _bullet, const class CollisionInfo& _hit)override;
    void OnEnd(class Bullet& _bullet, BulletData::BULLET_END_REASON _reason)override;
    void Reset()override;
};


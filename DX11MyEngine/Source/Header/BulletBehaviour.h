#pragma once
#include "ConstantBulletData.h"
#include "CollisionInfo.h"

namespace BulletBehaviour
{
    BulletData::BulletMoveResult UpdateMove(
        BulletData::BulletRuntime& _runtime,
        const BulletData::CommonBulletData& _common,
        const BulletData::LinearMoveData& _moveData,
        float _deltaTime);

    BulletData::BulletMoveResult UpdateMove(
        BulletData::BulletRuntime& _runtime,
        const BulletData::CommonBulletData& _common,
        const BulletData::HomingMoveData& _moveData,
        float _deltaTime);

    BulletData::BulletHitResult OnHit(
        BulletData::BulletRuntime& _runtime,
        const BulletData::CommonBulletData& _common,
        const BulletData::DirectHitData& _hitData,
        const CollisionInfo& _collision,
        class RendererEngine& _renderer);

    BulletData::BulletHitResult OnHit(
        BulletData::BulletRuntime& runtime,
        const BulletData::CommonBulletData& _common,
        const BulletData::ExplosionHitData& _hitData,
        const CollisionInfo& _collision,
        class RendererEngine& _renderer);
};

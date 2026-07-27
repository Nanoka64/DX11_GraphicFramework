#pragma once
#include "ConstantBulletData.h"
#include "CollisionInfo.h"

namespace BulletBehaviour
{
    BulletData::BulletMoveResult UpdateMove(
        BulletData::RuntimeState& _runtime,
        const BulletData::CommonConfig& _common,
        const BulletData::LinearMovementConfig& _moveData,
        float _deltaTime);

    BulletData::BulletMoveResult UpdateMove(
        BulletData::RuntimeState& _runtime,
        const BulletData::CommonConfig& _common,
        const BulletData::HomingMovementConfig& _moveData,
        float _deltaTime);

    BulletData::BulletVisualResult UpdateVisual(
        BulletData::RuntimeState& _runtime,
        const BulletData::CommonConfig& _common,
        const BulletData::ScaleLerpVisualConfig& _visualData,
        float _deltaTime);

    BulletData::BulletHitResult OnHit(
        BulletData::RuntimeState& _runtime,
        const BulletData::CommonConfig& _common,
        const BulletData::DirectHitConfig& _hitData,
        const CollisionInfo& _collision,
        class RendererEngine& _renderer);

    BulletData::BulletHitResult OnHit(
        BulletData::RuntimeState& runtime,
        const BulletData::CommonConfig& _common,
        const BulletData::ExplosionHitConfig& _hitData,
        const CollisionInfo& _collision,
        class RendererEngine& _renderer);
};

#include "pch.h"
#include "BulletBehaviour.h"

using namespace BulletData;
using namespace GIGA_Engine;
using namespace UtilityData;
using namespace Input;
using namespace VECTOR3;


//*---------------------------------------------------------------------------------------
//*【?】生成時の処理
//*
//* [引数]
//* &_renderer : 描画エンジン
//* &_bullet : 弾クラス
//*
//* [返値] なし
//*----------------------------------------------------------------------------------------
void ExplosionBehaviour::OnSpawn(RendererEngine& renderer, class Bullet& _bullet)
{

}

//*---------------------------------------------------------------------------------------
//*【?】飛行時の更新処理
//*
//* [引数]
//* &_renderer : 描画エンジン
//* &_bullet : 弾クラス
//* _deltaTime : デルタタイム
//*
//* [返値] なし
//*----------------------------------------------------------------------------------------
void ExplosionBehaviour::OnFlightUpdate(RendererEngine& _renderer, class Bullet& _bullet, float _deltaTime)
{

}

//*---------------------------------------------------------------------------------------
//*【?】ヒット時の処理
//*
//* [引数]
//* &_renderer : 描画エンジン
//* &_bullet : 弾クラス
//* &_hit : ヒット情報
//*
//* [返値] ヒットリザルト
//*----------------------------------------------------------------------------------------
BulletData::BulletHitResult ExplosionBehaviour::OnHit(RendererEngine& _renderer, class Bullet& _bullet, const class CollisionInfo& _hit)
{

}

//*---------------------------------------------------------------------------------------
//*【?】終了時の処理
//*
//* [引数]
//* &_bullet : 弾クラス
//* _reason : 終了情報
//*
//* [返値] なし
//*----------------------------------------------------------------------------------------
void ExplosionBehaviour::OnEnd(class Bullet& _bullet, BulletData::BULLET_END_REASON _reason)
{

}

//*---------------------------------------------------------------------------------------
//*【?】パラメータ等のリセット
//*
//* [引数] なし
//* [返値] なし
//*----------------------------------------------------------------------------------------
void ExplosionBehaviour::Reset()
{

}
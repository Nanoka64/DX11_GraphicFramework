#include "pch.h"
#include "Component_Bullet.h"
#include "BulletBehaviour.h"
#include "RendererEngine.h"
#include "Component_Transform.h"
#include "Component_DecalRenderer.h"
#include "Component_TimerDestruction.h"
#include "Component_Health.h"
#include "Component_MoveLogic.h"
#include "GameObjectManager.h"
#include "GameObject.h"
#include "InputFactory.h"
#include "MeshFactory.h"
#include "CollisionInfo.h"
#include "ResourceManager.h"
#include "Component_Collider.h"
#include "Component_Physics.h"

using namespace GIGA_Engine;
using namespace UtilityData;
using namespace Input;
using namespace VECTOR3;

//*---------------------------------------------------------------------------------------
//*【?】コンストラクタ
//* [引数]
//* pOwner : オーナーオブジェクト
//* updateRank : 更新レイヤー
//*----------------------------------------------------------------------------------------
Bullet::Bullet(std::weak_ptr<GameObject> pOwner, int updateRank) :
    IComponent(pOwner, updateRank),
    m_StartPos(VEC3()),
    m_PrevPos(VEC3()),
    m_MoveDir(VEC3()),
    m_pBulletData(nullptr),
    m_pCurrentBehaviour(nullptr),
    m_CrntPenetrationCount(0)
{
    this->set_Tag("Bullet");
}

//*---------------------------------------------------------------------------------------
//*【?】初期化
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//* [返値]なし
//*----------------------------------------------------------------------------------------
void Bullet::Start(RendererEngine& renderer) 
{

}

//*---------------------------------------------------------------------------------------
//*【?】更新
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//* [返値]なし
//*----------------------------------------------------------------------------------------
void Bullet::Update(RendererEngine& renderer) 
{
    auto transform = m_pOwner.lock()->get_Transform().lock();
    VEC3 crntPos = transform->get_VEC3ToPos();
    float deltaTime = Master::m_pTimeManager->get_DeltaTime();
    auto moveComp = m_pOwner.lock()->get_Component<MoveLogic>();

    MoveParam param;
    param._moveDirection = m_MoveDir;
    param._moveSpeed = m_pBulletData->_commonData._speed;
    param._gravity = m_pBulletData->_commonData._gravityScale;

    m_PrevPos = crntPos;    // 前回の位置を更新

    // 移動処理
    moveComp->set_MoveParam(param);	// 移動ロジックにパラメータを渡す
}

//*---------------------------------------------------------------------------------------
//*【?】遅延更新
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//* [返値]なし
//*----------------------------------------------------------------------------------------
void Bullet::LateUpdate(RendererEngine& renderer)
{
    auto transform = m_pOwner.lock()->get_Transform().lock();

    // 移動後の位置
    VEC3 newPos = transform->get_VEC3ToPos();

    // 射程距離外で削除
    float distSq = VEC3::DistanceSq(newPos, m_StartPos);
    float range = m_pBulletData->_commonData._range;
    if (distSq > range * range) {
        m_pOwner.lock()->clear_StatusFlag(OBJECT_STATUS_BITFLAG::IS_ACTIVE);    // ノンアクティブに
    }
    // レイキャストで衝突判定（コライダーの衝突処理をこっちに移動）
    CollInData_Ray ray;
    ray._point = m_PrevPos;           // 前回の位置からレイを飛ばす
    ray._dir = newPos - m_PrevPos;    // 前回の位置から新しい位置へのベクトル
    unsigned mask = m_pBulletData->_commonData._collisionMask;
    CollisionInfo hitInfo;

    if (Master::m_pCollisionManager->CheckRaycast(ray, mask, &hitInfo))
    {
        // 衝突時の処理
        m_pCurrentBehaviour->OnHit(renderer, *this, hitInfo);
    }
}

//*---------------------------------------------------------------------------------------
//*【?】パラメータ等の設定
//*     発射時に呼ぶ 
//* [引数]
//* _pParam : 弾のパラメータ 
//* &_context : 弾の生成に必要な情報
//* [返値]なし
//*----------------------------------------------------------------------------------------
void Bullet::Setup(
    const BulletData::BulletDataBase*_pData,
    const BulletData::BulletSpawnContext& _context)
{
    m_pBulletData = _pData;

    _context._shooter;
    _context._target;
    _context._transform;
}

//*---------------------------------------------------------------------------------------
//*【?】無効にする
//* [引数]
//* _reason  : どんな条件で無効になったか
//* [返値]なし
//*----------------------------------------------------------------------------------------
void Bullet::Deactivate(BulletData::BULLET_END_REASON _reason)
{
    // 終了処理
    m_pCurrentBehaviour->OnEnd(*this, _reason);
}

//*---------------------------------------------------------------------------------------
//*【?】状態のリセット
//* [引数]なし
//* [返値]なし
//*----------------------------------------------------------------------------------------
void Bullet::Reset()
{
    m_CrntPenetrationCount = 0;

    auto moveLogic = m_pOwner.lock()->get_Component<MoveLogic>();
    moveLogic->ParamReset();
}
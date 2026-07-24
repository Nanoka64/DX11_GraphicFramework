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
using namespace BulletData;

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
    m_pMyTransform(nullptr),
    //m_pCurrentBehaviour(nullptr),
    m_CrntPenetrationCount(0),
    m_CrntSpeed(0.0f)
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
    VEC3 crntPos = m_pMyTransform->get_VEC3ToPos();
    float deltaTime = Master::m_pTimeManager->get_DeltaTime();
    auto moveComp = m_pOwner.lock()->get_Component<MoveLogic>();

    MoveParam param;
    param._moveDirection = m_MoveDir;
    param._moveSpeed = m_pBulletData->_commonData._speed;
    param._acceleration = m_pBulletData->_commonData._acceleration;
    param._gravity = m_pBulletData->_commonData._gravityScale;


    // 移動処理
    moveComp->set_MoveParam(param);	// 移動ロジックにパラメータを渡す

    // 前回座標として保持
    m_PrevPos = m_pMyTransform->get_VEC3ToPos();

    //m_Runtime._moveDirection = m_MoveDir;

    //// 移動処理
    //BulletMoveResult move_result = std::visit(
    //    [&](const auto& moveData)
    //    {
    //        return BulletBehaviour::UpdateMove(
    //            m_Runtime,
    //            m_pBulletData->_commonData,
    //            moveData,
    //            deltaTime);
    //    },
    //    m_pBulletData->_moveData);

    //// 新しい座標を求める
    //VEC3 newPos = m_PrevPos + move_result._velocity * deltaTime;

    //m_pMyTransform->set_Pos(newPos);
    //m_pMyTransform->set_RotationQuaternion(move_result._rotation);
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
    // 移動後の位置
    VEC3 newPos = m_pMyTransform->get_VEC3ToPos();

    // 射程距離外で削除
    float distSq = VEC3::DistanceSq(newPos, m_StartPos);
    float range = m_pBulletData->_commonData._range;
    if (distSq > range * range) {
        Deactivate(BULLET_END_REASON::OUT_OF_RANGE);
        return;
    }
    // レイキャストで衝突判定（コライダーの衝突処理をこっちに移動）
    CollInData_Ray ray;
    ray._point = m_PrevPos;           // 前回の位置からレイを飛ばす
    ray._dir = newPos - m_PrevPos;    // 前回の位置から新しい位置へのベクトル
    unsigned mask = m_pBulletData->_commonData._collisionMask;
    CollisionInfo hitInfo;
    if (Master::m_pCollisionManager->CheckRaycast(ray, mask, &hitInfo))
    {
        m_pMyTransform->set_Pos(hitInfo.get_HitPoint());

        // トランスフォームの設定
        m_Runtime._transform = m_pMyTransform;

        // 衝突時の処理
        BulletHitResult result = std::visit(
            [&](const auto& hitData)
            {
                return BulletBehaviour::OnHit(
                    m_Runtime,
                    m_pBulletData->_commonData,
                    hitData,
                    hitInfo,
                    renderer);
            },
            m_pBulletData->_hitData);

        // 弾を無効にする
        if (result._deactivate)
        {
            Deactivate(BULLET_END_REASON::HIT);
        }
        // 貫通数増やす
        else if (result._penetration)
        {
            m_CrntPenetrationCount++;
        }

        auto obj = hitInfo.get_HitObject().lock();

        // 吹っ飛び
        if (auto physics = obj->get_Component<Physics>())
        {
            auto targetTransform = obj->get_Transform().lock();
            VEC3 targetPos = targetTransform->get_VEC3ToPos();
            VECTOR3::VEC3 knockbackDir = m_MoveDir;     // 移動ベクトルをそのまま衝撃のベクトルにする

            // 衝撃ベクトルの設定
            physics->AddImpulse(knockbackDir * m_pBulletData->_commonData._knockbackForce);
        }
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

    // トランスフォームのポインタを取得
    m_pMyTransform = m_pOwner.lock()->get_Transform().lock().get();

    // 開始位置
    m_StartPos = m_pMyTransform->get_VEC3ToPos();
    m_PrevPos = m_StartPos;
    
    // 前方向ベクトル
    m_MoveDir = m_pMyTransform->get_WorldForward().Normalize();

    m_CrntSpeed = _pData->_commonData._speed;
    m_CrntPenetrationCount = 0;

    m_Runtime._currentSpeed = m_CrntSpeed;
}

//*---------------------------------------------------------------------------------------
//*【?】無効にする
//* [引数]
//* _reason  : どんな条件で無効になったか
//* [返値]なし
//*----------------------------------------------------------------------------------------
void Bullet::Deactivate(BulletData::BULLET_END_REASON _reason)
{
    switch (_reason)
    {
    case BulletData::BULLET_END_REASON::HIT:
        break;
    case BulletData::BULLET_END_REASON::OUT_OF_RANGE:
        break;
    case BulletData::BULLET_END_REASON::CANCELLED:
        break;
    default:
        break;
    }


    // 終了処理
    //m_pCurrentBehaviour->OnEnd(*this, _reason);

    m_pOwner.lock()->clear_StatusFlag(OBJECT_STATUS_BITFLAG::IS_ACTIVE);    // ノンアクティブに
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
#include "pch.h"
#include "Component_NormalBullet.h"
#include "Component_AssultRifle.h"
#include "Component_Transform.h"
#include "Component_DecalRenderer.h"
#include "Component_TimerDestruction.h"
#include "Component_Health.h"
#include "Component_MoveLogic.h"
#include "RendererEngine.h"
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

constexpr float DECAL_SIZE_FACTOR        = 7.0f;   // デカールの大きさの補正値（transformのスケールだと小さすぎるため）
constexpr float DECAL_Z_AXIS_SIZE_FACTOR = 0.0f;   // デカールの奥行に加算する補正値
constexpr float DECAL_LIFE_TIME          = 5.0f;   // デカールの生存時間
constexpr float EFFECTL_SIZE_FACTOR      = 2.0f;   // エフェクトの大きさの補正値（transformのスケールだと小さすぎるため）

//*---------------------------------------------------------------------------------------
//*【?】コンストラクタ
//* [引数]
//* pOwner : オーナーオブジェクト
//* updateRank : 更新レイヤー
//*----------------------------------------------------------------------------------------
NormalBullet::NormalBullet(std::weak_ptr<GameObject> pOwner, int updateRank)
    : BulletBase(pOwner, updateRank)
{
    this->set_Tag("Bullet_Normal");
}



//*---------------------------------------------------------------------------------------
//*【?】デストラクタ
//*----------------------------------------------------------------------------------------
NormalBullet::~NormalBullet()
{

}


//*---------------------------------------------------------------------------------------
//*【?】初期化
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//* [返値]なし
//*----------------------------------------------------------------------------------------
void NormalBullet::Start(RendererEngine& renderer)
{
    //////////////////////////////////////////////////////////////////////////////////////////
    //
    //						衝突時処理の設定
    // 
    //////////////////////////////////////////////////////////////////////////////////////////
    m_CollisionTask =
        [this, &renderer](const class CollisionInfo& _other)
        {
            //*****************************************************************************************
            //						衝突した際の処理
            //*****************************************************************************************
            auto hitObj = _other.get_HitObject().lock();
            if (!hitObj) return;
            COLLISION_CATEGORY hitCategory = _other.get_HitCollider().lock()->get_CollisionCategory();

            // 相手がHealthComponentを持っているか確認（破壊可能な建物は壊せないように）
            auto health = hitObj->get_Component<Health>();
            if (health && hitCategory != COLLISION_CATEGORY::DESTRUCTION_BUILDING)
            {
                // 弾が保持しているダメージ値を渡す
                health->TakeDamage(m_pWeaponData->_damage);
            }

            // 建物に当たったら即消えるようにして、その他は貫通数を減らす
            if (hitCategory == COLLISION_CATEGORY::BUILDING || hitCategory == COLLISION_CATEGORY::DESTRUCTION_BUILDING)
            {
                m_pOwner.lock()->clear_StatusFlag(OBJECT_STATUS_BITFLAG::IS_ACTIVE);    // ノンアクティブに
            }
            else
            {
                m_CrntPenetrationCount++; // 貫通数を増やす

                if (m_CrntPenetrationCount >= m_pWeaponData->_penetrationsCount)
                {
                    m_pOwner.lock()->clear_StatusFlag(OBJECT_STATUS_BITFLAG::IS_ACTIVE);    // ノンアクティブに
                }
            }

            auto transform = m_pOwner.lock()->get_Transform().lock();
            VEC3 pos = transform->get_VEC3ToPos();


            VEC3 hitNormal = _other.get_HitNormal();    // 衝突相手の法線

            // 水平方向の向きを求める
            float yaw = atan2(hitNormal.x, hitNormal.z);
            // 水平成分の長さ
            float xzLen = sqrtf(hitNormal.x * hitNormal.x + hitNormal.z * hitNormal.z);
            // 垂直方向の角度を求める
            // 法線の逆を向かせたいのでマイナスを付ける
            float pitch = atan2(-hitNormal.y, xzLen);

            //*****************************************************************************************
            //						デカールの生成
            //*****************************************************************************************
            if (m_pWeaponData->_decalMaterialTag.empty() == false)
            {
                auto matPtr = Master::m_pResourceManager->FindMaterial(m_pWeaponData->_decalMaterialTag);

                SetupMaterialInfo matInfo[1];
                matInfo[0].Index = 0;
                matInfo[0].pMaterialData = matPtr;

                CreateDecalInfo decal;
                decal.pRenderer = &renderer;
                decal.Type = UTILITY_MESH_TYPE::CUBE;
                decal.MatNum = 1;
                decal.MaterialData = matInfo;
                decal.IsActive = false;
                decal.ShaderType = SHADER_TYPE::DEFERRED_STD_DECAL;
                decal.IsNormalMap = false;
                decal.IsDynamic = true;


                // 当たった方向に伸ばそうとしたけど上手くいかなかった
                {
                    //VEC3 moveVel = -m_MoveDir.Normalize();
                    //// 法線と弾の移動ベクトルの内積
                    //float dot_HitNormToMoveVel = abs(VEC3::Dot(hitNormal, moveVel));

                    //// 地面を這うベクトルを求める
                    //VEC3 tempVec = moveVel - (hitNormal * dot_HitNormToMoveVel);
                    //tempVec = tempVec.Normalize();

                    //VEC3 baseUp(0.0f, 1.0f, 0.0f);
                    //// 法線が真上を向いているか確かめる
                    //if (abs(hitNormal.y) > 0.99f) {
                    //    baseUp = VEC3(1.0f, 0.0f, 0.0f); // 軸をズラす
                    //}

                    //VEC3 side = VEC3::Cross(baseUp, hitNormal).Normalize();
                    //VEC3 up = VEC3::Cross(hitNormal, side);

                    //float dotSide = VEC3::Dot(tempVec, side);
                    //float dotUp = VEC3::Dot(tempVec, up);

                    //float angleZ = atan2f(dotSide, dotUp);
                    //scale.y = 20.0f + (1.0f - dot_HitNormToMoveVel) * 120.0f;
                }

                float angleZ = Tool::RandRange(0.0f, 6.14f);

                VEC3 scale = transform->get_VEC3ToScale();
                scale *= DECAL_SIZE_FACTOR;
                scale.z += DECAL_Z_AXIS_SIZE_FACTOR;


                auto obj = MeshFactory::CreateDecal(decal);
                obj->get_Component<DecalRenderer>()->Start(renderer);
                obj->get_Transform().lock()->set_Pos(pos);
                obj->get_Transform().lock()->set_Scale(scale);
                obj->get_Transform().lock()->set_RotateToRad(pitch, yaw, angleZ);
                obj->set_Tag("BulletHole");
                auto timer = obj->add_Component<TimerDestruction>();
                timer->set_LifeTime(DECAL_LIFE_TIME);  // 生存時間
            }

            //*****************************************************************************************
            //						ヒットエフェクトの再生
            //*****************************************************************************************
            if (m_pWeaponData->_hitEffectTag.empty() == false)
            {
                VEC3 effectRot = VEC3(pitch, yaw, 0.0f);
                int effectHandle = Master::m_pEffectManager->PlayEffect(m_pWeaponData->_hitEffectTag);

                VEC3 effectScale = transform->get_VEC3ToScale();
                effectScale *= EFFECTL_SIZE_FACTOR;     // 大きさの補正

                // エフェクトパラメータ
                Master::m_pEffectManager->SetScaleEffect(effectHandle, effectScale.x, effectScale.y, effectScale.z);
                Master::m_pEffectManager->SetPositionEffect(effectHandle, pos.x, pos.y, pos.z);
                Master::m_pEffectManager->SetRotationEffect(effectHandle, effectRot.x, effectRot.y, effectRot.z);
            }
        };
}


//*---------------------------------------------------------------------------------------
//*【?】更新
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//* [返値]なし
//*----------------------------------------------------------------------------------------
void NormalBullet::Update(RendererEngine &renderer)
{
    auto transform = m_pOwner.lock()->get_Transform().lock();
    VEC3 crntPos = transform->get_VEC3ToPos();
    float deltaTime = Master::m_pTimeManager->get_DeltaTime();
    auto moveComp = m_pOwner.lock()->get_Component<MoveLogic>();

    MoveParam param;
    param._moveDirection = m_MoveDir;
    param._moveSpeed = m_pWeaponData->_speed;
    param._gravity = m_pWeaponData->_gravityScale;

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
void NormalBullet::LateUpdate(RendererEngine& renderer)
{
    auto transform = m_pOwner.lock()->get_Transform().lock();
    
    // 移動後の位置
    VEC3 newPos = transform->get_VEC3ToPos();

    // 射程距離外で削除
    float distSq = VEC3::DistanceSq(newPos, m_StartPos);
    if (distSq > m_pWeaponData->_range * m_pWeaponData->_range) {
        m_pOwner.lock()->clear_StatusFlag(OBJECT_STATUS_BITFLAG::IS_ACTIVE);    // ノンアクティブに
    }
    // レイキャストで衝突判定（コライダーの衝突処理をこっちに移動）
    CollInData_Ray ray;
	ray._point = m_PrevPos;           // 前回の位置からレイを飛ばす
    ray._dir = newPos - m_PrevPos;    // 前回の位置から新しい位置へのベクトル
    unsigned mask = m_pWeaponData->_collisionMask;
    CollisionInfo hitInfo;

    if (Master::m_pCollisionManager->CheckRaycast(ray, mask, &hitInfo))
    {
        auto owner = m_pOwner.lock();
        auto transform = owner->get_Transform().lock();
        transform->set_Pos(hitInfo.get_HitPoint());     // 衝突位置に合わせる

        // 衝突時の処理
        if (m_CollisionTask)
        {
            m_CollisionTask(hitInfo);
        }

        VEC3 pos = transform->get_VEC3ToPos();

        auto obj = hitInfo.get_HitObject().lock();

        // 吹っ飛び
        if (auto physics = obj->get_Component<Physics>())
        {
            auto targetTransform = obj->get_Transform().lock();
            VEC3 targetPos = targetTransform->get_VEC3ToPos();
            VECTOR3::VEC3 knockbackDir = m_MoveDir;     // 移動ベクトルをそのまま衝撃のベクトルにする

            // 衝撃ベクトルの設定
            physics->AddImpulse(knockbackDir * m_pWeaponData->_knockbackForce);
        }


    }
}


//*---------------------------------------------------------------------------------------
//*【?】衝突処理
//* 
//* [引数]
// * &other : 衝突情報
//* [返値]なし
//*----------------------------------------------------------------------------------------
void NormalBullet::OnCollisionEnter(const class CollisionInfo& _other)
{
 //   if (m_CollisionTask)
 //   {
 //       m_CollisionTask(_other);
 //   }

 //   auto hitObj = _other.get_HitObject().lock();
 //   if (!hitObj) return;

 //   // 相手がHealthComponentを持っているか確認
 //   auto health = hitObj->get_Component<Health>();
 //   if (health)
 //   {
 //       // 弾が保持しているダメージ値を渡す
 //       health->TakeDamage(m_Parameter._damage);
 //   }

	//// 建物に当たったら即消えるようにして、その他は貫通数を減らす
 //   if (_other.get_HitCollider().lock()->get_CollisionCategory() == COLLISION_CATEGORY::BUILDING)
 //   {
 //       m_pOwner.lock()->clear_StatusFlag(OBJECT_STATUS_BITFLAG::IS_ACTIVE);    // ノンアクティブに
 //   }
 //   else 
 //   {
 //       m_CrntPenetrationCount++; // 貫通数を増やす
 //       
 //       if (m_CrntPenetrationCount >= m_Parameter._penetrationsCount) 
 //       {
 //           m_pOwner.lock()->clear_StatusFlag(OBJECT_STATUS_BITFLAG::IS_ACTIVE);    // ノンアクティブに
 //       }
 //   }
}


//*---------------------------------------------------------------------------------------
//*【?】パラメータ等の設定
//*     発射時に呼ぶ 
//* [引数]
//* _pParam : 弾のパラメータ 
//* [返値]なし
//*----------------------------------------------------------------------------------------
void NormalBullet::Setup(const BulletData::NormalBulletData* _pParam)
{
    m_pWeaponData = _pParam;

    auto transform = m_pOwner.lock()->get_Transform().lock();

    // 開始位置
    m_StartPos = transform->get_VEC3ToPos();
    m_PrevPos = m_StartPos;

    m_MoveDir = transform->get_WorldForward(); // 前方向ベクトル

    //m_MoveDir.x += Tool::RandRange(-0.03f, 0.03f);
    //m_MoveDir.y += Tool::RandRange(-0.03f, 0.03f);
    //m_MoveDir.z += Tool::RandRange(-0.03f, 0.03f);

    m_MoveDir = m_MoveDir.Normalize();
}


//*---------------------------------------------------------------------------------------
//*【?】状態のリセット
//* [引数]なし
//* [返値]なし
//*----------------------------------------------------------------------------------------
void NormalBullet::Reset()
{
    //m_pWeaponData->Reset();

    m_CrntPenetrationCount = 0;

    auto moveLogic = m_pOwner.lock()->get_Component<MoveLogic>();
    moveLogic->ParamReset();
}

//*---------------------------------------------------------------------------------------
//*【?】パラメータの取得
//* [引数]なし
//* [返値]
//* 爆発弾のデータ 
//*----------------------------------------------------------------------------------------
const BulletData::NormalBulletData* NormalBullet::get_Parameter()const
{
    return static_cast<const BulletData::NormalBulletData*>(m_pWeaponData);
}
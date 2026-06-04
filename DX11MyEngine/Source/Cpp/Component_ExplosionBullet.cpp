#include "pch.h" 
#include "Component_ExplosionBullet.h"
#include "Component_MoveLogic.h"
#include "Component_TimerDestruction.h"
#include "Component_DecalRenderer.h"
#include "Component_SphereCollider.h"
#include "Component_Health.h"
#include "Component_3DCamera.h"
#include "Component_Physics.h"
#include "RendererEngine.h"

#include "GameObject.h"
#include "InputFactory.h"
#include "MeshFactory.h"
#include "CollisionInfo.h"
#include "ResourceManager.h"

constexpr float DECAL_SIZE_FACTOR        = 100.0f;   // デカールの大きさの補正値
constexpr float DECAL_Z_AXIS_SIZE_FACTOR = 0.0f;     // デカールの奥行に加算する補正値
constexpr float DECAL_LIFE_TIME = 1000.0f;             // デカールの生存時間
constexpr float SHAKE_MAX_RANGE_EXPLOSION_SCALE_FACTOR = 15.0f; // カメラシェイク時、シェイクの最大距離を求める際に掛ける補正値
constexpr float SHAKE_LENGTH_SCALE_FACTOR = 0.007f;             // カメラシェイク時、シェイクの大きさを求める際に掛ける補正値
constexpr float SHAKEDURATION = 1.0f;                           // カメラシェイクの持続時間
constexpr float EFFECT_SCALE_FACTOR = 0.12f;            // そのままだと、エフェクトが大きすぎるので

using namespace UtilityData;
using namespace VECTOR3;

//*---------------------------------------------------------------------------------------
//*【?】コンストラクタ
//* [引数]
//* pOwner : オーナーオブジェクト
//* updateRank : 更新レイヤー
//*----------------------------------------------------------------------------------------
ExplosionBullet::ExplosionBullet(std::weak_ptr<GameObject> pOwner, int updateRank) : 
    BulletBase(pOwner, updateRank)
{
	this->set_Tag("Bullet_Explosion");
}


//*---------------------------------------------------------------------------------------
//*【?】デストラクタ
//*----------------------------------------------------------------------------------------
ExplosionBullet::~ExplosionBullet()
{

}


//*---------------------------------------------------------------------------------------
//*【?】初期化
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//* [返値]なし
//*----------------------------------------------------------------------------------------
void ExplosionBullet::Start(RendererEngine& renderer)
{
    //////////////////////////////////////////////////////////////////////////////////////////
    //
    //						衝突時処理の設定
    // 
    //////////////////////////////////////////////////////////////////////////////////////////
    m_CollisionTask =
        [this, &renderer](const class CollisionInfo& _other)
        {
            auto transform = m_pOwner.lock()->get_Transform().lock();
            VEC3 pos = transform->get_VEC3ToPos();
            
            auto bulletParam = this->get_ExplosionParameter();

            // ****************************************************
            //				 カメラシェイク
            // ****************************************************
            std::shared_ptr<Camera3D> camera;
            if (camera = Master::m_pDataManager->get_CameraComponent().lock())
            {
                float maxRange = bulletParam->_explosionRadius * SHAKE_MAX_RANGE_EXPLOSION_SCALE_FACTOR;
                float shaleLength = bulletParam->_explosionRadius * SHAKE_LENGTH_SCALE_FACTOR;
                camera->DistanceDecay(SHAKEDURATION, VEC3(shaleLength), pos, maxRange);
            }


            // ****************************************************
            //				 爆発音再生
            // ****************************************************
            Master::m_pSoundManager->Play_3D(SOUND_TYPE::SE, SOUND_ID_TO_INT(SOUND_ID::EXPLOSION01), pos, 1500.0f);


            // デカールの作成
            auto matPtr = Master::m_pResourceManager->FindMaterial(bulletParam->_decalMaterialTag);

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

            VEC3 hitNormal = _other.get_HitNormal();    // 衝突相手の法線

            // 水平方向の向きを求める
            float angleY = atan2(hitNormal.x, hitNormal.z);
            // 水平成分の長さ
            float xzLen = sqrtf(hitNormal.x * hitNormal.x + hitNormal.z * hitNormal.z);
            // 垂直方向の角度を求める
            // 法線の逆を向かせたいのでマイナスを付ける
            float angleX = atan2(-hitNormal.y, xzLen);
            float angleZ = Tool::RandRange(0.0f, 6.14f);

            float expSize = bulletParam->_explosionRadius * 2.0f;   // 元が半径なので2倍に
            VEC3 scale;
            scale.x = expSize;
            scale.y = expSize;
            scale.z = expSize;

            //auto obj = MeshFactory::CreateDecal(decal);
            //obj->get_Component<DecalRenderer>()->Start(renderer);
            //obj->get_Transform().lock()->set_Pos(pos);
            //obj->get_Transform().lock()->set_Scale(scale);
            //obj->get_Transform().lock()->set_RotateToRad(angleX, angleY, angleZ);
            //obj->set_Tag("BulletHole");
            //auto timer = obj->add_Component<TimerDestruction>();
            //timer->set_LifeTime(DECAL_LIFE_TIME);  // 生存時間

            // エフェクト
            VEC3 effectRot = VEC3(abs(angleX - 0.05f), angleY, 0.0f);
            int exp_handle = Master::m_pEffectManager->PlayEffect(bulletParam->_explosionEffectHandleTag);   // 爆発

            float effectExpSize = expSize * EFFECT_SCALE_FACTOR;   // 爆発半径（そのままだと大きすぎるので補正）
            VEC3 expRot = VEC3(
                Master::m_pRandomManager->GetFloatRandom(0.0f, 3.14f), 
                Master::m_pRandomManager->GetFloatRandom(0.0f, 3.14f), 
                Master::m_pRandomManager->GetFloatRandom(0.0f, 3.14f)
            );


            if (bulletParam->_isSmoke)
            {
                int exp_smoke_handle = Master::m_pEffectManager->PlayEffect("Explosion_Smoke_01");   // 煙

                // 爆発煙
                Master::m_pEffectManager->SetScaleEffect(exp_smoke_handle, effectExpSize, effectExpSize, effectExpSize);
                Master::m_pEffectManager->SetPositionEffect(exp_smoke_handle, pos.x, pos.y, pos.z);
                Master::m_pEffectManager->SetRotationEffect(exp_smoke_handle, expRot.x, expRot.y, expRot.z);

                Master::m_pEffectManager->SetDynamicParameter(exp_smoke_handle, 1, bulletParam->_explosionEffectAliveTime); // 生存時間を変更
            }

            // 爆発
            Master::m_pEffectManager->SetScaleEffect(exp_handle, effectExpSize, effectExpSize, effectExpSize);
            Master::m_pEffectManager->SetPositionEffect(exp_handle, pos.x, pos.y, pos.z);
            Master::m_pEffectManager->SetRotationEffect(exp_handle, expRot.x, expRot.y, expRot.z);
            // 動的パラメータの設定
            Master::m_pEffectManager->SetDynamicParameter(exp_handle, 1, bulletParam->_explosionEffectAliveTime); // 生存時間を変更

        };
}


//*---------------------------------------------------------------------------------------
//*【?】更新
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//* [返値]なし
//*----------------------------------------------------------------------------------------
void ExplosionBullet::Update(RendererEngine &renderer)
{
    auto transform = m_pOwner.lock()->get_Transform().lock();
    VEC3 crntPos = transform->get_VEC3ToPos();
    auto moveComp = m_pOwner.lock()->get_Component<MoveLogic>();
    float moveDistance = VEC3::Distance(crntPos, m_PrevPos);
    float deltaTime = Master::m_pTimeManager->get_DeltaTime();

    // 爆発弾のパラメータ
    auto bulletParam = get_ExplosionParameter();

    MoveParam param;
    param._moveDirection = m_MoveDir;// ※マイナスにしているのはプレイヤーの方向がおかしいせい（後で直す）
    param._moveSpeed = bulletParam->_speed;
    param._gravity = bulletParam->_gravityScale;

    // 前回の位置として保持
    m_PrevPos = crntPos;  

    moveComp->set_MoveParam(param);	// 移動ロジックにパラメータを渡す
}


//*---------------------------------------------------------------------------------------
//*【?】遅延更新
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//* [返値]なし
//*----------------------------------------------------------------------------------------
void ExplosionBullet::LateUpdate(RendererEngine& renderer)
{
    auto transform = m_pOwner.lock()->get_Transform().lock();
    auto bulletParam = get_ExplosionParameter();

    // 移動後の位置
    VEC3 newPos = transform->get_VEC3ToPos();

    m_SmokeSpwanTimer += Master::m_pTimeManager->get_DeltaTime();

    if (m_SmokeSpwanTimer >= 0.05f)
    {
        int smoke_handle = Master::m_pEffectManager->PlayEffect("Smoke_03");   // 煙
        float smokeSize = 1.0f;

        // 爆発煙
        Master::m_pEffectManager->SetScaleEffect(smoke_handle, smokeSize, smokeSize, smokeSize);
        Master::m_pEffectManager->SetPositionEffect(smoke_handle, newPos.x, newPos.y, newPos.z);
        m_SmokeSpwanTimer = 0.0f;
    }


    // 射程距離外で削除
    float distSq = VEC3::DistanceSq(newPos, m_StartPos);
    if (distSq > bulletParam->_range * bulletParam->_range) {
        m_pOwner.lock()->clear_StatusFlag(OBJECT_STATUS_BITFLAG::IS_ACTIVE);    // ノンアクティブに
    }

    // レイキャストで衝突判定
    CollInData_Ray ray;
	ray._point = m_PrevPos;           // 前回の位置からレイを飛ばす
    ray._dir = newPos - m_PrevPos;    // 前回の位置から新しい位置へのベクトル
    unsigned mask = bulletParam->_collisionMask;
    CollisionInfo hitInfo;

    if (Master::m_pCollisionManager->CheckRaycast(ray, mask, &hitInfo))
    {
        auto owner = m_pOwner.lock();
        auto transform = owner->get_Transform().lock();
        transform->set_Pos(m_PrevPos + (hitInfo.get_HitNormal() * 1.0f));     // 衝突位置に合わせる

        // 衝突処理
        if (m_CollisionTask)
        {
            m_CollisionTask(hitInfo);
        }

        if (m_pOwner.expired())return;

        VEC3 pos = transform->get_VEC3ToPos();

        unsigned mask = bulletParam->_collisionMask;;

        // 範囲内チェック
        auto targets = Master::m_pCollisionManager->CheckSphere(pos, bulletParam->_explosionRadius, mask);

        // 範囲内の全員にダメージ
        for (auto& target : targets) {

            if (auto obj = target->get_OwnerObj().lock())
            {
                // ダメージ
                if (auto health = obj->get_Component<Health>())
                {
                    health->TakeDamage(bulletParam->_damage);
                }

                // 吹っ飛び
                if (auto physics = obj->get_Component<Physics>())
                {
                    auto targetTransform = obj->get_Transform().lock();
                    VEC3 targetPos = targetTransform->get_VEC3ToPos();

                    VECTOR3::VEC3 knockbackDir = targetPos - pos;   // 爆心地から外（衝突オブジェクト）へ向かうベクトル
                    knockbackDir = knockbackDir.Normalize();

                    // 少し上（Y軸）に向かせることで、綺麗な放物線を描いて吹き飛ぶ
                    knockbackDir.y += 0.5f;
                    knockbackDir.x += Master::m_pRandomManager->GetFloatRandom(-0.5f, 0.5f);
                    knockbackDir.z += Master::m_pRandomManager->GetFloatRandom(-0.5f, 0.5f);

                    knockbackDir = knockbackDir.Normalize();

                    // 衝撃ベクトルの設定
                    physics->AddImpulse(knockbackDir * m_pWeaponData->_knockbackForce);

                    knockbackDir.y = Master::m_pRandomManager->GetFloatRandom(-1, 1);
                    knockbackDir.x = Master::m_pRandomManager->GetFloatRandom(-1, 1);
                    knockbackDir.z = Master::m_pRandomManager->GetFloatRandom(-1, 1);
                    physics->AddAngularImpulse(knockbackDir * m_pWeaponData->_knockbackForce);
                }
            }
        }
        m_pOwner.lock()->clear_StatusFlag(OBJECT_STATUS_BITFLAG::IS_ACTIVE);
    }
}

//*---------------------------------------------------------------------------------------
//*【?】トリガー衝突
//*     
//* [引数]
//*  &other : 衝突相手の情報
//* [返値]なし
//*----------------------------------------------------------------------------------------
void ExplosionBullet::OnTriggerEnter(const class CollisionInfo &_other)
{

}


//*---------------------------------------------------------------------------------------
//*【?】パラメータの取得
//* [引数]なし
//* [返値]
//* 爆発弾のデータ 
//*----------------------------------------------------------------------------------------
const BulletData::ExplosionBulletData* ExplosionBullet::get_ExplosionParameter()const
{
    return static_cast<const BulletData::ExplosionBulletData*>(m_pWeaponData);
}


//*---------------------------------------------------------------------------------------
//*【?】パラメータ等の設定
//*     発射時に呼ぶ 
//* [引数]なし
//* [返値]なし
//*----------------------------------------------------------------------------------------
void ExplosionBullet::Setup(const BulletData::NormalBulletData* _pParam)
{
    if (m_pOwner.expired())return;
    auto owner = m_pOwner.lock();
    auto transform = owner->get_Transform().lock();

    m_pWeaponData = static_cast<const BulletData::ExplosionBulletData*>(_pParam);

    // 開始位置
    m_StartPos = transform->get_VEC3ToPos();
    m_PrevPos = m_StartPos;

    m_MoveDir = transform->get_Forward(); // 前方向ベクトル

    m_MoveDir = m_MoveDir.Normalize();
}

//*---------------------------------------------------------------------------------------
//*【?】状態のリセット
//* [引数]なし
//* [返値]なし
//*----------------------------------------------------------------------------------------
void ExplosionBullet::Reset()
{
    //auto transform = m_pOwner.lock()->get_Transform().lock();
    //VEC3 pos = transform->get_VEC3ToPos();

    //// エフェクト
    //VEC3 effectRot = VEC3();
    //int exp_handle = Master::m_pEffectManager->PlayEffect(m_Parameter._explosionEffectHandleTag);   // 爆発
    //int exp_smoke_handle = Master::m_pEffectManager->PlayEffect("Explosion_Smoke_01");   // 煙

    //float expSize = m_Parameter._explosionRadius;   // 爆発半径
    //VEC3 expRot = VEC3(Tool::RandRange(0.0f, 3.14f), Tool::RandRange(0.0f, 3.14f), Tool::RandRange(0.0f, 3.14f));

    //// 爆発
    //Master::m_pEffectManager->SetScaleEffect(exp_handle, expSize, expSize, expSize);
    //Master::m_pEffectManager->SetPositionEffect(exp_handle, pos.x, pos.y, pos.z);
    //Master::m_pEffectManager->SetRotationEffect(exp_handle, expRot.x, expRot.y, expRot.z);

    //// 爆発煙
    //Master::m_pEffectManager->SetScaleEffect(exp_smoke_handle, expSize, expSize, expSize);
    //Master::m_pEffectManager->SetPositionEffect(exp_smoke_handle, pos.x, pos.y, pos.z);
    //Master::m_pEffectManager->SetRotationEffect(exp_smoke_handle, expRot.x, expRot.y, expRot.z);


    auto moveLogic = m_pOwner.lock()->get_Component<MoveLogic>();
    moveLogic->ParamReset();

    //m_Parameter.Reset();
}
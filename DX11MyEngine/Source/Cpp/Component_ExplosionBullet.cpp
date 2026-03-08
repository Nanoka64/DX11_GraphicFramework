#include "pch.h"
#include "Component_ExplosionBullet.h"
#include "Component_MoveLogic.h"
#include "Component_TimerDestruction.h"
#include "Component_DecalRenderer.h"
#include "Component_SphereCollider.h"
#include "Component_Health.h"
#include "RendererEngine.h"

#include "GameObject.h"
#include "InputFactory.h"
#include "MeshFactory.h"
#include "CollisionInfo.h"
#include "ResourceManager.h"

constexpr float DECAL_SIZE_FACTOR        = 100.0f;   // デカールの大きさの補正値
constexpr float DECAL_Z_AXIS_SIZE_FACTOR = 40.0f;    // デカールの奥行に加算する補正値
constexpr float DECAL_LIFE_TIME = 10.0f;             // デカールの生存時間
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
void ExplosionBullet::Start(RendererEngine &renderer)
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

            // ****************************************************
            //				 爆発音再生
            // ****************************************************
            Master::m_pSoundManager->Play_3D(SOUND_TYPE::SE, SOUND_ID_TO_INT(SOUND_ID::EXPLOSION01), pos, 1500.0f);


            // デカールの作成
            auto matPtr = Master::m_pResourceManager->FindMaterial("Decal_BulletHole");

            SetupMaterialInfo matInfo[1];
            matInfo[0].Index = 0;
            matInfo[0].pMaterialData = matPtr;

            CreateDecalInfo decal;
            decal.pRenderer = &renderer;
            decal.Type = UTILITY_MESH_TYPE::CUBU;
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

            VEC3 scale;
            scale.x = m_Parameter._explosionRadius;
            scale.y = m_Parameter._explosionRadius;
            scale.z = m_Parameter._explosionRadius + DECAL_Z_AXIS_SIZE_FACTOR;

            auto obj = MeshFactory::CreateDecal(decal);
            obj->get_Component<DecalRenderer>()->Start(renderer);
            obj->get_Transform().lock()->set_Pos(pos);
            obj->get_Transform().lock()->set_Scale(scale + DECAL_SIZE_FACTOR);
            obj->get_Transform().lock()->set_RotateToRad(angleX, angleY, angleZ);
            obj->set_Tag("BulletHole");
            auto timer = obj->add_Component<TimerDestruction>();
            timer->set_LifeTime(10.0f);  // 生存時間

            // エフェクト
            VEC3 effectRot = VEC3(abs(angleX - 0.05f), angleY, 0.0f);
            int exp_handle = Master::m_pEffectManager->PlayEffect(m_Parameter._explosionEffectHandleTag);   // 爆発
            int exp_smoke_handle = Master::m_pEffectManager->PlayEffect("Explosion_Smoke_01");   // 煙

            float expSize = m_Parameter._explosionRadius;   // 爆発半径
            VEC3 expRot = VEC3(Tool::RandRange(0.0f, 3.14f), Tool::RandRange(0.0f, 3.14f), Tool::RandRange(0.0f, 3.14f));

            // 爆発
            Master::m_pEffectManager->SetScaleEffect(exp_handle, expSize, expSize, expSize);
            Master::m_pEffectManager->SetPositionEffect(exp_handle, pos.x, pos.y, pos.z);
            Master::m_pEffectManager->SetRotationEffect(exp_handle, expRot.x, expRot.y, expRot.z);

            // 爆発煙
            Master::m_pEffectManager->SetScaleEffect(exp_smoke_handle, expSize, expSize, expSize);
            Master::m_pEffectManager->SetPositionEffect(exp_smoke_handle, pos.x, pos.y, pos.z);
            Master::m_pEffectManager->SetRotationEffect(exp_smoke_handle, expRot.x, expRot.y, expRot.z);

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

    MoveParam param;
    param._moveDirection = -m_MoveDir;// ※マイナスにしているのはプレイヤーの方向がおかしいせい（後で直す）
    param._moveSpeed = m_Parameter._speed;
    moveComp->Calculate(param);

    m_PrevPos = crntPos;


    // 射程距離外で削除
    float distSq = VEC3::DistanceSq(crntPos, m_StartPos);
    if (distSq > m_Parameter._range * m_Parameter._range) {
        m_pOwner.lock()->clear_StatusFlag(OBJECT_STATUS_BITFLAG::IS_ACTIVE);    // ノンアクティブに
    }
}


//*---------------------------------------------------------------------------------------
//*【?】トリガー衝突
//*     
//* [引数]
//*  &other : 衝突相手の情報
//* [返値]なし
//*----------------------------------------------------------------------------------------
void ExplosionBullet::OnTriggerEnter(const class CollisionInfo &other)
{
    if (m_CollisionTask)
    {
        m_CollisionTask(other);
    }    
    
    if (m_pOwner.expired())return;

    auto owner = m_pOwner.lock();
    auto transform = owner->get_Transform().lock();
    VEC3 pos = transform->get_VEC3ToPos();

    unsigned mask = UINT_CAST(COLLISION_CATEGORY::ENEMY);   // 敵のみ
    
    // 範囲内チェック
    auto targets = Master::m_pCollisionManager->CheckSphere(pos, m_Parameter._explosionRadius * 2, mask);

    // 範囲内の全員にダメージ
    for (auto target : targets) {
        auto obj = target->get_OwnerObj().lock();
        obj->get_Component <Health>()->TakeDamage(100.0f);
    }

    m_pOwner.lock()->clear_StatusFlag(OBJECT_STATUS_BITFLAG::IS_ACTIVE);
}


//*---------------------------------------------------------------------------------------
//*【?】パラメータ等の設定
//*     発射時に呼ぶ 
//* [引数]なし
//* [返値]なし
//*----------------------------------------------------------------------------------------
void ExplosionBullet::Setup()
{
    if (m_pOwner.expired())return;
    auto owner = m_pOwner.lock();
    auto transform = owner->get_Transform().lock();

    // 開始位置
    m_StartPos = transform->get_VEC3ToPos();
    m_PrevPos = m_StartPos;

    m_MoveDir = transform->get_Forward(); // 前方向ベクトル

    m_MoveDir.x += Tool::RandRange(-0.03f, 0.03f);
    m_MoveDir.y += Tool::RandRange(-0.03f, 0.03f);
    m_MoveDir.z += Tool::RandRange(-0.03f, 0.03f);

    m_MoveDir = m_MoveDir.Normalize();
}

//*---------------------------------------------------------------------------------------
//*【?】状態のリセット
//* [引数]なし
//* [返値]なし
//*----------------------------------------------------------------------------------------
void ExplosionBullet::Reset()
{
    m_Parameter.Reset();
}
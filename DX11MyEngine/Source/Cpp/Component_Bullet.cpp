#include "pch.h"
#include "Component_Bullet.h"
#include "Component_AssultRifle.h"
#include "Component_Transform.h"
#include "Component_DecalRenderer.h"
#include "Component_TimerDestruction.h"
#include "RendererEngine.h"
#include "GameObjectManager.h"
#include "GameObject.h"
#include "InputFactory.h"
#include "MeshFactory.h"
#include "CollisionInfo.h"
#include "ResourceManager.h"

using namespace GIGA_Engine;
using namespace Input;
using namespace VECTOR3;

//*---------------------------------------------------------------------------------------
//*【?】コンストラクタ
//* [引数]
//* pOwner : オーナーオブジェクト
//* updateRank : 更新レイヤー
//*----------------------------------------------------------------------------------------
Bullet::Bullet(std::weak_ptr<GameObject> pOwner, int updateRank)
    : IComponent(pOwner, updateRank),
    m_StartPos(VEC3()),
    m_PrevPos(VEC3()),
    m_MoveVelocity(VEC3()),
    m_Counter(0)
{
    this->set_Tag("Bullet");
}



//*---------------------------------------------------------------------------------------
//*【?】デストラクタ
//*----------------------------------------------------------------------------------------
Bullet::~Bullet()
{

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
    auto transform = m_pOwner.lock()->get_Transform().lock();

    // 開始位置
    m_StartPos = transform->get_VEC3ToPos();
    m_PrevPos = m_StartPos;

    m_MoveVelocity = transform->get_Forward(); // 前方向ベクトル

    m_MoveVelocity.x += Tool::RandRange(-0.03f, 0.03f);
    m_MoveVelocity.y += Tool::RandRange(-0.03f, 0.03f);
    m_MoveVelocity.z += Tool::RandRange(-0.03f, 0.03f);

    m_MoveVelocity = m_MoveVelocity.Normalize();


    m_CollisionTask =
        [this, &renderer](const class CollisionInfo& _other)
        {
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

            auto transform = m_pOwner.lock()->get_Transform().lock();
            VEC3 pos = transform->get_VEC3ToPos();

            VEC3 hitNormal = _other.get_HitNormal();    // 衝突相手の法線

            // 水平方向の向きを求める
            float angleY = atan2(hitNormal.x, hitNormal.z);
            // 水平成分の長さ
            float xzLen = sqrtf(hitNormal.x * hitNormal.x + hitNormal.z * hitNormal.z);
            // 垂直方向の角度を求める
            // 法線の逆を向かせたいのでマイナスを付ける
            float angleX = atan2(-hitNormal.y, xzLen);

            // 当たった方向に伸ばそうとしたけど上手くいかなかった
            {
                //VEC3 moveVel = -m_MoveVelocity.Normalize();
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

            VEC3 scale;
            scale.x = 20.0f;
            scale.y = 20.0f;
            scale.z = 60.0f;

            auto obj = MeshFactory::CreateDecal(decal);
            obj->get_Component<DecalRenderer>()->Start(renderer);
            obj->get_Transform().lock()->set_Pos(pos);
            obj->get_Transform().lock()->set_Scale(scale);
            obj->get_Transform().lock()->set_RotateToRad(angleX, angleY, angleZ);
            obj->set_Tag("BulletHole");
            auto timer = obj->add_Component<TimerDestruction>();
            timer->set_LifeTime(2.0f);  // 生存時間

            // エフェクト
            VEC3 effectRot = VEC3(abs(angleX - 0.05f), angleY, 0.0f);
            int spark_handle = Master::m_pEffectManager->PlayEffect("Spark");   // 火花
            int smoke_handle = Master::m_pEffectManager->PlayEffect("Smoke");   // 煙
            int exp_handle = Master::m_pEffectManager->PlayEffect("Explosion_01");   // 煙
            int exp_smoke_handle = Master::m_pEffectManager->PlayEffect("Explosion_Smoke_01");   // 煙
            
            
            // 火花
            Master::m_pEffectManager->SetScaleEffect(spark_handle, 5.0f, 5.0f, 5.0f);
            Master::m_pEffectManager->SetPositionEffect(spark_handle, pos.x, pos.y, pos.z);
            Master::m_pEffectManager->SetRotationEffect(spark_handle, effectRot.x, effectRot.y, effectRot.z);

            // 煙
            Master::m_pEffectManager->SetScaleEffect(smoke_handle, 5.0f, 5.0f, 5.0f);
            Master::m_pEffectManager->SetPositionEffect(smoke_handle, pos.x, pos.y, pos.z);
            Master::m_pEffectManager->SetRotationEffect(smoke_handle, effectRot.x, effectRot.y, effectRot.z);

            float expSize = 10.0f;
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
void Bullet::Update(RendererEngine &renderer)
{
    auto transform = m_pOwner.lock()->get_Transform().lock();
    VEC3 crntPos = transform->get_VEC3ToPos();
    
    m_PrevPos = crntPos;
    crntPos = crntPos - m_MoveVelocity * 20.0f;

    transform->set_Pos(crntPos);

    // 距離で削除
    if (VEC3::Distance(crntPos, m_StartPos) > 2000.0f)
    {
        m_pOwner.lock()->set_StatusFlag(OBJECT_STATUS_BITFLAG::IS_DELETE);
    }
}


//*---------------------------------------------------------------------------------------
//*【?】描画
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//* [返値]なし
//*----------------------------------------------------------------------------------------
void Bullet::Draw(RendererEngine &renderer)
{

}


void Bullet::OnCollisionEnter(const class CollisionInfo &other)
{
    m_CollisionTask(other);
    m_pOwner.lock()->set_StatusFlag(OBJECT_STATUS_BITFLAG::IS_DELETE);
}
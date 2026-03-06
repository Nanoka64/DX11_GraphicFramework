#include "pch.h"
#include "Component_NormalBullet.h"
#include "Component_AssultRifle.h"
#include "Component_Transform.h"
#include "Component_DecalRenderer.h"
#include "Component_TimerDestruction.h"
#include "Component_MoveLogic.h"
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

constexpr float DECAL_SIZE_FACTOR        = 25.0f;   // デカールの大きさの補正値（transformのスケールだと小さすぎるため）
constexpr float DECAL_Z_AXIS_SIZE_FACTOR = 40.0f;    // デカールの奥行に加算する補正値
constexpr float DECAL_LIFE_TIME          = 10.0f;    // デカールの生存時間

//*---------------------------------------------------------------------------------------
//*【?】コンストラクタ
//* [引数]
//* pOwner : オーナーオブジェクト
//* updateRank : 更新レイヤー
//*----------------------------------------------------------------------------------------
NormalBullet::NormalBullet(std::weak_ptr<GameObject> pOwner, int updateRank)
    : BulletBase(pOwner, updateRank)
{
    this->set_Tag("NormalBullet");
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
            scale += DECAL_SIZE_FACTOR;
            scale.z += DECAL_Z_AXIS_SIZE_FACTOR;


            auto obj = MeshFactory::CreateDecal(decal);
            obj->get_Component<DecalRenderer>()->Start(renderer);
            obj->get_Transform().lock()->set_Pos(pos);
            obj->get_Transform().lock()->set_Scale(scale);
            obj->get_Transform().lock()->set_RotateToRad(angleX, angleY, angleZ);
            obj->set_Tag("BulletHole");
            auto timer = obj->add_Component<TimerDestruction>();
            timer->set_LifeTime(DECAL_LIFE_TIME);  // 生存時間

            // エフェクト
            VEC3 effectRot = VEC3(abs(angleX - 0.05f), angleY, 0.0f);
            int spark_handle = Master::m_pEffectManager->PlayEffect("Spark");   // 火花
            int smoke_handle = Master::m_pEffectManager->PlayEffect("Smoke");   // 煙
            
            VEC3 effectScale = 5.0f;

            // 火花
            Master::m_pEffectManager->SetScaleEffect(spark_handle, effectScale.x, effectScale.y, effectScale.z);
            Master::m_pEffectManager->SetPositionEffect(spark_handle, pos.x, pos.y, pos.z);
            Master::m_pEffectManager->SetRotationEffect(spark_handle, effectRot.x, effectRot.y, effectRot.z);

            // 煙
            Master::m_pEffectManager->SetScaleEffect(smoke_handle, effectScale.x, effectScale.y, effectScale.z);
            Master::m_pEffectManager->SetPositionEffect(smoke_handle, pos.x, pos.y, pos.z);
            Master::m_pEffectManager->SetRotationEffect(smoke_handle, effectRot.x, effectRot.y, effectRot.z);
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
    param._moveDirection = -m_MoveDir;// ※マイナスにしているのはプレイヤーの方向がおかしいせい（後で直す）
    param._moveSpeed = m_Parameter._speed;
    moveComp->Calculate(param);

    m_PrevPos = crntPos;


    //// 移動ベクトルを求める
    //VEC3 moveVec = m_MoveDir * (m_Parameter._speed * deltaTime);

    //// ※マイナスにしているのはプレイヤーの方向がおかしいせい（後で直す）
    //crntPos = crntPos - moveVec;
    //transform->set_Pos(crntPos);

    // 射程距離外で削除
    float distSq =  VEC3::DistanceSq(crntPos, m_StartPos);
    if (distSq > m_Parameter._range * m_Parameter._range) {
        m_pOwner.lock()->clear_StatusFlag(OBJECT_STATUS_BITFLAG::IS_ACTIVE);    // ノンアクティブに
    }
}

void NormalBullet::OnCollisionEnter(const class CollisionInfo &other)
{
    if (m_CollisionTask)
    {
        m_CollisionTask(other);
    }

    m_pOwner.lock()->clear_StatusFlag(OBJECT_STATUS_BITFLAG::IS_ACTIVE);    // ノンアクティブに
}


//*---------------------------------------------------------------------------------------
//*【?】パラメータ等の設定
//*     発射時に呼ぶ 
//* [引数]なし
//* [返値]なし
//*----------------------------------------------------------------------------------------
void NormalBullet::Setup()
{
    auto transform = m_pOwner.lock()->get_Transform().lock();

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
void NormalBullet::Reset()
{
    m_Parameter.Reset();
}
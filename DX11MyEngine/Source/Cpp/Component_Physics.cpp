#include "pch.h"
#include "Component_Physics.h"
#include "CollisionInfo.h"

using namespace GIGA_Engine;
using namespace VECTOR3;
using namespace UtilityData;

//*---------------------------------------------------------------------------------------
//*【?】コンストラクタ
//* [引数]
//* pOwner : オーナーオブジェクト
//* updateRank : 更新レイヤー
//*----------------------------------------------------------------------------------------
Physics::Physics(std::weak_ptr<GameObject> pOwner, int updateRank)
    :IComponent(pOwner, updateRank),
    m_Velocity(VEC3()),
    m_ForceAccumulator(VEC3()),
    m_Mass(1.0f),
    m_GravityScale(9.8f),
    m_IsEnable(true)
{
    this->set_Tag("Physics");
}

//*---------------------------------------------------------------------------------------
//*【?】デストラクタ
//*----------------------------------------------------------------------------------------
Physics::~Physics()
{
}

//*---------------------------------------------------------------------------------------
//*【?】開始
//*
//* [引数]
//* &renderer : 描画エンジン
//* [返値]なし
//*----------------------------------------------------------------------------------------
void Physics::Start(RendererEngine& renderer)
{

}


//*---------------------------------------------------------------------------------------
//*【?】更新
//*
//* [引数]
//* &renderer : 描画エンジン
//* [返値]なし
//*----------------------------------------------------------------------------------------
void Physics::Update(RendererEngine& renderer)
{
    if (m_IsEnable == false) return;

    float deltaTime = Master::m_pTimeManager->get_DeltaTime();
    auto transform = m_pOwner.lock()->get_Transform().lock();
    VEC3 position = transform->get_VEC3ToPos();

    // 1. 重力の適用
    VEC3 gravity = { 0.0f, -m_GravityScale * m_Mass, 0.0f };
    AddForce(gravity);

    // 2. 加速度の計算 (a = F/m)
    VEC3 acceleration = m_ForceAccumulator / m_Mass;

    // 3. 速度と位置の更新 (オイラー積分)
    m_Velocity += acceleration * deltaTime;
    position += m_Velocity * deltaTime;

    // ※バウンドや摩擦の処理は OnCollisionEnter で行うため、ここには何も書かない

    transform->set_Pos(position);

    // 4. 次のフレームのために力をリセット
    m_ForceAccumulator = { 0, 0, 0 };
}

//*---------------------------------------------------------------------------------------
//*【?】衝突時の処理
//*
//* [引数]
//* &info : 衝突情報
//* [返値]なし
//*----------------------------------------------------------------------------------------
void Physics::OnCollisionEnter(const CollisionInfo& info)
{
    VEC3 hitNorm = -info.get_HitNormal();

    // 現在の進行方向と、ぶつかった面の法線の内積
    float dot_VelocityAndNormal = VEC3::Dot(m_Velocity, hitNorm);

    // 内積がマイナス ＝ 面に向かって突っ込んでいる状態
    if (dot_VelocityAndNormal < 0.0f)
    {
        // 壁に向かって進もうとしている分の速度ベクトル
        VEC3 penetrationVelocity = hitNorm * dot_VelocityAndNormal;

        // 反発係数 (0.0fなら完全に滑る/壁に張り付く、0.3fなら少し跳ねる)
        float restitution = 0.3f;

        // 公式： V_new = V - (1 + e) * V_p に基づいて速度を補正（これ1行だけでOK！）
        m_Velocity = m_Velocity - (penetrationVelocity * (1.0f + restitution));


        // 【摩擦処理】床にぶつかった時だけ、XとZの速度を減衰させる
        // HitNormal.y が 0.5f より大きい（＝だいたい上を向いている面）なら床とみなす
        if (hitNorm.y > 0.1f)
        {
            m_Velocity.x *= 0.8f; // 摩擦係数（お好みで調整）
            m_Velocity.z *= 0.8f;
        }
    }
}

//*---------------------------------------------------------------------------------------
//*【?】継続的な力を加える
//*
//* [引数]
//* &_force : 力ベクトル
//* [返値]なし
//*----------------------------------------------------------------------------------------
void Physics::AddForce(const VECTOR3::VEC3& _force)
{
    m_ForceAccumulator += _force;
}

//*---------------------------------------------------------------------------------------
//*【?】瞬間的な衝撃を加える
//*
//* [引数]
//* &_impulse : 衝撃ベクトル
//* [返値]なし
//*----------------------------------------------------------------------------------------
void Physics::AddImpulse(const VECTOR3::VEC3& _impulse)
{
    m_Velocity = _impulse / m_Mass;
}

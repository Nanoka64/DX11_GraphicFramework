#pragma once
#include "IComponent.h"

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:Physics Class --- */
//
//  ★継承：Component ★
//
// 【?】物理コンポーネント
//		リジッドボディ的な役割     
// 
// ***************************************************************************************
class Physics : public IComponent
{
private:
    friend class PhysicsEditor;

    VECTOR3::VEC3 m_Velocity;
    VECTOR3::VEC3 m_ForceAccumulator; // 1フレームに蓄積された力
    float m_Mass ;
    float m_GravityScale; // 重力の強さ
    bool m_IsEnable;    
    bool m_IsGrounded;

    // --- 回転用の物理パラメータ ---
    VECTOR3::VEC3 m_AngularVelocity = { 0.0f, 0.0f, 0.0f };      // 角速度（1秒間にどの軸でどれくらい回転するか）
    float m_AngularDrag = 0.95f;                        // 空気抵抗による回転の減衰（0.0～1.0）

public:
    Physics(std::weak_ptr<GameObject> pOwner, int updateRank = 100);
    ~Physics();

    void Start(RendererEngine& renderer) override;		// 初期化
    void Update(RendererEngine& renderer) override;
    void OnCollisionEnter(const CollisionInfo& info) override;

    void AddForce(const VECTOR3::VEC3& _force);
    void AddImpulse(const VECTOR3::VEC3& _impulse);

    void set_IsEnable(bool _flag) { m_IsEnable = _flag; }
    const bool get_IsEnable()const { return m_IsEnable; }

    const VECTOR3::VEC3& get_Velocity()const { return m_Velocity; }
    const VECTOR3::VEC3& get_ForceAccumulator()const { return m_ForceAccumulator; }
    const float get_Mass()const { return m_Mass; }
    const float get_GravityScale()const { return m_GravityScale; }

    void set_Mass(float _mass) { m_Mass = _mass; }
    void set_GravityScale(float _g) { m_GravityScale = _g; }
    // 瞬間的な回転力を加える
    void AddAngularImpulse(const VECTOR3::VEC3& angularImpulse);

    // 移動ベクトルをクリア
    void SetZeroVelocity();
};


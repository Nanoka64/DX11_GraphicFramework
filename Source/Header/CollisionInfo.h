#pragma once
// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:CollisionInfo Class --- */
//
// 【?】衝突時の情報をまとめたクラス
//
// ***************************************************************************************
class CollisionInfo
{
private:
    std::weak_ptr<class GameObject> m_pHitObject;  // 衝突相手
    std::weak_ptr<class Transform> m_pHitTransform;// 衝突相手のトランスフォーム
    std::weak_ptr<class Collider> m_pHitCollider;  // 衝突相手のコライダー
    VECTOR3::VEC3 m_HitPoint;                      // 衝突位置
    VECTOR3::VEC3 m_HitNormal;                     // 衝突面の向き
    VECTOR3::VEC3 m_RelativeVelocity;              // 衝突した物体の相対速度
    float m_PenetrationDepth;                      // めり込み量
public:
    CollisionInfo();
    ~CollisionInfo();

    //*---------------------------------------------------------------------------------------
    //*【?】衝突相手のオブジェクト
    //*----------------------------------------------------------------------------------------
    void set_HitObject(std::weak_ptr<GameObject> _pObj) { m_pHitObject = _pObj; }
    std::weak_ptr<GameObject> get_HitObject()const { return m_pHitObject; };
    
    //*---------------------------------------------------------------------------------------
    //*【?】衝突相手のトランスフォーム
    //*----------------------------------------------------------------------------------------
    void set_HitTransform(std::weak_ptr<Transform> _pTrans) { m_pHitTransform = _pTrans; }
    std::weak_ptr<Transform> get_HitTransform()const { return m_pHitTransform; };

    //*---------------------------------------------------------------------------------------
    //*【?】衝突したコライダー
    //*----------------------------------------------------------------------------------------
    void set_HitCollider(std::weak_ptr<Collider> _pColl) { m_pHitCollider = _pColl; }
    std::weak_ptr<Collider> get_HitCollider()const { return m_pHitCollider; };

    //*---------------------------------------------------------------------------------------
    //*【?】衝突した場所
    //*----------------------------------------------------------------------------------------
    void set_HitPoint(const VECTOR3::VEC3 &_vIn) { m_HitPoint = _vIn; }
    VECTOR3::VEC3 get_HitPoint()const { return m_HitPoint; }

    //*---------------------------------------------------------------------------------------
    //*【?】衝突面の法線
    //*----------------------------------------------------------------------------------------
    void set_HitNormal(const VECTOR3::VEC3 &_vIn) { m_HitNormal = _vIn; }
    VECTOR3::VEC3 get_HitNormal()const { return m_HitNormal; }
    
    //*---------------------------------------------------------------------------------------
    //*【?】衝突した物体の相対速度
    //*----------------------------------------------------------------------------------------
    void set_RelativeVelocity(const VECTOR3::VEC3 &_vIn) { m_RelativeVelocity = _vIn; }
    VECTOR3::VEC3 get_RelativeVelocity()const { return m_HitPoint; }
    
    //*---------------------------------------------------------------------------------------
    //*【?】めり込み深度
    //*----------------------------------------------------------------------------------------
    void set_PenetrationDepth(const float &_depth) { m_PenetrationDepth = _depth; }
    float get_PenetrationDepth()const { return m_PenetrationDepth; }

};


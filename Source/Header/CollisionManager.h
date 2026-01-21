#pragma once

/// <summary>
/// 衝突の判定分岐
/// </summary>
enum class COLLISION_CALC_TYPE
{
    BOX_BOX,        // 箱 と 箱
    SPHRERE_SPHRERE,// 球 と 球
    BOX_SPHRERE,    // 箱 と 球
    RAY_RAY,        // 線分 と 線分
    BOX_RAY,        // 箱 と 線分
    SPHERE_RAY,     // 球 と 線分

    NUM,
};

struct CollInData_Ray
{
    VECTOR3::VEC3 _startPoint;    // 開始点 
    VECTOR3::VEC3 _endPoint;      // 終了点
};

struct CollInData_Sphere
{
    VECTOR3::VEC3 _pos;
    float _radius;
};

struct CollInData_AABB
{
    VECTOR3::VEC3 _min;
    VECTOR3::VEC3 _max;
};

class CollisionManager
{
private:
    // 衝突計算をするコライダーのリスト
    std::vector<std::shared_ptr<class Collider>> m_pCollidersList;

public:
    CollisionManager();
    ~CollisionManager();


    /// <summary>
    /// 衝突判定の更新
    /// </summary>
    void CollisionProcess();

    /// <summary>
    /// 判定を行う
    /// </summary>
    /// <returns></returns>
    bool HitCheck(std::shared_ptr<class Collider> _colA,std::shared_ptr<class Collider> _colB, class CollisionInfo* info);

    // 箱と箱
    bool HitCheck_BoxVsBox(const CollInData_AABB &_src, const CollInData_AABB &_dst);
    
    // 球と球
    bool HitCheck_SphereVsSphere(const CollInData_Sphere &_src, const CollInData_Sphere &_dst);
    
    // 箱と球
    bool HitCheck_BoxVsSphere(const CollInData_AABB &_box, const CollInData_Sphere &_sphere);
    
    // 箱と線
    bool HitCheck_BoxVsRay(const CollInData_AABB &_box, const CollInData_Ray &_ray);
    
    // 球と線
    bool HitCheck_SphereVsRay(const CollInData_Sphere &_sphere, const CollInData_Ray &_ray);

private:
    // コピー禁止
    CollisionManager(const CollisionManager &) = delete;
    CollisionManager &operator=(const CollisionManager &) = delete;
    // ------------------------------------------------------

};


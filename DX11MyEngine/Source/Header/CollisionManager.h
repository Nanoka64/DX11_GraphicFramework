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

/// <summary>
/// 3Dレイ判定
/// </summary>
struct CollInData_Ray
{
    VECTOR3::VEC3 _point;    // 開始点 
    VECTOR3::VEC3 _dir;      // 方向
};

/// <summary>
/// 3D球判定
/// </summary>
struct CollInData_Sphere
{
    VECTOR3::VEC3 _pos;
    float _radius;
};

/// <summary>
/// 3DボックスAABB判定
/// </summary>
struct CollInData_AABB
{
    VECTOR3::VEC3 _min;
    VECTOR3::VEC3 _max;
};

/// <summary>
/// 2DボックスAABB判定
/// </summary>
struct CollInData2D_AABB
{
    VECTOR2::VEC2 _min;
    VECTOR2::VEC2 _max;
};

// Oriented Bounding Box
struct CollInData_OBB
{
    VECTOR3::VEC3 _center;      // 中心位置
    VECTOR3::VEC3 _slope[3];    // 各座標軸の傾き
    VECTOR3::VEC3 _harfLength;  // 各座標軸に沿った長さの半分
};

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:CollisionManager Class --- */
//
//  ★★★シングルトン★★★
//
// 【?】衝突判定の管理
// 参考サイト：https://yutateno.hatenablog.jp/entry/2019/11/27/001801 
//           ：https://developer.mozilla.org/ja/docs/Games/Techniques/3D_collision_detection
//           ：https://qiita.com/Aqua-218/items/a432cf0410bff57202c5
//
// ***************************************************************************************
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

    void RegisterCollider(std::shared_ptr<class Collider> pCol);

    /// <summary>
    /// 判定を行う
    /// </summary>
    /// <returns></returns>
    bool HitCheck(std::shared_ptr<class Collider> _colA,std::shared_ptr<class Collider> _colB, std::shared_ptr<class MyTransform> _transA,std::shared_ptr<class MyTransform> _transB, class CollisionInfo* info);
    
    /// <summary>
    /// 範囲内のオブジェクトを取得する
    /// </summary>
    std::vector<std::shared_ptr<class Collider>> CheckSphere(const VECTOR3::VEC3& _center, float _radius, unsigned _mask);

    // 3D --------------------------------------------------------
    // 箱と箱 物理的判定
    bool HitCheck_BoxVsBox_Physics(const CollInData_AABB &_src, const CollInData_AABB &_dst, class CollisionInfo *info);

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

    // 2D --------------------------------------------------------
    // 箱と点
    bool HitCheck2D_BoxVsPoint(const CollInData2D_AABB& _box, const VECTOR2::VEC2& _p);



private:
    // コピー禁止
    CollisionManager(const CollisionManager &) = delete;
    CollisionManager &operator=(const CollisionManager &) = delete;
    // ------------------------------------------------------

};


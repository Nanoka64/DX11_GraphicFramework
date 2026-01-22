#include "pch.h"
#include "CollisionManager.h"
#include "Component_Collider.h"
#include "Component_BoxCollider.h"
#include "Component_SphereCollider.h"
#include "CollisionInfo.h"
#include "Component_Transform.h"
#include "GameObject.h"

using namespace VECTOR3;

CollisionManager::CollisionManager()
{
}

CollisionManager::~CollisionManager()
{
}

//*---------------------------------------------------------------------------------------
//*【?】衝突判定を行う
//* [引数] pCol : 登録するコライダー
//* [返値] なし
//*----------------------------------------------------------------------------------------
void CollisionManager::RegisterCollider(std::shared_ptr<class Collider> pCol)
{
    m_pCollidersList.push_back(pCol);
}

//*---------------------------------------------------------------------------------------
//*【?】衝突判定を行う
//* [引数]
//* [返値] なし
//*----------------------------------------------------------------------------------------
void CollisionManager::CollisionProcess()
{
    for (int i = 0; i < m_pCollidersList.size(); i++)
    {
        auto &colA = m_pCollidersList[i];

        // 使用フラグチェック
        if (colA->get_IsEnable() == false)
        {
            continue;
        }
        
        for (int j = i + 1; j < m_pCollidersList.size(); j++)
        {
            auto& colB = m_pCollidersList[j];

            // 両方とも静的なら判定しない
            if (colA->get_IsStatic() == colB->get_IsStatic())
            {
                continue;
            }

            colA->set_IsHit(false);
            colB->set_IsHit(false);

            CollisionInfo info;
            if (HitCheck(colA, colB, &info))
            {
                // 衝突
                colA->set_IsHit(true);
                colB->set_IsHit(true);

                // Bと衝突したことをオブジェクト側に伝える
                info.set_HitObject(colB->get_OwnerObj());
                info.set_HitCollider(colB);
                colA->get_OwnerObj().lock()->OnCollisionEnter(info);

                // Aと衝突したことをオブジェクト側に伝える
                info.set_HitObject(colA->get_OwnerObj());
                info.set_HitCollider(colA);
                colB->get_OwnerObj().lock()->OnCollisionEnter(info);
            }
        }
    }
}

bool CollisionManager::HitCheck(std::shared_ptr<class Collider> _colA, std::shared_ptr<class Collider> _colB, class CollisionInfo *info)
{
    COLLIDER_TYPE colA_Type = _colA->get_ColliderType();
    COLLIDER_TYPE colB_Type = _colB->get_ColliderType();

    // ボックス同士 ********************************************************************
    if (colA_Type == COLLIDER_TYPE::BOX && colB_Type == COLLIDER_TYPE::BOX)
    {
        // 一旦キャストして実装
        auto boxA = std::static_pointer_cast<BoxCollider>(_colA);
        auto boxB = std::static_pointer_cast<BoxCollider>(_colB);

        std::weak_ptr<Transform> transform_A = boxA->get_Transform();
        std::weak_ptr<Transform> transform_B = boxB->get_Transform();

        if (transform_A.expired() || transform_B.expired())
        {
            MessageBox(NULL, "トランスフォームコンポーネントの期限切れ", "衝突判定", MB_OK);
            return false;
        }

        // コライダーの位置と、オブジェクトの位置を合わせる。
        VEC3 centerA = boxA->get_Center() + transform_A.lock()->get_VEC3ToPos();
        VEC3 centerB = boxB->get_Center() + transform_B.lock()->get_VEC3ToPos();

        CollInData_AABB data_A;
        data_A._min = centerA - boxA->get_Size();
        data_A._max = centerA + boxA->get_Size();

        CollInData_AABB data_B;
        data_B._min = centerB - boxB->get_Size();
        data_B._max = centerB + boxB->get_Size();
        if (HitCheck_BoxVsBox(data_A, data_B))
        {
            return true;
        }
    }
    // 球同士 ********************************************************************
    else if (colA_Type == COLLIDER_TYPE::SPHERE && colB_Type == COLLIDER_TYPE::SPHERE)
    {
        CollInData_Sphere data_A;
        CollInData_Sphere data_B;

        if (HitCheck_SphereVsSphere(data_A, data_B))
        {
            return true;
        }
    }

    return false;
}

//*---------------------------------------------------------------------------------------
//*【?】ボックスとボックスの物理的な判定
//*
//* [引数]
//* &_src : ボックス 
//* &_dst : ボックス
//* [返値]
//* true : 当たった
//* false : 当たってない
//*----------------------------------------------------------------------------------------
bool CollisionManager::HitCheck_BoxVsBox_Physics(const CollInData_AABB &_src, const CollInData_AABB &_dst)
{
    return true;
}

//*---------------------------------------------------------------------------------------
//*【?】ボックスとボックスの判定
//*
//* [引数]
//* &_src : ボックス 
//* &_dst : ボックス
//* [返値]
//* true : 当たった
//* false : 当たってない
//*----------------------------------------------------------------------------------------
bool CollisionManager::HitCheck_BoxVsBox(const CollInData_AABB &_src, const CollInData_AABB &_dst)
{
    // 左側の判定
    if (_src._max.x < _dst._min.x) return false;

    // 右側の判定
    if (_src._min.x > _dst._max.x) return false;

    // 上側の判定
    if (_src._max.y < _dst._min.y) return false;

    // 下側の判定
    if (_src._min.y > _dst._max.y) return false;

    // 奥の判定
    if (_src._max.z < _dst._min.z) return false;

    // 手前の判定
    if (_src._min.z > _dst._max.z) return false;

    // 当たっている可能性しか残っていない
    return true;
}

//*---------------------------------------------------------------------------------------
//*【?】スフィアとスフィアの判定
//*
//* [引数]
//* &_src : スフィア 
//* &_dst : スフィア
//* [返値]
//* true : 当たった
//* false : 当たってない
//*----------------------------------------------------------------------------------------
bool CollisionManager::HitCheck_SphereVsSphere(const CollInData_Sphere &_src, const CollInData_Sphere &_dst)
{
    VECTOR3::VEC3 result;
    result = _dst._pos - _src._pos;

    float length = result.LengthSq();
    float r = _src._radius + _dst._radius;

    if ((r * r) > length)
    {
        return true;
    }

    return false;
}

//*---------------------------------------------------------------------------------------
//*【?】ボックスとスフィアの判定
//* 参考サイト：https://yutateno.hatenablog.jp/entry/2019/11/27/001801 
//*
//* [引数]
//* &_src : 箱 
//* &_dst : 球
//* [返値]
//* true : 当たった
//* false : 当たってない
//*----------------------------------------------------------------------------------------
bool CollisionManager::HitCheck_BoxVsSphere(const CollInData_AABB &_box, const CollInData_Sphere &_sphere)
{
    // 左側
    if (_box._min.x - _sphere._radius > _sphere._pos.x) return false;

    // 右側
    if (_box._max.x + _sphere._radius < _sphere._pos.x) return false;

    // 下側
    if (_box._min.y - _sphere._radius > _sphere._pos.y) return false;

    // 上側
    if (_box._max.y + _sphere._radius < _sphere._pos.y) return false;

    // 手前側
    if (_box._min.z - _sphere._radius > _sphere._pos.z) return false;

    // 奥側
    if (_box._max.z + _sphere._radius < _sphere._pos.z) return false;

    float r = _sphere._radius * _sphere._radius;

    // 左
    if (_sphere._pos.x > _box._min.x)
    {
        // 左上
        if (_sphere._pos.y < _box._max.y)
        {
        }
        else
        {   // 左下
            if (_sphere._pos.y > _box._min.y)
            {

            }
        }
    }
    else
    {

    }
    return true;
}


//*---------------------------------------------------------------------------------------
//*【?】ボックスとレイの判定
//*
//* [引数]
//* &_src : 箱 
//* &_dst : 光線
//* [返値]
//* true : 当たった
//* false : 当たってない
//*----------------------------------------------------------------------------------------
bool CollisionManager::HitCheck_BoxVsRay(const CollInData_AABB &_src, const CollInData_Ray &_dst)
{
    return true;
}

//*---------------------------------------------------------------------------------------
//*【?】スフィアとレイの判定
//*
//* [引数]
//* &_src : 球
//* &_dst : 光線
//* [返値]
//* true : 当たった
//* false : 当たってない
//*----------------------------------------------------------------------------------------
bool CollisionManager::HitCheck_SphereVsRay(const CollInData_Sphere &_src, const CollInData_Ray &_dst)
{
    return true;
}

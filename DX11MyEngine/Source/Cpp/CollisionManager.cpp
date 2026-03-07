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
    m_pCollidersList.clear();
}

//*---------------------------------------------------------------------------------------
//*【?】衝突判定を行う
//* [引数] pCol : 登録するコライダー
//* [返値] なし
//*----------------------------------------------------------------------------------------
void CollisionManager::RegisterCollider(std::shared_ptr<Collider> pCol)
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
    // コライダーがないまたは、オブジェクトがない場合は削除
    m_pCollidersList.erase(
        std::remove_if(
            m_pCollidersList.begin(),
            m_pCollidersList.end(),
            [](const std::shared_ptr<Collider> pIn)
            {
                return (pIn == nullptr || pIn->get_OwnerObj().expired());
            }),
        m_pCollidersList.end()
    );

    // 全てのコライダーのヒットフラグを一旦falseに
    for (auto &comp : m_pCollidersList)
    {
        comp->set_IsHit(false);
    }

    // 判定ループ処理
    for (int i = 0; i < m_pCollidersList.size(); i++)
    {
        auto &colA = m_pCollidersList[i];
        bool isStaticA = false;
        bool isStaticB = false;
        auto transA = colA->get_OwnerObj().lock()->get_Component<MyTransform>();
        if (transA == nullptr) {
            MessageBox(NULL, "Aトランスフォームコンポーネントがありません", "衝突判定", MB_OK);
            continue;
        }

        // 使用フラグチェック
        if (colA->get_IsEnable() == false) {
            continue;
        }
        
        for (int j = i + 1; j < m_pCollidersList.size(); j++)
        {
            auto& colB = m_pCollidersList[j];

            // 使用フラグチェック
            if (colB->get_IsEnable() == false){
                continue;
            }

            isStaticA = colA->get_IsStatic();
            isStaticB = colB->get_IsStatic();

            // 両方とも静的なら判定しない
            if (isStaticA && isStaticB)
            {
                continue;
            }

            // 衝突マスクのチェック
            if ((colA->get_CollisionBitMask() & static_cast<unsigned>(colB->get_CollisionCategory())) == 0 ||
                (colB->get_CollisionBitMask() & static_cast<unsigned>(colA->get_CollisionCategory())) == 0)
            {
                continue;
            }

            // トランスフォームの取得
            auto transB = colB->get_OwnerObj().lock()->get_Component<MyTransform>();

            if (transB == nullptr) {
                MessageBox(NULL, "Bトランスフォームコンポーネントがありません", "衝突判定", MB_OK);
                continue;
            }

            CollisionInfo info; // 衝突情報格納用
            
            // 衝突チェック
            if (HitCheck(colA, colB, transA, transB, &info))
            {
                // 衝突
                colA->set_IsHit(true);
                colB->set_IsHit(true);

                // トリガー判定
                bool isTriggerHit = (colA->get_IsTrigger() || colB->get_IsTrigger());

                // トリガーでない場合は押し出し処理を行う
                if (isTriggerHit == false)
                {
                    VEC3 pushVector;    // 押し出しベクトル
                    VEC3 currentPos;    // 押し出し反映用

                    // 押し出すベクトル = 法線 * めり込み量
                    pushVector = info.get_HitNormal() * info.get_PenetrationDepth();

                    // 静的オブジェクトの場合は0.0にする（押し出さない）
                    float ratioA = isStaticA ? 0.0f : 0.5f;
                    float ratioB = isStaticB ? 0.0f : 0.5f;


                    // Aは法線方向に押し出す *******************************************
                    currentPos = transA->get_VEC3ToPos();
                    transA->set_Pos(currentPos + (pushVector * ratioA));

                    // Bは衝突された側なので法線とは逆方向に押し出す ********************
                    currentPos = transB->get_VEC3ToPos();
                    transB->set_Pos(currentPos + (-pushVector * ratioB));
                }

                // Bと衝突したことをAオブジェクト側に伝える
                CollisionInfo infoA = info;
                infoA.set_HitObject(colB->get_OwnerObj());
                infoA.set_HitCollider(colB);


                // Aと衝突したことをBオブジェクト側に伝える
                CollisionInfo infoB = info;
                infoB.set_HitObject(colA->get_OwnerObj());
                infoB.set_HitCollider(colA);
                infoB.set_HitNormal(-info.get_HitNormal()); // そのままだとぶつかった側の押し出し法線なので、法線を反転させる


                // イベント通知
                if (colA->get_IsTrigger() || colB->get_IsTrigger())
                {
                    colA->get_OwnerObj().lock()->OnTriggerEnter(infoA);
                    colB->get_OwnerObj().lock()->OnTriggerEnter(infoB);
                }
                else
                {
                    colA->get_OwnerObj().lock()->OnCollisionEnter(infoA);
                    colB->get_OwnerObj().lock()->OnCollisionEnter(infoB);

                }

            }
        }
    }
}


//*---------------------------------------------------------------------------------------
//*【?】衝突したかどうかを確かめる
//*     コライダーごとに処理を分ける 
//*     参考サイト：https://qiita.com/Aqua-218/items/a432cf0410bff57202c5
// 
//* [引数]
//* _colA :     コライダーA
//* _colB :     コライダーB
//* _transA :   トランスフォームA
//* _transB :   トランスフォームB
//* info :      衝突情報の保存先
//* [返値]
//* true : 成功
//* false : 失敗
//*----------------------------------------------------------------------------------------
bool CollisionManager::HitCheck(
    std::shared_ptr<Collider> _colA, 
    std::shared_ptr<Collider> _colB, 
    std::shared_ptr<MyTransform> _transA, 
    std::shared_ptr<MyTransform> _transB, 
    CollisionInfo *info
)
{
    COLLIDER_TYPE colA_Type = _colA->get_ColliderType();
    COLLIDER_TYPE colB_Type = _colB->get_ColliderType();

    //=========================================================================================
    //
    //						 ボックス同士
    //
    //=========================================================================================
    if (colA_Type == COLLIDER_TYPE::BOX && colB_Type == COLLIDER_TYPE::BOX)
    {
        // 一旦キャストして実装（TODO: キャストしないように実装したい）
        auto boxA = std::static_pointer_cast<BoxCollider>(_colA);
        auto boxB = std::static_pointer_cast<BoxCollider>(_colB);

        // コライダーの位置と、オブジェクトの位置を合わせる。
        VEC3 centerA = boxA->get_Center() + _transA->get_VEC3ToPos();
        VEC3 centerB = boxB->get_Center() + _transB->get_VEC3ToPos();

        // サイズを反映させる
        CollInData_AABB data_A;
        data_A._min = centerA - boxA->get_Size();
        data_A._max = centerA + boxA->get_Size();

        CollInData_AABB data_B;
        data_B._min = centerB - boxB->get_Size();
        data_B._max = centerB + boxB->get_Size();

        // BoxとBoxの判定を行う
        if (HitCheck_BoxVsBox(data_A, data_B))
        {
            // 参考サイト

            // 各軸でのオーバーラップ量を計算
            float overlapX = std::min(data_A._max.x - data_B._min.x, data_B._max.x - data_A._min.x);
            float overlapY = std::min(data_A._max.y - data_B._min.y, data_B._max.y - data_A._min.y);
            float overlapZ = std::min(data_A._max.z - data_B._min.z, data_B._max.z - data_A._min.z);

            VEC3 normal = VEC3();

            if (overlapX <= overlapY && overlapX <= overlapZ)
            {
                info->set_PenetrationDepth(overlapX);
                normal = (centerA.x < centerB.x) ? VEC3(-1.0f, 0.0f, 0.0f) : VEC3(1.0f, 0.0f, 0.0f);
            }
            else if (overlapY <= overlapZ)
            {
                info->set_PenetrationDepth(overlapY);
                normal = (centerA.y < centerB.y) ? VEC3(0.0f, -1.0f, 0.0f) : VEC3(0.0f, 1.0f, 0.0f);
            }
            else
            {
                info->set_PenetrationDepth(overlapZ);
                normal = (centerA.z < centerB.z) ? VEC3(0.0f, 0.0f, -1.0f) : VEC3(0.0f, 0.0f, 1.0f);
            }

            info->set_HitNormal(normal);

            return true;
        }
    }
    //=========================================================================================
    //
    //						スフィア同士
    //
    //=========================================================================================
    else if (colA_Type == COLLIDER_TYPE::SPHERE && colB_Type == COLLIDER_TYPE::SPHERE)
    {
        // 一旦キャストして実装（TODO: キャストしないように実装したい）
        auto sphereA = std::static_pointer_cast<SphereCollider>(_colA);
        auto sphereB = std::static_pointer_cast<SphereCollider>(_colB);

        // コライダーの位置と、オブジェクトの位置を合わせる。
        VEC3 centerA = sphereA->get_Center() + _transA->get_VEC3ToPos();
        VEC3 centerB = sphereB->get_Center() + _transB->get_VEC3ToPos();

        // 中心位置と半径を入れる
        CollInData_Sphere data_A;
        data_A._pos = centerA;
        data_A._radius = sphereA->get_Radius();

        CollInData_Sphere data_B;
        data_B._pos = centerB;
        data_B._radius = sphereB->get_Radius();

        // 球同士の判定
        if (HitCheck_SphereVsSphere(data_A, data_B))
        {
            // 中心座標と半径で最大と最小位置を求める
            VEC3 maxPos_A = data_A._pos + data_A._radius;   
            VEC3 minPos_A = data_A._pos - data_A._radius;
            VEC3 maxPos_B = data_B._pos + data_B._radius;
            VEC3 minPos_B = data_B._pos - data_B._radius;

            // 各軸でのオーバーラップ量を計算
            float overlapX = std::min(maxPos_A.x - minPos_B.x, maxPos_B.x - minPos_A.x);
            float overlapY = std::min(maxPos_A.y - minPos_B.y, maxPos_B.y - minPos_A.y);
            float overlapZ = std::min(maxPos_A.z - minPos_B.z, maxPos_B.z - minPos_A.z);

            VEC3 normal = VEC3();

            if (overlapX <= overlapY && overlapX <= overlapZ)
            {
                info->set_PenetrationDepth(overlapX);
                normal = (centerA.x < centerB.x) ? VEC3(-1.0f, 0.0f, 0.0f) : VEC3(1.0f, 0.0f, 0.0f);
            }
            else if (overlapY <= overlapZ)
            {
                info->set_PenetrationDepth(overlapY);
                normal = (centerA.y < centerB.y) ? VEC3(0.0f, -1.0f, 0.0f) : VEC3(0.0f, 1.0f, 0.0f);
            }
            else
            {
                info->set_PenetrationDepth(overlapZ);
                normal = (centerA.z < centerB.z) ? VEC3(0.0f, 0.0f, -1.0f) : VEC3(0.0f, 0.0f, 1.0f);
            }

            info->set_HitNormal(normal);


            return true;
        }
    }
    //=========================================================================================
    //
    //						ボックスとスフィア
    //
    //=========================================================================================
    else if (colA_Type == COLLIDER_TYPE::BOX && colB_Type == COLLIDER_TYPE::SPHERE)
    {
        // 一旦キャストして実装（TODO: キャストしないように実装したい）
        auto boxA = std::static_pointer_cast<BoxCollider>(_colA);
        auto sphereB = std::static_pointer_cast<SphereCollider>(_colB);

        // コライダーの位置と、オブジェクトの位置を合わせる。
        VEC3 centerA = boxA->get_Center() + _transA->get_VEC3ToPos();
        VEC3 centerB = sphereB->get_Center() + _transB->get_VEC3ToPos();

        // ボックス
        CollInData_AABB data_A;
        data_A._min = centerA - boxA->get_Size();
        data_A._max = centerA + boxA->get_Size();

        // スフィア
        CollInData_Sphere data_B;
        data_B._pos = centerB;
        data_B._radius = sphereB->get_Radius();

        // 球同士の判定
        if (HitCheck_BoxVsSphere(data_A, data_B))
        {
            // 中心座標と半径で最大と最小位置を求める
            // ボックスはそのまま
            VEC3 maxPos_A = data_A._max;
            VEC3 minPos_A = data_A._min;
            VEC3 maxPos_B = data_B._pos + data_B._radius;
            VEC3 minPos_B = data_B._pos - data_B._radius;

            // 各軸でのオーバーラップ量を計算
            float overlapX = std::min(maxPos_A.x - minPos_B.x, maxPos_B.x - minPos_A.x);
            float overlapY = std::min(maxPos_A.y - minPos_B.y, maxPos_B.y - minPos_A.y);
            float overlapZ = std::min(maxPos_A.z - minPos_B.z, maxPos_B.z - minPos_A.z);

            VEC3 normal = VEC3();

            if (overlapX <= overlapY && overlapX <= overlapZ)
            {
                info->set_PenetrationDepth(overlapX);
                normal = (centerA.x < centerB.x) ? VEC3(-1.0f, 0.0f, 0.0f) : VEC3(1.0f, 0.0f, 0.0f);
            }
            else if (overlapY <= overlapZ)
            {
                info->set_PenetrationDepth(overlapY);
                normal = (centerA.y < centerB.y) ? VEC3(0.0f, -1.0f, 0.0f) : VEC3(0.0f, 1.0f, 0.0f);
            }
            else
            {
                info->set_PenetrationDepth(overlapZ);
                normal = (centerA.z < centerB.z) ? VEC3(0.0f, 0.0f, -1.0f) : VEC3(0.0f, 0.0f, 1.0f);
            }

            info->set_HitNormal(normal);


            return true;
        }
    }
    
    
    
    return false;
}

//*---------------------------------------------------------------------------------------
//*【?】範囲内のオブジェクトを取得する
//*
//* [引数]
//* &_center : 中心座標
//* _radius  : 範囲
//* _mask    : 衝突判定用マスク
//* [返値]
//* 範囲内オブジェクトのリスト 
//*----------------------------------------------------------------------------------------
std::vector<std::shared_ptr<Collider>> CollisionManager::CheckSphere(const VECTOR3::VEC3& _center, float _radius, unsigned _mask)
{
    std::vector<std::shared_ptr<Collider>> hitList;

    for (auto& col : m_pCollidersList) {
        if (!col->get_IsEnable()) continue;

        // マスクチェック
        if (!(UINT_CAST(col->get_CollisionCategory()) & _mask)) continue;

        // トランスフォーム取得
        auto trans = col->get_OwnerObj().lock()->get_Component<MyTransform>();
        VEC3 pos = trans->get_VEC3ToPos();
        CollInData_Sphere srcA = { _center,_radius };
        CollInData_Sphere srcB = { pos,    _radius };

        if (HitCheck_SphereVsSphere(srcA, srcB))
        {
            hitList.push_back(col);
        }
    }
    return hitList;
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
bool CollisionManager::HitCheck_BoxVsBox_Physics(const CollInData_AABB &_src, const CollInData_AABB &_dst, class CollisionInfo *info)
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
//*【?】2Dボックスと2D ポイントの判定
//*
//* [引数]
//* &_box : ボックス 
//* &_p : ポイント
//* [返値]
//* true : 当たった
//* false : 当たってない
//*----------------------------------------------------------------------------------------
bool CollisionManager::HitCheck2D_BoxVsPoint(const CollInData2D_AABB& _box, const VECTOR2::VEC2& _p)
{
    // 左側の判定
    if (_box._max.x < _p.x) return false;

    // 右側の判定
    if (_box._min.x > _p.x) return false;

    // 上側の判定
    if (_box._max.y < _p.y) return false;

    // 下側の判定
    if (_box._min.y > _p.y) return false;

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
//*           ：https://developer.mozilla.org/ja/docs/Games/Techniques/3D_collision_detection
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
    // クランプして球の中心からボックスの最も近い点を取得
    VEC3 P;
    P.x = std::max(_box._min.x, std::min(_sphere._pos.x, _box._max.x));
    P.y = std::max(_box._min.y, std::min(_sphere._pos.y, _box._max.y));
    P.z = std::max(_box._min.z, std::min(_sphere._pos.z, _box._max.z));

    float distance = (P.x - _sphere._pos.x) * (P.x - _sphere._pos.x) +
                     (P.y - _sphere._pos.y) * (P.y - _sphere._pos.y) +
                     (P.z - _sphere._pos.z) * (P.z - _sphere._pos.z);

    // 距離が球の半径より小さいなら衝突
    return distance < _sphere._radius;
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
bool CollisionManager::HitCheck_BoxVsRay(const CollInData_AABB &_box, const CollInData_Ray &_ray)
{
    float t_Min = 0.0f;
    float t_Max = 0.0f;
    
    VEC3 minV = _box._min;
    VEC3 maxV = _box._max;
    VEC3 point = _ray._point;
    VEC3 dir = _ray._dir;

    // X軸の判定*************************************************
    float tx_Min = 0.0f;
    float tx_Max = 0.0f;
    // 線分ベクトルが0.0の場合
    if (dir.x == 0.0f) {
        if (point.x < minV.x || point.x > maxV.x)
        {
            return false;
        }

        tx_Min = 0.0f;
        tx_Max = 1.0f;
    }
    else {
        // X軸での領域を決定
        float t0 = (minV.x - point.x) / dir.x;
        float t1 = (maxV.x - point.x) / dir.x;
        if (t0 < t1){
            tx_Min = t0;
            tx_Max = t1;
        }
        else{
            tx_Min = t1;
            tx_Max = t0;
        }
        // X軸領域内ですでに範囲外
        if (tx_Max < 0.0f || tx_Min > 1.0f) {
            return false;
        }
    }

    t_Min = tx_Min;
    t_Max = tx_Max;


    // Y軸の判定*************************************************
    float ty_Min = 0.0f;
    float ty_Max = 0.0f;
    // 線分ベクトルが0.0の場合
    if (dir.y == 0.0f) {
        if (point.y < minV.y || point.y > maxV.y)
        {
            return false;
        }

        ty_Min = 0.0f;
        ty_Max = 1.0f;
    }
    else {
        // X軸での領域を決定
        float t0 = (minV.y - point.y) / dir.y;
        float t1 = (maxV.y - point.y) / dir.y;
        if (t0 < t1) {
            ty_Min = t0;
            ty_Max = t1;
        }
        else {
            ty_Min = t1;
            ty_Max = t0;
        }    
        // Y軸領域内ですでに範囲外
        if (ty_Max < 0.0f || ty_Min > 1.0f) {
            return false;
        }
    }

    if (t_Max < ty_Min || t_Min > ty_Max)
    {
        return false;
    }

    // X軸とY軸の領域が被っていない
    if (t_Min < ty_Min)t_Min = ty_Min;
    if (t_Max > ty_Max)t_Max = ty_Max;



    // Z軸の判定*************************************************
    float tz_Min = 0.0f;
    float tz_Max = 0.0f;
    // 線分ベクトルが0.0の場合
    if (dir.z == 0.0f) {
        if (point.z < minV.z || point.z > maxV.z)
        {
            return false;
        }

        tz_Min = 0.0f;
        tz_Max = 1.0f;
    }
    else {
        // X軸での領域を決定
        float t0 = (minV.z - point.z) / dir.z;
        float t1 = (maxV.z - point.z) / dir.z;
        if (t0 < t1) {
            tz_Min = t0;
            tz_Max = t1;
        }
        else {
            tz_Min = t1;
            tz_Max = t0;
        }
        // Z軸領域内ですでに範囲外
        if (tz_Max < 0.0f || tz_Min > 1.0f) {
            return false;
        }
    }

    if (t_Max < tz_Min || t_Min > tz_Max)
    {
        return false;
    }

    // X軸とY軸の領域が被っていない
    if (t_Min < tz_Min)t_Min = tz_Min;
    if (t_Max > tz_Max)t_Max = tz_Max;

    // 共通領域のチェック
    if (t_Min > 1.0f || t_Max < 0.0f){
        return false;
    }

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

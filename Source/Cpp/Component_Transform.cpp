#include "pch.h"
#include "Component_Transform.h"
#include "GameObject.h"

using namespace DirectX;
using namespace VERTEX;
using namespace Tool::UV;
using namespace Tool;


const DirectX::XMVECTOR Transform::FORWARD  = DirectX::XMVectorSet(0, 0, 1, 0);
const DirectX::XMVECTOR Transform::UP       = DirectX::XMVectorSet(0, 1, 0, 0);
const DirectX::XMVECTOR Transform::RIGHT    = DirectX::XMVectorSet(1, 0, 0, 0);


/// <summary>
/// コンストラクタ
/// </summary>
Transform::Transform(std::weak_ptr<GameObject> pOwner, int updateRank)
    : IComponent(pOwner, updateRank),
	m_Position(DirectX::XMVectorZero()),
	m_Rotation(DirectX::XMVectorZero()),
	m_Scale(DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f)),
	m_pParent()
{
    this->set_Tag("Transform");
}

/// <summary>
/// デストラクタ
/// </summary>
Transform::~Transform()
{
    this->Release();
}


//*---------------------------------------------------------------------------------------
//*【?】特定の方向に向かせる
//*     主にターゲットになるオブジェクトへ向かせる場合等に使う 
//*
//* [引数]
//* &target : ターゲットのトランスフォーム
//*
//* [返値]
//* なし 
//*----------------------------------------------------------------------------------------
XMMATRIX Transform::LookAt(const VECTOR3::VEC3 &target)
{
    VEC3 forward;
    VEC3 right;
    VEC3 up;

    // 目標方向から前方向ベクトルを作る
    forward = (target - VEC3::FromXMVECTOR(this->m_Position)).Normalize();
    
    // 上方向と前方向ベクトルから横ベクトルを求める
    right = VEC3::Cross(this->get_Up(), forward).Normalize();
    
    // 前と横方向のベクトルの外積を求めて上方向ベクトルにする
    up = VEC3::Cross(forward, right).Normalize();
    
    // 回転行列の作成
    XMMATRIX mtxRot = XMMatrixIdentity();
    mtxRot.r[0] = right;
    mtxRot.r[1] = up;
    mtxRot.r[2] = forward;


    return mtxRot;
}


// ---------------------------------------------------------------------------
/// <summary>
/// 解放
/// </summary>
// ---------------------------------------------------------------------------
void Transform::Release()
{
}

// *===========================================================================
// ・セッタ
// *===========================================================================
/*--------------- Position ---------------*/
void Transform::set_Pos(float x, float y, float z){
    m_Position = XMVectorSet(x, y, z, 1.f);
}
void Transform::set_Pos(const VECTOR3::VEC3& vIn) {
    m_Position = vIn;
}

/*--------------- Rotate ---------------*/
/* ラジアン指定 */

void Transform::set_RotateToRad(float x, float y, float z){
    m_Rotation = XMVectorSet(x, y, z, 1.f);
}
void Transform::set_RotateToRad(const VECTOR3::VEC3& vIn) {
    m_Rotation = vIn;
}

/* デグリー指定 */

void Transform::set_RotateToDeg(float x, float y, float z){
    m_Rotation = XMVectorSet(DegToRad(x), DegToRad(y), DegToRad(z), 1.f);
}
void Transform::set_RotateToDeg(const VECTOR3::VEC3& vIn) {
    VEC3 deg;
    deg.x = DegToRad(vIn.x);
    deg.y = DegToRad(vIn.y);
    deg.z = DegToRad(vIn.z);

    m_Rotation = deg;
}

/*--------------- Scale ---------------*/
void Transform::set_Scale(float x, float y, float z){
    m_Scale = XMVectorSet(x, y, z, 1.f);
}
void Transform::set_Scale(const VECTOR3::VEC3& vIn) {
    m_Scale = vIn;
}

// *===========================================================================
// ・ゲッタ
// *===========================================================================
/*--------------- Position ---------------*/
XMVECTOR Transform::get_XMVecToPos() const {
    return m_Position;
}
const VEC3 Transform::get_VEC3ToPos() const {
    return VEC3(
        XMVectorGetX(m_Position),
        XMVectorGetY(m_Position),
        XMVectorGetZ(m_Position)
    );
}
/*--------------- Rotate ---------------*/
/* ラジアン */

XMVECTOR Transform::get_XMVecToRotateToRad() const {
    return m_Rotation;
}
const VEC3 Transform::get_VEC3ToRotateToRad() const {
    return VEC3(
        XMVectorGetX(m_Rotation),
        XMVectorGetY(m_Rotation),
        XMVectorGetZ(m_Rotation)
    );
}

/* デグリー */

XMVECTOR Transform::get_XMVecToRotateToDeg() const {
	return XMVectorSet(
		XMConvertToDegrees(XMVectorGetX(m_Rotation)),
		XMConvertToDegrees(XMVectorGetY(m_Rotation)),
		XMConvertToDegrees(XMVectorGetZ(m_Rotation)),
		0.0f
	);
}

const VEC3 Transform::get_VEC3ToRotateToDeg() const {
    VEC3 res = VEC3(
        XMVectorGetX(m_Rotation),
        XMVectorGetY(m_Rotation),
        XMVectorGetZ(m_Rotation)
    );
    res.x = RadToDeg(res.x);
    res.y = RadToDeg(res.y);
    res.z = RadToDeg(res.z);
    return res;
}

/*--------------- Scale ---------------*/
XMVECTOR Transform::get_XMVecToScale() const {
    return m_Scale;
}
const VEC3 Transform::get_VEC3ToScale() const {
    return VEC3(
        XMVectorGetX(m_Scale),
        XMVectorGetY(m_Scale),
        XMVectorGetZ(m_Scale)
    );
}

/// <summary>
/// 座標行列取得
/// </summary>
XMMATRIX Transform::get_MtxPos()const{
    return XMMatrixTranslationFromVector(
        m_Position
    );
}
/// <summary>
/// 回転行列取得
/// </summary>
XMMATRIX Transform::get_MtxRotate()const{
    return XMMatrixScaling(
        XMVectorGetX(m_Rotation),
        XMVectorGetY(m_Rotation),
        XMVectorGetZ(m_Rotation)
    );
}

/// <summary>
/// 拡大縮小行列取得
/// </summary>
XMMATRIX Transform::get_MtxScale()const{
    return XMMatrixScalingFromVector(
        m_Scale
    );
}

// -----------------------------------------------------------------------------
/// <summary>
/// ワールド行列取得
/// </summary>
/// <returns></returns>
// -----------------------------------------------------------------------------
XMMATRIX Transform::get_WorldMtx()const{
    XMMATRIX mtxS   = XMMatrixScalingFromVector(m_Scale);
    XMMATRIX mtxRot = XMMatrixRotationRollPitchYawFromVector(m_Rotation);
    XMMATRIX mtxT   = XMMatrixTranslationFromVector(m_Position);

    XMMATRIX localMtx = mtxS * mtxRot * mtxT;

    // 親がいるなら自分と親を掛けたものを返す
    if (m_pParent.lock())
        return localMtx * m_pParent.lock()->get_WorldMtx();
    else
        return localMtx;
}

// -----------------------------------------------------------------------------
/// <summary>
/// 回転を含めないワールド行列
/// </summary>
/// <returns></returns>
// -----------------------------------------------------------------------------
XMMATRIX Transform::get_WorldMtx(const XMMATRIX &scl, const XMMATRIX &rot, const XMMATRIX &trans)const
{
    XMMATRIX localMtx = scl * rot * trans;

    // 親がいるなら自分と親を掛けたものを返す
    if (m_pParent.lock())
        return localMtx * m_pParent.lock()->get_WorldMtx();
    else
        return localMtx;
}

// -----------------------------------------------------------------------------
/// <summary>
/// 回転を含めないワールド行列
/// </summary>
/// <returns></returns>
// -----------------------------------------------------------------------------
XMMATRIX Transform::get_ExcludingRotWorldMtx()const{
    XMMATRIX mtxS   = XMMatrixScalingFromVector(m_Scale);
    XMMATRIX mtxT   = XMMatrixTranslationFromVector(m_Position);

    XMMATRIX localMtx = mtxS * mtxT;

    // 親がいるなら自分と親を掛けたものを返す
    if (m_pParent.lock())
        return localMtx * m_pParent.lock()->get_WorldMtx();
    else
        return localMtx;
}


//*---------------------------------------------------------------------------------------
//* @:Transform Class 
//*【?】「前」方向取得
//* 引数：なし
//* 返値：VEC3
//*----------------------------------------------------------------------------------------
const VEC3 Transform::get_Forward() const
{
    return VEC3::FromXMVECTOR(DirectX::XMVector3Rotate(FORWARD, m_Rotation));
}


//*---------------------------------------------------------------------------------------
//* @:Transform Class 
//*【?】「上」方向取得
//* 引数：なし
//* 返値：VEC3
//*----------------------------------------------------------------------------------------
VEC3 Transform::get_Up() const
{
    return VEC3::FromXMVECTOR(DirectX::XMVector3Rotate(UP, m_Rotation));
}


//*---------------------------------------------------------------------------------------
//* @:Transform Class 
//*【?】「右」方向取得
//* 引数：なし
//* 返値：VEC3
//*----------------------------------------------------------------------------------------
VEC3 Transform::get_Right() const
{
    return VEC3::FromXMVECTOR(DirectX::XMVector3Rotate(RIGHT, m_Rotation));
}

std::weak_ptr<Transform> Transform::get_Parent()const
{
    return m_pParent;
}

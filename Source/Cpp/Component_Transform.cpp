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
    m_LocalOffset_Position(DirectX::XMVectorZero()),
    m_LocalOffset_Rotation(DirectX::XMVectorZero()),
    m_LocalOffset_Scale(DirectX::XMVectorZero()),
	m_pParent(),
    m_isDirty(false)
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


/////////////////////////////////////////////////////////////////////////////////////////
//
//  ◆ワールド空間
//
/////////////////////////////////////////////////////////////////////////////////////////
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

/* ワールド座標の取得 */
const VECTOR3::VEC3 Transform::get_WorldVEC3ToPos()const
{
    XMMATRIX mat = get_WorldMtx();
    return VEC3::FromXMVECTOR(mat.r[3]);
}



/////////////////////////////////////////////////////////////////////////////////////////
//
//  ◆ローカル空間オフセット
//
/////////////////////////////////////////////////////////////////////////////////////////

// *===========================================================================
// ・セッタ
// *===========================================================================
/*--------------- Position ---------------*/
void Transform::set_VEC3ToLocalOffset_Pos(const VECTOR3::VEC3 &vIn) {
    m_LocalOffset_Position = vIn;
}
/*--------------- Rotate ---------------*/
/* ラジアン */
void Transform::set_VEC3ToLocalOffset_RotateToRad(const VECTOR3::VEC3 &vIn) {
    m_LocalOffset_Rotation = vIn;
}
/* デグリー */
void Transform::set_VEC3ToLocalOffset_RotateToDeg(const VECTOR3::VEC3 &vIn) {
    VEC3 deg;
    deg.x = DegToRad(vIn.x);
    deg.y = DegToRad(vIn.y);
    deg.z = DegToRad(vIn.z);

    m_LocalOffset_Rotation = deg;
}
/*--------------- Scale ---------------*/
void Transform::set_VEC3ToLocalOffset_Scale(const VECTOR3::VEC3 &vIn) {
    m_LocalOffset_Scale = vIn;
}

// *===========================================================================
// ・ゲッタ
// *===========================================================================
/*--------------- Position ---------------*/
const VEC3 Transform::get_VEC3ToLocalOffset_Pos()const
{
    return VEC3(
        XMVectorGetX(m_LocalOffset_Position),
        XMVectorGetY(m_LocalOffset_Position),
        XMVectorGetZ(m_LocalOffset_Position)
    );
}

/*--------------- Rotate ---------------*/
/* ラジアン */
const VEC3 Transform::get_VEC3ToLocalOffset_RotateToRad()const
{
    return VEC3(
        XMVectorGetX(m_LocalOffset_Rotation),
        XMVectorGetY(m_LocalOffset_Rotation),
        XMVectorGetZ(m_LocalOffset_Rotation)
    );
}
/* デグリー */
const VEC3 Transform::get_VEC3ToLocalOffset_RotateToDeg()const
{
    VEC3 res = VEC3(
        XMVectorGetX(m_LocalOffset_Rotation),
        XMVectorGetY(m_LocalOffset_Rotation),
        XMVectorGetZ(m_LocalOffset_Rotation)
    );
    res.x = RadToDeg(res.x);
    res.y = RadToDeg(res.y);
    res.z = RadToDeg(res.z);
    return res;
}

/*--------------- Scale ---------------*/
const VEC3 Transform::get_VEC3ToLocalOffset_Scale()const
{
    return VEC3(
        XMVectorGetX(m_LocalOffset_Scale),
        XMVectorGetY(m_LocalOffset_Scale),
        XMVectorGetZ(m_LocalOffset_Scale)
    );
}


/////////////////////////////////////////////////////////////////////////////////////////
//
//  ◆行列
//
/////////////////////////////////////////////////////////////////////////////////////////
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
    return XMMatrixRotationRollPitchYawFromVector(
        m_Rotation
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
XMMATRIX Transform::get_WorldMtx()const {

    // オフセットを加算
    XMVECTOR scl = m_Scale    + m_LocalOffset_Scale;
    XMVECTOR rot = m_Rotation + m_LocalOffset_Rotation;
    XMVECTOR pos = m_Position + m_LocalOffset_Position;

    XMMATRIX mtxS = XMMatrixScalingFromVector(scl);
    XMMATRIX mtxRot = XMMatrixRotationRollPitchYawFromVector(rot);
    XMMATRIX mtxT = XMMatrixTranslationFromVector(pos);

    XMMATRIX localMtx = mtxS * mtxRot * mtxT;

    // 親がいるなら自分と親を掛けたものを返す
    if (m_pParent.lock())
    {
        return localMtx * m_pParent.lock()->get_WorldMtx();
    }
    else
    {
        return localMtx;
    }
}

// -----------------------------------------------------------------------------
/// <summary>
/// ワールド行列取得
/// パラメータを明示的に指定する
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
    // オフセットを加算
    // オフセットを加算
    XMVECTOR scl = m_Scale + m_LocalOffset_Scale;
    XMVECTOR pos = m_Position + m_LocalOffset_Position;

    XMMATRIX mtxS = XMMatrixScalingFromVector(scl);
    XMMATRIX mtxT = XMMatrixTranslationFromVector(pos);

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
    // オイラー角からクォータニオンを作成
    XMVECTOR quaternion = XMQuaternionRotationRollPitchYawFromVector(m_Rotation);
    return VEC3::FromXMVECTOR(DirectX::XMVector3Rotate(FORWARD, quaternion));
}


//*---------------------------------------------------------------------------------------
//* @:Transform Class 
//*【?】「上」方向取得
//* 引数：なし
//* 返値：VEC3
//*----------------------------------------------------------------------------------------
VEC3 Transform::get_Up() const
{
    // オイラー角からクォータニオンを作成
    XMVECTOR quaternion = XMQuaternionRotationRollPitchYawFromVector(m_Rotation);
    return VEC3::FromXMVECTOR(DirectX::XMVector3Rotate(UP, quaternion));
}


//*---------------------------------------------------------------------------------------
//* @:Transform Class 
//*【?】「右」方向取得
//* 引数：なし
//* 返値：VEC3
//*----------------------------------------------------------------------------------------
VEC3 Transform::get_Right() const
{
    // オイラー角からクォータニオンを作成
    XMVECTOR quaternion = XMQuaternionRotationRollPitchYawFromVector(m_Rotation);
    return VEC3::FromXMVECTOR(DirectX::XMVector3Rotate(RIGHT, quaternion));
}

std::weak_ptr<Transform> Transform::get_Parent()const
{
    return m_pParent;
}

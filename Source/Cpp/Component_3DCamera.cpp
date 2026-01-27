#include "pch.h"
#include "Component_3DCamera.h"
#include "GameObject.h"
#include "InputFactory.h"
#include "DirectWriteManager.h"
#include "RendererEngine.h"

using namespace Input;
using namespace VECTOR3;
using namespace VECTOR2;

using namespace DirectX;

#define CAMERA_ANGLE_SPEED		0.05f		// カメラの方向転換スピード
#define CAMERA_MOVE_FACTOR		0.5f		// 
#define CAMERA_POS_OFFSET		10.0f		// 
#define CAMERA_FOCUS_Y_OFFSET	18.0f		// 


//*---------------------------------------------------------------------------------------
//* @:Camera3D Class 
//*【?】コンストラクタ
//* 引数：1.オーナーオブジェクト
//* 引数：2.更新レイヤー
//*----------------------------------------------------------------------------------------
Camera3D::Camera3D(std::weak_ptr<GameObject> pOwner, int updateRank) : IComponent(pOwner, updateRank),
m_FocusPoint({ 0.0f,0.0f,0.0f }),
m_UpVec({ 0.0f,1.0f,0.0f }),
m_Angle_H(1.57f),
m_Angle_V(0.f),
m_Fov(90.0f),
m_NearClipDist(1.0f),
m_FarClipDist(8000.0f)
{
	this->set_Tag("Camera3D"); 
	m_PosOffset.x = CAMERA_POS_OFFSET;
	m_PosOffset.y = CAMERA_POS_OFFSET;
	m_PosOffset.z = CAMERA_POS_OFFSET;
	m_FocusOffset.x = 0.0f;
	m_FocusOffset.y = CAMERA_FOCUS_Y_OFFSET;
	m_FocusOffset.z = 0.0f;
}


//*---------------------------------------------------------------------------------------
//* @:Camera3D Class 
//*【?】デストラクタ
//* 引数：なし
//*----------------------------------------------------------------------------------------
Camera3D::~Camera3D()
{

}

//*---------------------------------------------------------------------------------------
//* @:Camera3D Class 
//*【?】初期化
//* 引数：1.RendererEngine
//* 返値：void
//*----------------------------------------------------------------------------------------
void Camera3D::Start(RendererEngine& renderer)
{

}


//*---------------------------------------------------------------------------------------
//* @:Camera3D Class 
//*【?】カメラの操作など
//*		Updateに書くと更新順で不都合が出る可能性があるので、
//*		明示的にこれを呼び出す。
//* 引数：なし
//* 返値：void
//*----------------------------------------------------------------------------------------
void Camera3D::LateUpdate(RendererEngine &renderer)
{
	// マウスの移動量の差を取得する
	LONG lX = Master::m_pInputManager->GetMousePosSlopeX();	
	LONG lY = Master::m_pInputManager->GetMousePosSlopeY();
	float semsitivity = 0.004f;

	// マウスの移動量を計算
	m_Angle_H -= (float)lX * semsitivity;
	m_Angle_V += (float)lY * semsitivity;

	if (m_Angle_V >= 1.5f)
	{
		m_Angle_V = 1.5f;
	}
	if (m_Angle_V <= -1.0f)
	{
		m_Angle_V = -1.0f;
	}
	if (m_Angle_H >= 3.14f) {
		m_Angle_H -= 6.28f;
	}
	if (m_Angle_H <= -3.14f) {
		m_Angle_H += 6.28f;
	}

	if (GetInput(GAME_CONFIG::VIEW_UP))	// 上
	{
		m_Angle_V += CAMERA_ANGLE_SPEED;
		if (m_Angle_V >= 1.5f) {
			m_Angle_V = 1.5f;
		}
	}
	if (GetInput(GAME_CONFIG::VIEW_DOWN))		// 下
	{
		m_Angle_V -= CAMERA_ANGLE_SPEED;
		if (m_Angle_V <= -1.0f) {
			m_Angle_V = -1.0f;
		}
	}
	if (GetInput(GAME_CONFIG::VIEW_LEFT))	// 右
	{
		m_Angle_H += CAMERA_ANGLE_SPEED;
		if (m_Angle_H >= 3.14f) {
			m_Angle_H -= 6.28f;
		}
	}
	if (GetInput(GAME_CONFIG::VIEW_RIGHT))	// 左
	{
		m_Angle_H -= CAMERA_ANGLE_SPEED;
		if (m_Angle_H <= -3.14f) {
			m_Angle_H += 6.28f;
		}
	}

	VEC3 focusObjPos = m_pFocusObject.lock()->get_Transform().lock()->get_VEC3ToPos();
	focusObjPos += m_FocusOffset;

	// 注視点を設定
	//m_FocusPoint = VEC3::Lerp(m_FocusPoint, focusObjPos, CAMERA_MOVE_FACTOR);	// ガタガタする
	m_FocusPoint = focusObjPos;

	// 方向ベクトルを作る
	VEC3 lookDir;
	lookDir.x = m_PosOffset.x * cosf(m_Angle_V) * cosf(m_Angle_H);
	lookDir.y = m_PosOffset.y * sinf(m_Angle_V);
	lookDir.z = m_PosOffset.z * cosf(m_Angle_V) * sinf(m_Angle_H);

	// カメラの位置
	m_pOwner.lock()->get_Transform().lock()->set_Pos(lookDir + m_FocusPoint);
}


//*---------------------------------------------------------------------------------------
//* @:Camera3D Class 
//*【?】ビュー変換行列の取得
//* 引数：なし
//* 返値：XMMATRIX
//*----------------------------------------------------------------------------------------
XMMATRIX Camera3D::get_ViewMatrix()const
{
	XMFLOAT3 eye = m_pOwner.lock()->get_Transform().lock()->get_VEC3ToPos();;
	XMFLOAT3 foucus = m_FocusPoint;
	XMFLOAT3 upVec = m_UpVec;

	// ビュー行列の作成
	XMMATRIX viewMat = XMMatrixLookAtLH(
		XMLoadFloat3(&eye),
		XMLoadFloat3(&foucus),
		XMLoadFloat3(&upVec)
	);

	return viewMat;
}

void Camera3D::set_UpVec(const VECTOR3::VEC3& upVec)
{
	m_UpVec = upVec;
}

void Camera3D::set_FocusPoint(const VECTOR3::VEC3& focus)
{
	m_FocusPoint = focus;
}

void Camera3D::set_FocusObject(std::weak_ptr<class GameObject> pObj)
{
	m_pFocusObject = pObj;
}

std::string  Camera3D::get_FocusObjectTag()const
{
	return m_pFocusObject.lock()->get_Tag();
}

VECTOR3::VEC3 Camera3D::get_UpVec()const
{
	return m_UpVec;
}

VECTOR3::VEC3 Camera3D::get_FocusPoint()const
{
	return m_FocusPoint;
}

void Camera3D::set_PosOffset(const VECTOR3::VEC3& offset)
{
	m_PosOffset = offset;
}

VECTOR3::VEC3 Camera3D::get_PosOffset()const
{
	return m_PosOffset;
}

void Camera3D::set_FocusOffset(const VECTOR3::VEC3& offset)
{
	m_FocusOffset = offset;
}

VECTOR3::VEC3 Camera3D::get_FocusOffset()const
{
	return m_FocusOffset;
}

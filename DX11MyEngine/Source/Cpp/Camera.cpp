#include "pch.h"
#include "Camera.h"
//#include "SceneManager.h"
//#include "InputFactory.h"
//#include "DirectWriteManager.h"
//
//using namespace DirectX;
//using namespace Input;
//using namespace VECTOR3;
//using namespace VECTOR2;
//
//#define CAMERA_ANGLE_SPEED 0.03f	// カメラの方向転換スピード
//#define CAMERA_MOVE_SPEED  10.0f		// カメラの移動スピード
//
////*---------------------------------------------------------------------------------------
////* @:Camera Class 
////*【?】コンストラクタ
////* 引数：なし
////*----------------------------------------------------------------------------------------
//Camera::Camera():
//	m_EyePos({0.0f,2.0f,-15.0f}),
//	m_FocusPoint({0.0f,0.0f,0.0f}),
//	m_UpVec({0.0f,1.0f,0.0f}),
//	m_Angle_H(1.57f),
//	m_Angle_V(0.f)
//{
//
//}
//
////*---------------------------------------------------------------------------------------
////* @:Camera Class 
////*【?】デストラクタ
////* 引数：なし
////*----------------------------------------------------------------------------------------
//Camera::~Camera()
//{
//
//}
//
//
////*---------------------------------------------------------------------------------------
////* @:Camera Class 
////*【?】初期化
////* 引数：なし
////* 返値：bool
////*----------------------------------------------------------------------------------------
//bool Camera::Init(RendererManager &renderer)
//{
//	return true;
//}
//
//
////*---------------------------------------------------------------------------------------
////* @:Camera Class 
////*【?】更新処理
////* 引数：なし
////* 返値：void
////*----------------------------------------------------------------------------------------
//void Camera::Update(RendererManager &renderer)
//{
//	if (GetInput(CONFIG_INPUT::DOWN))	// 上
//	{
//		m_Angle_V += CAMERA_ANGLE_SPEED;
//		if (m_Angle_V >= 80.f)
//			m_Angle_V = 80.f;
//	}
//	if (GetInput(CONFIG_INPUT::UP))		// 下
//	{
//		m_Angle_V -= CAMERA_ANGLE_SPEED;
//		if (m_Angle_V <= -60.f)
//			m_Angle_V = -60.f;
//	}
//	if (GetInput(CONFIG_INPUT::LEFT))	// 右
//	{
//		m_Angle_H += CAMERA_ANGLE_SPEED;
//		if (m_Angle_H >= 180.f)
//			m_Angle_H -= 360.f;
//	}
//	if (GetInput(CONFIG_INPUT::RIGHT))	// 左
//	{
//		m_Angle_H -= CAMERA_ANGLE_SPEED;
//		if (m_Angle_H <= -180.f)
//			m_Angle_H += 360.0f;
//	}	
//	
//	// 方向ベクトルを作る
//	VEC3 lookDir;
//	lookDir.x = cosf(m_Angle_V) * cosf(m_Angle_H);
//	lookDir.y = sinf(m_Angle_V);
//	lookDir.z = cosf(m_Angle_V) * sin(m_Angle_H);
//
//	// 前方向と右方向ベクトルを作る 
//	// 右方向ベクトルは上方向と前方向ベクトルの外積を取ることでできる
//	VEC3 forward = lookDir.Normalize();
//	VEC3 right = VEC3::Cross(VEC3(0.0f, 1.0f, 0.0f), forward);
//	VEC3 moveDir{ 0.0f,0.0f,0.0f };
//
//	// 移動 
//	if (GetInput(CONFIG_INPUT::MOVE_F)) moveDir = moveDir+ forward;
//	if (GetInput(CONFIG_INPUT::MOVE_B)) moveDir = moveDir- forward;
//	if (GetInput(CONFIG_INPUT::MOVE_R)) moveDir = moveDir+ right;
//	if (GetInput(CONFIG_INPUT::MOVE_L)) moveDir = moveDir- right;
//
//	// 空中浮遊させないように
//	// moveDir.y = 0.0f;
//
//	VEC3 pos = this->get_Transform().lock()->get_VEC3ToPos();
//
//	// Imgui デバッグ
//	static float debugSpeed = 0.1f;
//	Debugger::Instance().BeginDebugWindow("CameraInfo");
//	Debugger::Instance().DG_SliderFloat("AccelerationSpeed:", 1, &debugSpeed, 0.1f, 3.0f);
//	Debugger::Instance().DG_TextValue("CrntMoveSpeed: %f.1", (CAMERA_MOVE_SPEED * debugSpeed));
//	Debugger::Instance().DG_DragVec3("Pos:",   &pos,	  0.1f, -10000.0f, 10000.0f);
//	Debugger::Instance().DG_DragVec3("Focus:", &m_FocusPoint, 0.1f, -10000.0f, 10000.0f);
//	Debugger::Instance().EndDebugWindow();
//
//	if (moveDir.Length() > 0.001) {
//		VEC3 crntPos = this->get_Transform().lock()->get_VEC3ToPos();
//		// 移動ベクトルを加算
//		this->get_Transform().lock()->set_Pos((crntPos + (moveDir.Normalize() * (CAMERA_MOVE_SPEED * debugSpeed))));
//	}
//	
//	// 注視点を設定
//	m_FocusPoint = this->get_Transform().lock()->get_VEC3ToPos() + lookDir;
//}
//
//
////*---------------------------------------------------------------------------------------
////* @:Camera Class 
////*【?】描画
////* 引数：なし
////* 返値：void
////*----------------------------------------------------------------------------------------
//void Camera::Draw(RendererManager &renderer)
//{
//
//}
//
//
////*---------------------------------------------------------------------------------------
////* @:Camera Class 
////*【?】終了
////* 引数：なし
////* 返値：bool
////*----------------------------------------------------------------------------------------
//bool Camera::Term(RendererManager &renderer)
//{
//	return true;
//}
//
//
////*---------------------------------------------------------------------------------------
////* @:Camera Class 
////*【?】ビュー変換行列の取得
////* 引数：なし
////* 返値：XMMATRIX
////*----------------------------------------------------------------------------------------
//XMMATRIX Camera::get_ViewMatrix()const
//{
//	XMFLOAT3 eye    = this->get_Transform().lock()->get_VEC3ToPos();
//	XMFLOAT3 foucut = m_FocusPoint;
//	XMFLOAT3 upVec  = m_UpVec;
//
//	XMMATRIX viewMat = XMMatrixLookAtLH(
//		XMLoadFloat3(&eye),
//		XMLoadFloat3(&foucut),
//		XMLoadFloat3(&upVec)
//	);
//
//	return viewMat;
//}
//

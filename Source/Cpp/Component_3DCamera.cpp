#include "pch.h"
#include "Component_3DCamera.h"
#include "GameObject.h"
#include "InputFactory.h"
#include "DirectWriteManager.h"

using namespace Input;
using namespace VECTOR3;
using namespace VECTOR2;

using namespace DirectX;

#define CAMERA_ANGLE_SPEED 0.03f		// カメラの方向転換スピード
#define CAMERA_MOVE_SPEED  10.0f		// カメラの移動スピード


//*---------------------------------------------------------------------------------------
//* @:Camera3D Class 
//*【?】コンストラクタ
//* 引数：1.オーナーオブジェクト
//* 引数：2.更新レイヤー
//*----------------------------------------------------------------------------------------
Camera3D::Camera3D(std::weak_ptr<GameObject> pOwner, int updateRank) : IComponent(pOwner, updateRank),
m_EyePos({ 0.0f,2.0f,-15.0f }),
m_FocusPoint({ 0.0f,0.0f,0.0f }),
m_UpVec({ 0.0f,1.0f,0.0f }),
m_Angle_H(1.57f),
m_Angle_V(0.f)
{
	this->set_Tag("Camera3D");
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
//* 引数：1.RendererManager
//* 返値：void
//*----------------------------------------------------------------------------------------
void Camera3D::Init(RendererManager& renderer)
{

}


//*---------------------------------------------------------------------------------------
//* @:Camera3D Class 
//*【?】更新処理
//* 引数：1.RendererManager
//* 返値：void
//*----------------------------------------------------------------------------------------
void Camera3D::Update(RendererManager& renderer)
{
	if (GetInput(CONFIG_INPUT::DOWN))	// 上
	{
		m_Angle_V += CAMERA_ANGLE_SPEED;
		if (m_Angle_V >= 1.5f) {
			m_Angle_V = 1.5f;
		}
	}
	if (GetInput(CONFIG_INPUT::UP))		// 下
	{
		m_Angle_V -= CAMERA_ANGLE_SPEED;
		if (m_Angle_V <= -1.3f) {
			m_Angle_V = -1.3f;
		}
	}
	if (GetInput(CONFIG_INPUT::LEFT))	// 右
	{
		m_Angle_H += CAMERA_ANGLE_SPEED;
		if (m_Angle_H >= 3.14f) {
			m_Angle_H -= 6.28f;
		}
	}
	if (GetInput(CONFIG_INPUT::RIGHT))	// 左
	{
		m_Angle_H -= CAMERA_ANGLE_SPEED;
		if (m_Angle_H <= -3.14f) {
			m_Angle_H += 6.28f;
		}
	}

	// 方向ベクトルを作る
	VEC3 lookDir;
	lookDir.x = cosf(m_Angle_V) * cosf(m_Angle_H);
	lookDir.y = sinf(m_Angle_V);
	lookDir.z = cosf(m_Angle_V) * sin(m_Angle_H);

	// 前方向と右方向ベクトルを作る 
	// 右方向ベクトルは上方向と前方向ベクトルの外積を取ることでできる
	VEC3 forward = lookDir.Normalize();
	VEC3 right = VEC3::Cross(m_UpVec, forward);
	VEC3 moveDir{ 0.0f,0.0f,0.0f };

	// 移動 
	if (GetInput(CONFIG_INPUT::MOVE_F)) moveDir = moveDir + forward;
	if (GetInput(CONFIG_INPUT::MOVE_B)) moveDir = moveDir - forward;
	if (GetInput(CONFIG_INPUT::MOVE_R)) moveDir = moveDir + right;
	if (GetInput(CONFIG_INPUT::MOVE_L)) moveDir = moveDir - right;
	if (GetInput(CONFIG_INPUT::JUMP))   moveDir = moveDir + m_UpVec;
	if (GetInput(CONFIG_INPUT::C))   moveDir = moveDir - m_UpVec;

	// 空中浮遊させないように
	//moveDir.y = 0.0f;

	VEC3 pos = m_pOwner.lock()->get_Transform().lock()->get_VEC3ToPos();

	// Imgui デバッグ
	static float debugSpeed = 5.1f;
	Debugger::Instance().BeginDebugWindow("CameraInfo");
	Debugger::Instance().DG_SliderFloat("AccelerationSpeed:", 1, &debugSpeed, 0.1f, 8.0f);
	Debugger::Instance().DG_TextValue("CrntMoveSpeed: %f.1", (CAMERA_MOVE_SPEED * debugSpeed));
	Debugger::Instance().DG_TextValue("Angle_H: %f.1", m_Angle_H);
	Debugger::Instance().DG_TextValue("Angle_V: %f.1", m_Angle_V);
	Debugger::Instance().DG_DragVec3("Pos:", &pos, 0.1f, -10000.0f, 10000.0f);
	Debugger::Instance().DG_DragVec3("Focus:", &m_FocusPoint, 0.1f, -10000.0f, 10000.0f);
	Debugger::Instance().EndDebugWindow();

	if (moveDir.Length() > 0.001) {
		VEC3 crntPos = m_pOwner.lock()->get_Transform().lock()->get_VEC3ToPos();
		// 移動ベクトルを加算
		m_pOwner.lock()->get_Transform().lock()->set_Pos((crntPos + (moveDir.Normalize() * (CAMERA_MOVE_SPEED * debugSpeed))));
	}

	// 注視点を設定
	m_FocusPoint = m_pOwner.lock()->get_Transform().lock()->get_VEC3ToPos() + lookDir;
}


//*---------------------------------------------------------------------------------------
//* @:Camera3D Class 
//*【?】描画処理
//* 引数：1.RendererManager
//* 返値：void
//*----------------------------------------------------------------------------------------
void Camera3D::Draw(RendererManager& renderer)
{

}


//*---------------------------------------------------------------------------------------
//* @:Camera3D Class 
//*【?】ビュー変換行列の取得
//* 引数：なし
//* 返値：XMMATRIX
//*----------------------------------------------------------------------------------------
XMMATRIX Camera3D::get_ViewMatrix()const
{
	XMFLOAT3 eye = m_pOwner.lock()->get_Transform().lock()->get_VEC3ToPos();
	XMFLOAT3 foucut = m_FocusPoint;
	XMFLOAT3 upVec = m_UpVec;

	XMMATRIX viewMat = XMMatrixLookAtLH(
		XMLoadFloat3(&eye),
		XMLoadFloat3(&foucut),
		XMLoadFloat3(&upVec)
	);

	return viewMat;
}
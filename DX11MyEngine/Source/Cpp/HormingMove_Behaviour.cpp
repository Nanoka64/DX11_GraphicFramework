#include "pch.h"
#include "HormingMove_Behaviour.h"


using namespace VECTOR3;
using namespace VECTOR2;
using namespace DirectX;

//*---------------------------------------------------------------------------------------
//*【?】コンストラクタ
//*----------------------------------------------------------------------------------------
HormingMove_Behaviour::HormingMove_Behaviour()
{

}

//*---------------------------------------------------------------------------------------
//*【?】デストラクタ
//*----------------------------------------------------------------------------------------
HormingMove_Behaviour::~HormingMove_Behaviour()
{

}

//*---------------------------------------------------------------------------------------
//*【?】ホーミング移動を行う
//*
//* [引数]
//* _deltaTime  : デルタタイム
//* &_param     : 移動に必要なパラメータをまとめた構造体
//* &_transform : トランスフォームの参照（constなのでsetではなく、行列など必要なパラメータがあるときなどに）
//*
//* [返値]
//* VEC3 : 移動ベクトル
//*----------------------------------------------------------------------------------------
ResultMove HormingMove_Behaviour::MoveCalculate(float _deltaTime, const MoveParam& _param, const MyTransform& _transform)
{
	ResultMove res;

	VEC3 crntPos = _transform.get_VEC3ToPos();
	VEC3 crntRot = _transform.get_VEC3ToRotateToRad();
	VEC3 targetDir = (_param._targetPos - crntPos).Normalize();		// 目標の方向
	float speed = _param._moveSpeed + _param._acceleration;
	if (speed >= _param._maxSpeed)
	{
		speed = _param._maxSpeed;
	}
	VEC3 moveVelocity = targetDir * speed;							// 目標への移動ベクトル


	//目標の方向ベクトルから角度値を算出c
	float targetAngleY = atan2(targetDir.x, targetDir.z);

	// 目標とするクォータニオン
	XMVECTOR targetRotQ = XMQuaternionRotationRollPitchYaw(0.0f, targetAngleY, 0.0f);

	XMVECTOR crntRotQ = _transform.get_RotationQuaternion();

	// クォータニオンの球面線形補間
	// 普通の線形補間だと、値が飛んでしまうためクォータニオンの場合は球面線形補間を使う
	XMVECTOR newRotQ = XMQuaternionSlerp(crntRotQ, targetRotQ, _param._turnSpeed);

	res._moveVelocity = moveVelocity;
	res._RotQ = newRotQ;
	return res;
}
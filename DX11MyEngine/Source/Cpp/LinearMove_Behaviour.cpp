#include "pch.h"
#include "LinearMove_Behaviour.h"

//*---------------------------------------------------------------------------------------
//*【?】コンストラクタ
//*----------------------------------------------------------------------------------------
LinearMove_Behaviour::LinearMove_Behaviour()
{

}

//*---------------------------------------------------------------------------------------
//*【?】デストラクタ
//*----------------------------------------------------------------------------------------
LinearMove_Behaviour::~LinearMove_Behaviour()
{

}

//*---------------------------------------------------------------------------------------
//*【?】直線移動を行う
//*
//* [引数]
//* _deltaTime  : デルタタイム
//* &_param     : 移動に必要なパラメータをまとめた構造体（この移動では方向と速度のみ使う）
//* &_transform : トランスフォームの参照（constなのでsetではなく、行列など必要なパラメータがあるときなどに）
//*
//* [返値]
//* VEC3 : 移動ベクトル
//*----------------------------------------------------------------------------------------
ResultMove LinearMove_Behaviour::MoveCalculate(float _deltaTime, const MoveParam& _param, const class MyTransform& _transform)
{
	ResultMove res;

	// 直線移動の計算
	VECTOR3::VEC3 forward = _param._moveDirection;		// 前方向のベクトルを取得
	float speed = _param._moveSpeed;
	res._moveVelocity = forward * speed;	// 移動量を計算

	return res;
}
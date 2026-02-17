#include "pch.h"
#include "Component_Light.h"

constexpr int DEF_INTENSITY = 1.0f;
constexpr int DEF_COLOR[3] = { 1.0f,1.0f,1.0f };

//*---------------------------------------------------------------------------------------
//* @:Light Class 
//*【?】コンストラクタ
//* 引数：1.オーナーオブジェクト
//* 引数：2.更新レイヤー
//*----------------------------------------------------------------------------------------
Light::Light(std::weak_ptr<GameObject> pOwner, int updateRank) : IComponent(pOwner, updateRank),
m_Intensity(DEF_INTENSITY),
m_LightColor(VECTOR3::VEC3(DEF_COLOR[0], DEF_COLOR[1], DEF_COLOR[2]))
{

};


//*---------------------------------------------------------------------------------------
//* @:Light Class 
//*【?】ライトカラーの設定
//* 引数：1.VECTOR3 カラー値
//* 返値：void
//*----------------------------------------------------------------------------------------
void Light::set_LightColor(const VECTOR3::VEC3 &col)
{
	m_LightColor = col;
}


//*---------------------------------------------------------------------------------------
//* @:Light Class 
//*【?】光の強さ設定
//* 引数：1.強度
//* 返値：void
//*----------------------------------------------------------------------------------------
void Light::set_Intensity(float ins)
{
	m_Intensity = ins;
}

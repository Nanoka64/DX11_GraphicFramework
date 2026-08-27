#include "pch.h"
#include "Component_SpotLight.h"
#include "RendererEngine.h"
#include "GameObject.h"

using namespace VECTOR3;

//*---------------------------------------------------------------------------------------
//* @:SpotLight Class 
//*【?】コンストラクタ
//* 引数：1.オーナーオブジェクト
//* 引数：2.更新レイヤー
//*----------------------------------------------------------------------------------------
SpotLight::SpotLight(std::weak_ptr<GameObject> pOwner, int updateRank)
    : Light(pOwner, updateRank),
    m_Range(0.0f),
    m_Angle(0.0f)
{
    this->set_Tag("SpotLight");
}


//*---------------------------------------------------------------------------------------
//* @:SpotLight Class 
//*【?】デストラクタ
//* 引数：なし
//*----------------------------------------------------------------------------------------
SpotLight::~SpotLight()
{

}


//*---------------------------------------------------------------------------------------
//* @:SpotLight Class 
//*【?】初期化
//* 引数：1.RendererEngine
//* 返値：bool
//*----------------------------------------------------------------------------------------
void SpotLight::Start(RendererEngine& renderer)
{
    m_pOwnerTransform = m_pOwner.lock()->get_Transform();
}


//*---------------------------------------------------------------------------------------
//* @:SpotLight Class 
//*【?】更新
//* 引数：1.RendererEngine
//* 返値：void
//*----------------------------------------------------------------------------------------
void SpotLight::Update(RendererEngine& renderer)
{
    auto pContext = renderer.get_DeviceContext();
    auto transform = m_pOwnerTransform.lock();

    VEC3 pos = transform->get_WorldVEC3ToPos();;
    VEC3 dir = transform->get_WorldForward();

    CB_SpotLightData spotData{};
    // バッファの更新
    spotData.Pos = pos;
    spotData.Range = m_Range;
    spotData.Angle = m_Angle;
    spotData.Direction = dir;
    spotData.SpecularColor = VECTOR3::VEC3(1.0f);
    spotData.SpecularIntensity = 150.0f;
    spotData.DiffuseColor = m_LightColor;
    spotData.DiffuseIntensity = m_Intensity;

    // 情報を設定
    Master::m_pLightManager->set_SpotLightData(spotData);
}

//*---------------------------------------------------------------------------------------
//* @:SpotLight Class 
//*【?】更新
//* 引数：1.影響範囲
//* 引数：2.放射角度
//* 返値：void
//*----------------------------------------------------------------------------------------
void SpotLight::set_SpotLightData(float _range, float _angleDeg)
{
    m_Range = _range;
    m_Angle = Tool::DegToRad(_angleDeg);
}

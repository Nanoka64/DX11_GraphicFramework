#include "pch.h"
#include "Component_PointLight.h"
#include "Component_Transform.h"
#include "Component_DirectionalLight.h"
#include "RendererEngine.h"
#include "GameObjectManager.h"
#include "GameObject.h"
#include "Component_3DCamera.h"

//*---------------------------------------------------------------------------------------
//* @:PointLight Class 
//*【?】コンストラクタ
//* 引数：1.オーナーオブジェクト
//* 引数：2.更新レイヤー
//*----------------------------------------------------------------------------------------
PointLight::PointLight(std::weak_ptr<GameObject> pOwner, int updateRank) : Light(pOwner, updateRank),
m_Range(0.0f)
{

}


//*---------------------------------------------------------------------------------------
//* @:PointLight Class 
//*【?】デストラクタ
//* 引数：なし
//*----------------------------------------------------------------------------------------
PointLight::~PointLight()
{

}


//*---------------------------------------------------------------------------------------
//* @:PointLight Class 
//*【?】初期化
//* 引数：1.RendererEngine
//* 返値：bool
//*----------------------------------------------------------------------------------------
void PointLight::Init(RendererEngine &renderer)
{
    m_pOwnerTransform = m_pOwner.lock()->get_Transform();
}


//*---------------------------------------------------------------------------------------
//* @:PointLight Class 
//*【?】更新
//* 引数：1.RendererEngine
//* 返値：void
//*----------------------------------------------------------------------------------------
void PointLight::Update(RendererEngine &renderer)
{
    auto pContext = renderer.get_DeviceContext();

    CB_POINT_LIGHT pointData{};

    // バッファの更新
    pointData.Pos           = m_pOwnerTransform.lock()->get_VEC3ToPos();
    pointData.DiffuseColor  = m_LightColor;
    pointData.SpecularColor = m_LightColor;
	pointData.Range	        = m_Range;

    // 情報を設定
    Master::m_pLightManager->set_PointLightData(pointData);
}


//*---------------------------------------------------------------------------------------
//* @:PointLight Class 
//*【?】描画
//* 引数：1.RendererEngine
//* 返値：void
//*----------------------------------------------------------------------------------------
void PointLight::Draw(RendererEngine &renderer)
{

}


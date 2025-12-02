#include "pch.h"
#include "Component_Transform.h"
#include "Component_DirectionalLight.h"
#include "RendererEngine.h"
#include "GameObjectManager.h"
#include "GameObject.h"
#include "Component_3DCamera.h"

//*---------------------------------------------------------------------------------------
//* @:DirectionalLight Class 
//*【?】コンストラクタ
//* 引数：1.オーナーオブジェクト
//* 引数：2.更新レイヤー
//*----------------------------------------------------------------------------------------
DirectionalLight::DirectionalLight(std::weak_ptr<GameObject> pOwner, int updateRank) : Light(pOwner, updateRank)
{

}


//*---------------------------------------------------------------------------------------
//* @:DirectionalLight Class 
//*【?】デストラクタ
//* 引数：なし
//*----------------------------------------------------------------------------------------
DirectionalLight::~DirectionalLight()
{

}


//*---------------------------------------------------------------------------------------
//* @:DirectionalLight Class 
//*【?】初期化
//* 引数：1.RendererEngine
//* 返値：bool
//*----------------------------------------------------------------------------------------
void DirectionalLight::Init(RendererEngine &renderer)
{
    m_pOwnerTransform = m_pOwner.lock()->get_Transform();
}


//*---------------------------------------------------------------------------------------
//* @:DirectionalLight Class 
//*【?】更新
//* 引数：1.RendererEngine
//* 返値：void
//*----------------------------------------------------------------------------------------
void DirectionalLight::Update(RendererEngine &renderer)
{
    auto pContext = renderer.get_DeviceContext();

	CB_DIRECTION_LIGHT dirData{};

	dirData.Direction = m_pOwnerTransform.lock()->get_Forward();
    dirData.DiffuseColor  = m_LightColor;
    dirData.SpecularColor = m_LightColor;
	dirData.Intensity     = m_Intensity;

	// 情報を設定
	Master::m_pLightManager->set_DirectionLightData(dirData);
}


//*---------------------------------------------------------------------------------------
//* @:DirectionalLight Class 
//*【?】描画
//* 引数：1.RendererEngine
//* 返値：void
//*----------------------------------------------------------------------------------------
void DirectionalLight::Draw(RendererEngine &renderer)
{

}



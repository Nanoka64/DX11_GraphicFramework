#include "pch.h"
#include "Component_Transform.h"
#include "Component_DirectionalLight.h"
#include "RendererEngine.h"
#include "GameObjectManager.h"
#include "GameObject.h"
#include "Component_3DCamera.h"

using namespace VECTOR3;

//*---------------------------------------------------------------------------------------
//* @:DirectionalLight Class 
//*【?】コンストラクタ
//* 引数：1.オーナーオブジェクト
//* 引数：2.更新レイヤー
//*----------------------------------------------------------------------------------------
DirectionalLight::DirectionalLight(std::weak_ptr<GameObject> pOwner, int updateRank) : Light(pOwner, updateRank)
{
	this->set_Tag("DirectionalLight");
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
	m_UpVec = VEC3(1, 0, 0);
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
	
	auto transform = m_pOwner.lock()->get_Transform().lock();
	VEC3 pos = transform->get_VEC3ToPos();
	VEC3 foward = transform->get_Forward();

	dirData.Direction		  = foward;
    dirData.DiffuseColor	  = m_LightColor;
	dirData.DiffuseIntensity  = m_Intensity;
    dirData.SpecularColor	  = m_LightColor;
    dirData.SpecularIntensity = 0.5f;

	// 注視点
	m_FocusPoint = pos + foward;

	// ライトから見たビュー行列の計算
	XMFLOAT3 eye = pos;
	XMFLOAT3 foucus = m_FocusPoint;
	XMFLOAT3 upVec = m_UpVec;

	XMMATRIX viewMat = XMMatrixLookAtLH(
		XMLoadFloat3(&eye),
		XMLoadFloat3(&foucus),
		XMLoadFloat3(&upVec)
	);

	// 正投影行列を作成する（こっちじゃないとダメっぽい？）
	float width  = 200.0f;  // ライトがカバーする横幅 (メートル単位)
	float height = 200.0f; // ライトがカバーする縦幅
	float nearZ  = 0.1f;    // ライトから見た描画開始距離
	float farZ   = 1000.0f;  // ライトから見た描画終了距離（地球防衛軍なら長めが必要）

	XMMATRIX projMat = XMMatrixOrthographicLH(width, height, nearZ, farZ);
	//XMMATRIX projMat = renderer.get_ProjectionMatrix();
	
	XMMATRIX viewProj = viewMat * projMat;
	XMStoreFloat4x4(&dirData.LightViewProj, XMMatrixTranspose(viewProj));

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



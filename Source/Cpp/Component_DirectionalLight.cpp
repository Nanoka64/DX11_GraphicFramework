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
	m_UpVec = VEC3(0, 1, 0);
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
	
	// トランスフォームの取得
	auto transform = m_pOwner.lock()->get_Transform().lock();

	VEC3 forward = transform->get_Forward();	// 前方
	VEC3 trackingObjPos = m_pLigCamTrackingObj.lock()->get_Transform().lock()->get_VEC3ToPos();
	
	// 正規化しないとデカい影が出てしまう！！
	forward = forward.Normalize();

	dirData.Direction		  = forward;		// 方向
    dirData.DiffuseColor	  = m_LightColor;	// ディフューズ
	dirData.DiffuseIntensity  = m_Intensity;	// つよさ
    dirData.SpecularColor	  = m_LightColor;	// スペキュラ
    dirData.SpecularIntensity = 0.5f;			// つよさ

    // シャドウマップの距離
	float shadowDistance = 4000.0f;

	// 注視点（プレイヤーを見るように）
	m_FocusPoint = trackingObjPos;

	// プレイヤーから少し離れた場所
    VEC3 eyePos = trackingObjPos - (forward * 800.0f);;

	// ライトから見たビュー行列の計算
	XMFLOAT3 eye	= eyePos;
	XMFLOAT3 foucus = m_FocusPoint;
	XMFLOAT3 upVec	= m_UpVec;

    // ビュー行列の作成
	XMMATRIX viewMat = XMMatrixLookAtLH(
		XMLoadFloat3(&eye),
		XMLoadFloat3(&foucus),
		XMLoadFloat3(&upVec)
	);

	// 正投影行列を作成する（ディレクションライトの場合こっちじゃないとダメっぽい？）
	float width  = 500.0f;		// ライトがカバーする横幅 
	float height = 500.0f;		// ライトがカバーする縦幅
	float nearZ  = 1.0f;		// ライトから見た描画開始距離
	float farZ   = shadowDistance;  // ライトから見た描画終了距離

	// 平行投影行列（ディレクションライトはこれがいいらしい）
	XMMATRIX projMat = XMMatrixOrthographicLH(width, height, nearZ, farZ);

    // ビュープロジェクション行列計算し、転置して格納
	XMMATRIX viewProj = viewMat * projMat;
	XMStoreFloat4x4(&dirData.LightViewProj, XMMatrixTranspose(viewProj));

	//Master::m_pDebugger->BeginDebugWindow("DirectionLight");
	//Master::m_pDebugger->DG_BulletText("Forward");
	//Master::m_pDebugger->DG_TextValue("X : %f.1", forward.x);
	//Master::m_pDebugger->DG_TextValue("Y : %f.1", forward.y);
	//Master::m_pDebugger->DG_TextValue("Z : %f.1", forward.z);

	//Master::m_pDebugger->DG_BulletText("FocusPoint");
	//Master::m_pDebugger->DG_TextValue("X : %f.1", m_FocusPoint.x);
	//Master::m_pDebugger->DG_TextValue("Y : %f.1", m_FocusPoint.y);
	//Master::m_pDebugger->DG_TextValue("Z : %f.1", m_FocusPoint.z);
	//Master::m_pDebugger->EndDebugWindow();

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

void DirectionalLight::set_LightCameraTrackingObj(std::shared_ptr<class GameObject> pObj)
{
	m_pLigCamTrackingObj = pObj;
}

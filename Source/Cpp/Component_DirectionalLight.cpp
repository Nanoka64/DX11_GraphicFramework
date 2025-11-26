#include "pch.h"
#include "Component_Transform.h"
#include "Component_DirectionalLight.h"
#include "RendererManager.h"
#include "GameObjectManager.h"
#include "GameObject.h"
#include "Component_3DCamera.h"

//*---------------------------------------------------------------------------------------
//* @:DirectionalLight Class 
//*【?】コンストラクタ
//* 引数：1.オーナーオブジェクト
//* 引数：2.更新レイヤー
//*----------------------------------------------------------------------------------------
DirectionalLight::DirectionalLight(std::weak_ptr<GameObject> pOwner, int updateRank) : Light(pOwner, updateRank),
m_pCBLightSet(nullptr)
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
//* 引数：1.RendererManager
//* 返値：bool
//*----------------------------------------------------------------------------------------
void DirectionalLight::Init(RendererManager &renderer)
{
    m_pOwnerTransform = m_pOwner.lock()->get_Transform();
}


//*---------------------------------------------------------------------------------------
//* @:DirectionalLight Class 
//*【?】更新
//* 引数：1.RendererManager
//* 返値：void
//*----------------------------------------------------------------------------------------
void DirectionalLight::Update(RendererManager &renderer)
{
    auto pContext = renderer.get_DeviceContext();

    // バッファの更新
	m_pCBLightSet->Data.Direction = VECTOR3::VEC3(0, 0, 0);
    m_pCBLightSet->Data.DiffuseColor  = m_LightColor;
    m_pCBLightSet->Data.SpecularColor = m_LightColor;
	m_pCBLightSet->Data.Intensity     = m_Intensity;
    m_pCBLightSet->Data.EyePos		  = m_pCameraTransform.lock()->get_VEC3ToPos();
    pContext->UpdateSubresource(m_pCBLightSet->pBuff, 0, nullptr, &m_pCBLightSet->Data, 0, 0);
    
    // ピクセルシェーダにセット
    pContext->PSSetConstantBuffers(5, 1, &m_pCBLightSet->pBuff);
}


//*---------------------------------------------------------------------------------------
//* @:DirectionalLight Class 
//*【?】描画
//* 引数：1.RendererManager
//* 返値：void
//*----------------------------------------------------------------------------------------
void DirectionalLight::Draw(RendererManager &renderer)
{

}



//*---------------------------------------------------------------------------------------
//* @:Light Class 
//*【?】定数バッファの作成
//* 引数：1.ID3D11Device*
//* 返値：bool
//*----------------------------------------------------------------------------------------
bool DirectionalLight::CreateCBuffer(ID3D11Device *pDevice)
{
	// ライト定数バッファの生成
	m_pCBLightSet = new CB_DIRECTION_LIGHT_SET;
	if (m_pCBLightSet == nullptr) {
		return false;
	}

	// 定数バッファの設定
	D3D11_BUFFER_DESC bd{};
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;						// 標準設定
	bd.ByteWidth = sizeof(CB_DIRECTION_LIGHT);			// バッファのサイズ
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;			// 定数バッファとして使う
	bd.CPUAccessFlags = 0;								// CPUから書き込みしない
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;

	// 定数バッファの生成
	HRESULT hr = pDevice->CreateBuffer(&bd, nullptr, &m_pCBLightSet->pBuff);
	if (FAILED(hr)) {
		return false;
	}

	return true;
}


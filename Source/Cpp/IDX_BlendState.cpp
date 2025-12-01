#include "pch.h"
#include "IDX_BlendState.h"
#include "RendererEngine.h"


IDX_BlendState::IDX_BlendState():
	m_IsIndependentBlendEnable(false)
{
}

IDX_BlendState::~IDX_BlendState()
{

}


//*---------------------------------------------------------------------------------------
//* @:RendererEngine Class 
//*【?】ブレンドステートの作成
//* 引数：1.RendererEngine&
//* 引数：2.BlendSetupDataInfo&
//* 戻値：成功したか
//*----------------------------------------------------------------------------------------
bool IDX_BlendState::Create(RendererEngine &renderer, const BlendSetupDataInfo &_InData)
{
	auto pDevice = renderer.get_Device();

	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));

	blendDesc.IndependentBlendEnable = _InData.independentBlendEnable;
	blendDesc.AlphaToCoverageEnable  = _InData.alphaToCoverageEnable;

	// レンダリングターゲットごとに設定を独立
	if (_InData.independentBlendEnable == TRUE)
	{
		for (int i = 0; i < ARRAYSIZE(blendDesc.RenderTarget); i++)
		{
			blendDesc.RenderTarget[i].BlendEnable			= _InData.renderTargets[i].blendEnable;
			blendDesc.RenderTarget[i].SrcBlend				= _InData.renderTargets[i].srcBlend;
			blendDesc.RenderTarget[i].DestBlend				= _InData.renderTargets[i].destBlend;
			blendDesc.RenderTarget[i].BlendOp				= _InData.renderTargets[i].blendOp;
			blendDesc.RenderTarget[i].SrcBlendAlpha			= _InData.renderTargets[i].srcBlendAlpha;
			blendDesc.RenderTarget[i].DestBlendAlpha		= _InData.renderTargets[i].destBlendAlpha;
			blendDesc.RenderTarget[i].BlendOpAlpha			= _InData.renderTargets[i].blendOpAlpha;
			blendDesc.RenderTarget[i].RenderTargetWriteMask = _InData.renderTargets[i].renderTargetWriteMask;
		}
	}
	else
	{
		blendDesc.RenderTarget[0].BlendEnable			= _InData.renderTargets[0].blendEnable;
		blendDesc.RenderTarget[0].SrcBlend				= _InData.renderTargets[0].srcBlend;
		blendDesc.RenderTarget[0].DestBlend				= _InData.renderTargets[0].destBlend;
		blendDesc.RenderTarget[0].BlendOp				= _InData.renderTargets[0].blendOp;
		blendDesc.RenderTarget[0].SrcBlendAlpha			= _InData.renderTargets[0].srcBlendAlpha;
		blendDesc.RenderTarget[0].DestBlendAlpha		= _InData.renderTargets[0].destBlendAlpha;
		blendDesc.RenderTarget[0].BlendOpAlpha			= _InData.renderTargets[0].blendOpAlpha;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = _InData.renderTargets[0].renderTargetWriteMask;
	}

	// 作成
	HRESULT hr = pDevice->CreateBlendState(&blendDesc, m_pResource.GetAddressOf());
	if (FAILED(hr))return false;

	return true;
}

void IDX_BlendState::Release()
{
}

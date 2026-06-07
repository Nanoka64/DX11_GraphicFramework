#pragma once
#include "IDXResource.h"

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:IConstantBuffer Class --- */
//
//  ★継承：IDXResource ★
//
// 【?】定数バッファクラスのインターフェース
//		
// ***************************************************************************************
class IConstantBuffer : public IDXResource<ID3D11Buffer>
{
public:
	virtual ~IConstantBuffer() = default;
	virtual bool Setup(ID3D11Device* _device, const D3D11_USAGE _usage = D3D11_USAGE_DEFAULT) = 0;
	virtual void Bind(ID3D11DeviceContext* context, UINT slot) = 0;
	virtual void UpdateRaw(ID3D11DeviceContext* context, const void* data, size_t size) = 0;
};
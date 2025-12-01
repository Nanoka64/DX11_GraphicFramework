#include "pch.h"
#include "RenderParam.h"
#include "RendererEngine.h"

//
/* RenderParam Struct - 初期化 - */
//
bool RenderParam::Init(RendererEngine& render)
{
	bool result = InitConstantBuffer(render);
	if (result == false)return false;

	return true;
}


//
/* RenderParam Struct - 終了 - */
//
void RenderParam::Term(RendererEngine& render)
{
	SAFE_RELEASE(cbTransformSet.pBuff);
	SAFE_RELEASE(cbViewSet.pBuff);
	SAFE_RELEASE(cbProjectionSet.pBuff);
}

//
/* RenderParam Struct - 定数バッファの初期化 - */
//
bool RenderParam::InitConstantBuffer(RendererEngine& render)
{
	HRESULT hr = S_OK;

	// 定数バッファの定義
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	// ワールド変換用定数バッファの作成
	desc.ByteWidth = sizeof(CB_TRANSFORM);
	hr = render.get_Device()->CreateBuffer(&desc, NULL, &cbTransformSet.pBuff);
	if (FAILED(hr))return false;

	// ビュー変換用定数バッファの作成
	desc.ByteWidth = sizeof(CB_VIEW);
	hr = render.get_Device()->CreateBuffer(&desc, NULL, &cbViewSet.pBuff);
	if (FAILED(hr))return false;


	// 投影変換用定数バッファの作成
	desc.ByteWidth = sizeof(CB_PROJECTION);
	hr = render.get_Device()->CreateBuffer(&desc, NULL, &cbProjectionSet.pBuff);
	if (FAILED(hr))return false;


	return true;
}

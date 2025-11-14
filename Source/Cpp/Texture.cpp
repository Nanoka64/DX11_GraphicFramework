#include "pch.h"
#include "Texture.h"
#include "RendererManager.h"
#include <wincodec.h>
using namespace DirectX;

Texture::Texture():
	m_pSRV(nullptr)
{

}

Texture::~Texture()
{
	this->Release();
}

HRESULT Texture::Load(const std::wstring& path, RendererManager& renderer)
{
	HRESULT hr = S_OK;
	TexMetadata metadata;	// 画像の情報
	ScratchImage image;		// 画像本体

	// テクスチャの読み込み
	hr = LoadFromWICFile(
		path.c_str(),		// パス
		WIC_FLAGS_NONE,		// フラグ
		&metadata, 
		image
	);

	if (FAILED(hr))return hr;

	auto device = renderer.get_Device();
	
	// シェーダーリソースビューの作成
	// 内部的にTexture2Dを作って色々やってくれている
	hr = CreateShaderResourceView(
		device, 
		image.GetImages(), 
		image.GetImageCount(), 
		metadata, 
		m_pSRV.GetAddressOf()
	);

	if (FAILED(hr))return hr;

	m_FilePath = path.c_str();	// パスを保持

	return hr;
}


void Texture::set_SRV_ComPtr(const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& pSrv)
{
	m_pSRV = pSrv;
}


void Texture::Release()
{
	SAFE_RELEASE(m_pSRV);
}


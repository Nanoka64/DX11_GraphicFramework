#include "pch.h"
#include "Texture.h"
#include "RendererEngine.h"
#include <wincodec.h>
using namespace DirectX;

Texture::Texture():
	m_pSRV(nullptr),
	m_FilePath(),
	m_Width(0),
	m_Height(0)
{
}

Texture::~Texture()
{
	//SAFE_RELEASE(m_pSRV);
}


//*---------------------------------------------------------------------------------------
//*【?】WIC対応ファイルの読み込み 
//* 
//*		対応ファイル : .BMP.PNG.GIF.TIFF.JPEG.GIF.TIFF
//*
//* [引数]
//* path : ファイルパス（ワイド文字列）
//* renderer : デバイス取得用
//*
//* [返値]
//* S_OK : 成功
//* S_FALSE : 失敗
//*----------------------------------------------------------------------------------------
HRESULT Texture::Load_WIC(const std::wstring& path, RendererEngine& renderer)
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

	m_Width = static_cast<UINT>(metadata.width);
	m_Height = static_cast<UINT>(metadata.height);


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


//*---------------------------------------------------------------------------------------
//*【?】キューブマップ用 DDSファイルの読み込み 
//* 
//*		対応ファイル : .DDS
//*
//* [引数]
//* path : ファイルパス（ワイド文字列）
//* renderer : デバイス取得用
//*
//* [返値]
//* S_OK : 成功
//* S_FALSE : 失敗
//*----------------------------------------------------------------------------------------
HRESULT Texture::Load_DDS_CubeMap(const std::wstring& path, RendererEngine& renderer)
{
	HRESULT hr = S_OK;
	TexMetadata metadata;	// 画像の情報
	ScratchImage image;		// 画像本体

	// テクスチャの読み込み
	hr = LoadFromDDSFile(
		path.c_str(),		// パス
		DDS_FLAGS_NONE,		// フラグ
		&metadata,
		image
	);
	if (FAILED(hr))return hr;

	// キューブマップじゃないなら返す
	if (!metadata.IsCubemap() || metadata.arraySize != 6)
	{
		return E_INVALIDARG;
	}


	auto device = renderer.get_Device();

	// テクスチャの作成
	hr = CreateTexture(
		device,
		image.GetImages(),
		image.GetImageCount(),
		metadata,
		(ID3D11Resource**)m_pTexture2D.GetAddressOf()
	);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = metadata.format;
	srvDesc.TextureCube.MipLevels = static_cast<UINT>(metadata.mipLevels);
	srvDesc.TextureCube.MostDetailedMip = 0;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;	// キューブマップ用

	// シェーダリソースビューの作成
	device->CreateShaderResourceView(
		m_pTexture2D.Get(),
		&srvDesc,
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
void Texture::set_Width(UINT w)
{
	m_Width = w;
}
void Texture::set_Height(UINT h)
{
	m_Height = h;
}
UINT Texture::get_Width()const
{
	return m_Width;
}
UINT Texture::get_Height()const
{
	return m_Height;
}

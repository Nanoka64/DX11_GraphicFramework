#include "pch.h"
#include "Component_BillboardResource.h"
#include "pch.h"
#include "ResourceManager.h"
#include "RendererEngine.h"

using namespace BASE_VERTEX;
using namespace GIGA_Engine;


//*---------------------------------------------------------------------------------------
//* @:DirectionalLight Class 
//*【?】コンストラクタ
//* 引数：1.オーナーオブジェクト
//* 引数：2.更新レイヤー
//*----------------------------------------------------------------------------------------
BillboardResource::BillboardResource(std::weak_ptr<GameObject> pOwner, int updateRank) :IComponent(pOwner, updateRank),
m_pVertexBuffer(nullptr),
m_pIndexBuffer(nullptr),
m_pCBTransformSet(nullptr),
m_pCBMaterialDataSet(nullptr),
m_pMeshInfo(nullptr)
{
	this->set_Tag("BillboardResource");


	// TODO:ビットフラグ、関数内で演算が効かない問題を直す
	BitFlag::SetFlag((int)FIXED_AXIS_BITFLAG_X, (int&)m_FixedAxisBitFlag);
	BitFlag::SetFlag((int)FIXED_AXIS_BITFLAG_Y, (int&)m_FixedAxisBitFlag);
	BitFlag::SetFlag((int)FIXED_AXIS_BITFLAG_Z, (int&)m_FixedAxisBitFlag);

}

// ----------------------------------------------------------------------------------------------------------------------
/// <summary>
/// デストラクタ
/// </summary>
BillboardResource::~BillboardResource()
{
	// 頂点バッファの解放
	if (m_pVertexBuffer) {
		m_pVertexBuffer->Release();
		m_pVertexBuffer = nullptr;
	}

	// インデックスバッファの解放
	if (m_pIndexBuffer) {
		m_pIndexBuffer->Release();
		m_pIndexBuffer = nullptr;
	}

	// 定数バッファの解放
	if (m_pCBTransformSet) {
		if (m_pCBTransformSet->pBuff) {
			m_pCBTransformSet->pBuff->Release();
		}
		delete m_pCBTransformSet;
		m_pCBTransformSet = nullptr;
	}

	if (m_pCBMaterialDataSet) {
		if (m_pCBMaterialDataSet->pBuff) {
			m_pCBMaterialDataSet->pBuff->Release();
		}
		delete m_pCBMaterialDataSet;
		m_pCBMaterialDataSet = nullptr;
	}


	// マテリアルの解放
	if (m_pMeshInfo->pMaterials) {
		delete m_pMeshInfo->pMaterials;
		m_pMeshInfo->pMaterials = nullptr;
	}
}

// ----------------------------------------------------------------------------------------------------------------------
//       * IPolyResource Class - 頂点バッファの作成- *
// ----------------------------------------------------------------------------------------------------------------------
bool BillboardResource::CreateVertexBuffer(ID3D11Device* pDevice, const void* pVertices, UINT vertexStride, UINT numVertices)
{
	// 頂点バッファの設定
	D3D11_BUFFER_DESC bd{};
	bd.Usage = D3D11_USAGE_DEFAULT;						// 標準設定
	bd.ByteWidth = vertexStride * numVertices;			// バッファのサイズ
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;			// 頂点バッファとして使う
	bd.CPUAccessFlags = 0;								// CPUから書き込みしない

	// 頂点バッファのデータ初期化構造体
	D3D11_SUBRESOURCE_DATA initData{};
	initData.pSysMem = pVertices;

	// 頂点バッファの生成
	HRESULT hr = pDevice->CreateBuffer(&bd, &initData, &m_pVertexBuffer);
	if (FAILED(hr)) {
		return false;
	}

	return true;
}

// ----------------------------------------------------------------------------------------------------------------------
//       * IPolyResource Class - インデックスバッファの作成- *
// ----------------------------------------------------------------------------------------------------------------------
bool BillboardResource::CreateIndexBuffer(ID3D11Device* pDevice, const void* pIndices, UINT indexStride, UINT numIndices)
{
	// インデックスバッファの設定
	D3D11_BUFFER_DESC bd{};
	bd.Usage = D3D11_USAGE_DEFAULT;						// 標準設定
	bd.ByteWidth = indexStride * numIndices;				// バッファのサイズ
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;				// インデックスバッファとして使う
	bd.CPUAccessFlags = 0;								// CPUから書き込みしない

	// インデックスバッファのデータ初期化構造体
	D3D11_SUBRESOURCE_DATA initData{};
	initData.pSysMem = pIndices;

	// インデックスバッファの生成
	HRESULT hr = pDevice->CreateBuffer(&bd, &initData, &m_pIndexBuffer);
	if (FAILED(hr)) {
		return false;
	}

	return true;
}

// ----------------------------------------------------------------------------------------------------------------------
//       * IPolyResource Class - テクスチャマップ設定 Setup後に呼ぶ *
// ----------------------------------------------------------------------------------------------------------------------
bool BillboardResource::set_TextureMap(TEXTURE_MAP mapType, UINT matIndex, const std::wstring& path)
{
	// テクスチャ読み込み
	auto texture = ResourceManager::Instance().LoadTexture(path);
	if (texture == nullptr) {
		return false;
	}

	// 範囲外アクセスチェック
	if (m_pMeshInfo->NumMaterial <= matIndex) {
		return false;
	}

	// 対応したマップにテクスチャを入れる
	switch (mapType)
	{
	case TEXTURE_MAP_NONE:
		break;
	case TEXTURE_MAP_DIFFUSE:
		m_pMeshInfo->pMaterials[matIndex].Diffuse.Texture = texture;
		break;
	case TEXTURE_MAP_NORMAL:
		m_pMeshInfo->pMaterials[matIndex].Normal.Texture = texture;
		break;
	case TEXTURE_MAP_SPECULAR:
		m_pMeshInfo->pMaterials[matIndex].Specular.Texture = texture;
		break;
	default:
		break;
	}

	return true;
}


// ----------------------------------------------------------------------------------------------------------------------
//       * IPolyResource Class - 定数バッファの作成- *
// ----------------------------------------------------------------------------------------------------------------------
bool BillboardResource::CreateCBuffer(ID3D11Device* pDevice)
{
	// ワールド変換定数バッファの生成
	m_pCBTransformSet = new CB_TRANSFORM_SET;
	if (m_pCBTransformSet == nullptr) {
		return false;
	}

	// 定数バッファの設定
	D3D11_BUFFER_DESC bd{};
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;						// 標準設定
	bd.ByteWidth = sizeof(CB_TRANSFORM);				// バッファのサイズ
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;			// 定数バッファとして使う
	bd.CPUAccessFlags = 0;								// CPUから書き込みしない
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;

	// 定数バッファの生成
	HRESULT hr = pDevice->CreateBuffer(&bd, nullptr, &m_pCBTransformSet->pBuff);
	if (FAILED(hr)) {
		return false;
	}

	// マテリアル情報定数バッファの生成
	m_pCBMaterialDataSet = new CB_MATERIAL_SET;
	if (m_pCBMaterialDataSet == nullptr) {
		return false;
	}

	// 定数バッファの設定
	bd.Usage = D3D11_USAGE_DEFAULT;						// 標準設定
	bd.ByteWidth = sizeof(CB_MATERIAL);					// バッファのサイズ
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;			// 定数バッファとして使う
	bd.CPUAccessFlags = 0;								// CPUから書き込みしない
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;

	// 定数バッファの生成
	hr = pDevice->CreateBuffer(&bd, nullptr, &m_pCBMaterialDataSet->pBuff);
	if (FAILED(hr)) {
		return false;
	}

	return true;
}

//*---------------------------------------------------------------------------------------
//* @:BillboardResource Class 
//*【?】
//* 引数：1.RendererEngine
//* 引数：2.メッシュの種類
//* 引数：3.マテリアル数
//* 引数：4.頂点数
//* 返値：bool
//*----------------------------------------------------------------------------------------
bool BillboardResource::Setup(RendererEngine& renderer, BILLBOARD_USAGE_TYPE type, MATERIAL* materials, UINT materialNum)
{
	auto pDevice = renderer.get_Device();

	/*
	*  メッシュ情報の作成
	*/

	switch (type)
	{
	case BILLBOARD_USAGE_TYPE::SIMPLE:
		// 板ポリでメッシュ作成
		m_pMeshInfo = MeshInfoFactory::CreateQuadInfo(materials, materialNum);	
		break;
	default:
		break;
	}


	// 頂点バッファの作成
	if (!CreateVertexBuffer(pDevice, m_pMeshInfo->pVertices, sizeof(VERTEX), m_pMeshInfo->NumVertex))return false;

	// インデックスバッファの作成
	if (!CreateIndexBuffer(pDevice, m_pMeshInfo->pIndices, sizeof(WORD), m_pMeshInfo->NumIndex)) return false;

	// 定数バッファの作成
	if (!CreateCBuffer(pDevice))	return false;

	return true;
}


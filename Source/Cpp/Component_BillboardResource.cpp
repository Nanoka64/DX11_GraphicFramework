#include "pch.h"
#include "Component_BillboardResource.h"
#include "pch.h"
#include "ResourceManager.h"
#include "RendererEngine.h"

using namespace VERTEX;
using namespace GIGA_Engine;


//*---------------------------------------------------------------------------------------
//* @:DirectionalLight Class 
//*【?】コンストラクタ
//* 引数：1.オーナーオブジェクト
//* 引数：2.更新レイヤー
//*----------------------------------------------------------------------------------------
BillboardResource::BillboardResource(std::weak_ptr<GameObject> pOwner, int updateRank) :IComponent(pOwner, updateRank),
m_pCBTransformSet(nullptr),
m_pCBMaterialDataSet(nullptr)
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
}

// ----------------------------------------------------------------------------------------------------------------------
//       * IPolyResource Class - テクスチャマップ設定 Setup後に呼ぶ *
// ----------------------------------------------------------------------------------------------------------------------
bool BillboardResource::set_TextureMap(TEXTURE_MAP mapType, UINT matIndex, const std::wstring& path)
{
	// テクスチャ読み込み
	auto texture = Master::m_pResourceManager->LoadWIC_Texture(path);
	if (texture == nullptr) {
		return false;
	}

	// 範囲外アクセスチェック
	if (m_pMeshData->NumMaterial <= matIndex) {
		return false;
	}

	// 対応したマップにテクスチャを入れる
	switch (mapType)
	{
	case TEXTURE_MAP_NONE:
		break;
	case TEXTURE_MAP_DIFFUSE:
		m_pMeshData->pMaterials[matIndex].m_DiffuseMap.Texture = texture;
		break;
	case TEXTURE_MAP_NORMAL:
		m_pMeshData->pMaterials[matIndex].m_NormalMap.Texture = texture;
		break;
	case TEXTURE_MAP_SPECULAR:
		m_pMeshData->pMaterials[matIndex].m_SpecularMap.Texture = texture;
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
bool BillboardResource::Setup(RendererEngine& renderer, BILLBOARD_USAGE_TYPE type, Material* materials, UINT materialNum)
{
	auto pDevice = renderer.get_Device();

	/*
	*  メッシュ情報の作成
	*/

	switch (type)
	{
	case BILLBOARD_USAGE_TYPE::SIMPLE:
		// 板ポリでメッシュ作成
		m_pMeshData = MeshInfoFactory::CreateQuadInfo(renderer,materials, materialNum, false);
		break;
	default:
		break;
	}

	// 定数バッファの作成
	if (!CreateCBuffer(pDevice))	return false;

	return true;
}


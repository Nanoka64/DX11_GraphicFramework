#pragma once
#include "IComponent.h"
#include "MeshInfoFactory.h"


/// <summary>
/// 固定軸指定ビットフラグ
/// </summary>
enum FIXED_AXIS_BITFLAG : unsigned char
{
	FIXED_AXIS_BITFLAG_X = 1 << 0,
	FIXED_AXIS_BITFLAG_Y = 1 << 1,
	FIXED_AXIS_BITFLAG_Z = 1 << 2,
};


/// <summary>
/// ビルボードの使用タイプ
/// </summary>
enum class BILLBOARD_USAGE_TYPE
{
	SIMPLE,
};
// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:BillboardResource Class --- */
//
//  ★継承：IComponent ★
//
// 【?】ビルボード描画用の構造を管理するコンポーネント
//
// ***************************************************************************************
class BillboardResource : public IComponent
{
private:
	friend class BillboardRenderer;

	FIXED_AXIS_BITFLAG m_FixedAxisBitFlag;	// 固定軸を決めるためのフラグ（Y軸固定など）


	std::shared_ptr<MeshResourceData> m_pMeshData;						// メッシュ情報
	CB_TRANSFORM_SET* m_pCBTransformSet;		// 定数バッファ(ワールド行列用)
	CB_MATERIAL_SET* m_pCBMaterialDataSet;		// 定数バッファ(マテリアル用)

public:
	BillboardResource(std::weak_ptr<GameObject> pOwner, int updateRank = 100);
	virtual~BillboardResource();

	bool Setup(RendererEngine& renderer, BILLBOARD_USAGE_TYPE usageType, Material* materials, UINT materialNum);	// 初期化


	/// <summary>
	/// テクスチャマップ設定 Setup後に呼ぶ
	/// </summary>
	/// <param name="mapType"></param>
	/// <param name="matIndex"></param>
	/// <param name="path"></param>
	/// <returns></returns>
	virtual bool set_TextureMap(TEXTURE_MAP mapType, UINT matIndex, const std::wstring& path);

	/// <summary>
	/// 定数バッファの作成
	/// </summary>
	/// <param name="pDevice">デバイス</param>
	/// <returns></returns>
	virtual bool CreateCBuffer(ID3D11Device* pDevice);

	UINT get_NumVertex()const { return m_pMeshData->NumVertex; };		// 頂点数取得
	UINT get_NumIndex()const { return m_pMeshData->NumIndex; };			// インデックス数取得
	UINT get_NumMaterial()const { return m_pMeshData->NumMaterial; };	// マテリアル数取得
};



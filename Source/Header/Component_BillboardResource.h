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


	MeshInfo* m_pMeshInfo;						// メッシュ情報
	ID3D11Buffer* m_pVertexBuffer;				// 頂点バッファ
	ID3D11Buffer* m_pIndexBuffer;				// インデックスバッファ
	CB_TRANSFORM_SET* m_pCBTransformSet;		// 定数バッファ(ワールド行列用)
	CB_MATERIAL_SET* m_pCBMaterialDataSet;		// 定数バッファ(マテリアル用)

public:
	BillboardResource(std::weak_ptr<GameObject> pOwner, int updateRank = 100);
	virtual~BillboardResource();

	bool Setup(RendererEngine& renderer, BILLBOARD_USAGE_TYPE usageType, MATERIAL* materials, UINT materialNum);	// 初期化


	/// <summary>
	/// テクスチャマップ設定 Setup後に呼ぶ
	/// </summary>
	/// <param name="mapType"></param>
	/// <param name="matIndex"></param>
	/// <param name="path"></param>
	/// <returns></returns>
	virtual bool set_TextureMap(TEXTURE_MAP mapType, UINT matIndex, const std::wstring& path);


	// ※const void*はどんな型のアドレスでも受け取れる
	/// <summary>
	/// 頂点バッファの作成
	/// </summary>
	/// <param name="pDevice">デバイス</param>
	/// <param name="pVertices">頂点データ配列</param>
	/// <param name="sizeVertex">頂点データのサイズ</param>
	/// <param name="numVertices">頂点の数</param>
	/// <returns></returns>
	virtual bool CreateVertexBuffer(ID3D11Device* pDevice, const void* pVertices, UINT vertexStride, UINT numVertices);


	/// <summary>
	/// インデックスバッファの作成
	/// </summary>
	/// <param name="pDevice">デバイス</param>
	/// <param name="pIndices">インデックスデータ配列</param>
	/// <param name="sizeIndex">インデックスデータのサイズ</param>
	/// <param name="numIndices">インデックスの数</param>
	/// <returns></returns>
	virtual bool CreateIndexBuffer(ID3D11Device* pDevice, const void* pIndices, UINT indexStride, UINT numIndices);


	/// <summary>
	/// 定数バッファの作成
	/// </summary>
	/// <param name="pDevice">デバイス</param>
	/// <returns></returns>
	virtual bool CreateCBuffer(ID3D11Device* pDevice);

	UINT get_NumVertex()const { return m_pMeshInfo->NumVertex; };		// 頂点数取得
	UINT get_NumIndex()const { return m_pMeshInfo->NumIndex; };			// インデックス数取得
	UINT get_NumMaterial()const { return m_pMeshInfo->NumMaterial; };	// マテリアル数取得
};



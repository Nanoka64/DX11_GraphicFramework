#pragma once
#include "IComponent.h"
#include "Mesh.h"

//* =========================================================================
//* - @:UTILITY_MESH_TYPE列挙体 - */
//* 【?】定義済みメッシュの種類
//* =========================================================================
enum class UTILITY_MESH_TYPE
{
	NONE = 0,
	CUBU,
	QUAD,
	SPHERE,
};

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:IMeshResource Class --- */
//
//  ★継承：IComponent ★
//
// 【?】メッシュ構造（データ）の管理
//	※　UnityのMeshFilter的な役割
//
// ***************************************************************************************
class IMeshResource : public IComponent
{
private:
	friend class MeshRenderer;

	MeshInfo *m_pMeshInfo;			// メッシュ情報
	ID3D11Buffer *m_pVertexBuffer;	// 頂点バッファ
	ID3D11Buffer *m_pIndexBuffer;	// インデックスバッファ
	CB_TRANSFORM_SET *m_pCBTransformSet;		// 定数バッファ(ワールド行列用)
	CB_MATERIAL_SET *m_pCBMaterialDataSet;		// 定数バッファ(マテリアル用)

public:
	IMeshResource(std::weak_ptr<GameObject> pOwner, int updateRank = 100);
	virtual~IMeshResource();

	bool Setup(RendererManager& renderer, UTILITY_MESH_TYPE type ,MATERIAL* materials, UINT materialNum);	// 初期化


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


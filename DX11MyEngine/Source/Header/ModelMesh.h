#pragma once
//--------------------------------------------------------------------------------------
//      * Includes *
//--------------------------------------------------------------------------------------

struct aiMesh;


enum class MESH_TYPE
{
	STATIC,		// 静的メッシュ
	STATIC_N,	// 法線マップ付き静的メッシュ
	SKINNED,	// スキンメッシュ
};

// =======================================================================================
//
// *---        ModelMesh Class         ---*
//			   メッシュ情報
//		3Dオブジェクト描画のコア部分
//		頂点データの構築や描画APIを叩く		
// 
// =======================================================================================
class ModelMesh
{
private:
	//VERTEX::VERTEX_Skneed*m_pVertex;	// 頂点データ
	u_int* m_pIndices;		// インデックスデータ
	u_int m_VertexNum;		// 頂点数
	u_int m_IndexNum;		// インデックス数
	UINT m_VertexStride;	// 頂点のストライド（サイズ）

	// バッファリソース
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pIndexBuffer;

	MESH_TYPE m_MeshType;		// メッシュの種類
	bool m_IsVisible;			// メッシュの表示・非表示

public:
	ModelMesh();
	~ModelMesh();

	void Draw(RendererEngine& render);
	void Term();

	bool Setup(RendererEngine& render, aiMesh* pMeshData);
	//void set_Color(const VECTOR4::VEC4& color, int vtxIdx = -1);	// カラーの設定 頂点インデックスの指定
	//VECTOR4::VEC4& get_Color(int vtxIdx = -1)const;					// カラーの取得 頂点インデックスの指定
	unsigned int get_VertexNum()const { return m_VertexNum; };		// メッシュに含まれる頂点数取得
	//VERTEX::VERTEX_Skneed* get_Vertices() const { return m_pVertex; };

	MESH_TYPE get_MeshType() const { return m_MeshType; }			// メッシュの種類取得
	bool get_IsVisible() const { return m_IsVisible; }				// メッシュの表示・非表示取得
	void set_IsVisible(bool isVisible) { m_IsVisible = isVisible; }	// メッシュの表示・非表示設定

private:
	bool CreateVertexBuffer(RendererEngine& render, const void* pVertexData, UINT vertexNum, UINT vertexStride);	// 頂点バッファの作成
	bool CreateIndexBuffer(RendererEngine& render);	// インデックスバッファの作成
	void DestroyVertexBuffer();							// 頂点バッファの削除
	void DestroyIndexBuffer();							// インデックスバッファの削除
	void SetVertexBoneData(VERTEX::VERTEX_Skneed& vertex, int boneId, float weight);	// 頂点へボーン情報をセット
};


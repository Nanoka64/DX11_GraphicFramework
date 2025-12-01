#pragma once
//--------------------------------------------------------------------------------------
//      * Includes *
//--------------------------------------------------------------------------------------

struct aiMesh;

// =======================================================================================
//
// *---        Mesh Class         ---*
//		3Dオブジェクト描画のコア部分
//		頂点データの構築や描画APIを叩く	
// 
// =======================================================================================
class ModelMesh
{
private:
	BASE_VERTEX::MODEL_VERTEX*m_pVertex;	// 頂点データ
	u_int* m_pIndices;		// インデックスデータ
	u_int m_VertexNum;		// 頂点数
	u_int m_IndexNum;		// インデックス数

	// バッファリソース
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pIndexBuffer;


public:
	ModelMesh();
	~ModelMesh();

	void Draw(RendererEngine& render);
	void Term();

	bool Setup(RendererEngine& render, aiMesh* pMeshData);
	void set_Color(const VECTOR4::VEC4& color, int vtxIdx = -1);	// カラーの設定 頂点インデックスの指定
	VECTOR4::VEC4& get_Color(int vtxIdx = -1)const;					// カラーの取得 頂点インデックスの指定
	unsigned int get_VertexNum()const { return m_VertexNum; };		// メッシュに含まれる頂点数取得
	BASE_VERTEX::MODEL_VERTEX* get_Vertices() const { return m_pVertex; };

private:
	bool CreateVertexBuffer(RendererEngine& render);	// 頂点バッファの作成
	bool CreateIndexBuffer(RendererEngine& render);	// インデックスバッファの作成
	void DestroyVertexBuffer();							// 頂点バッファの削除
	void DestroyIndexBuffer();							// インデックスバッファの削除
	void SetVertexBoneData(BASE_VERTEX::MODEL_VERTEX& vertex, int boneId, float weight);	// 頂点へボーン情報をセット
};


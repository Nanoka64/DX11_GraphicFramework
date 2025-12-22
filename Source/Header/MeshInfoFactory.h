#pragma once

/// <summary>
/// 静的メッシュ用
/// 法線マップなし
/// </summary>
//struct Static_MeshInfo
//{
//	TVertex *pVertices;					// 頂点情報
//	UINT NumVertex;						// 頂点数
//	WORD *pIndices;						// インデックス情報
//	UINT NumIndex;						// インデックス数
//	MATERIAL* pMaterials;				// マテリアル
//	UINT NumMaterial;					// マテリアル数
//};

struct MeshResourceData 
{
	ID3D11Buffer* pVertexBuffer;	// 頂点バッファ
	UINT VertexStride;				// 頂点一つのサイズ（sizeof(VERTEX_Static)など）
	UINT NumVertex;					// 頂点数
	ID3D11Buffer* pIndexBuffer;		// インデックスバッファ
	UINT NumIndex;					// インデックス数
	MATERIAL* pMaterials;			// マテリアル
	UINT NumMaterial;				// マテリアル数

	MeshResourceData() :
		pVertexBuffer(nullptr),
		VertexStride(0),
		NumVertex(0),
		pIndexBuffer(nullptr),
		NumIndex(0),
		pMaterials(nullptr),
		NumMaterial(0)
	{};
};

class MeshInfoFactory
{
public:
	MeshInfoFactory();
	~MeshInfoFactory();

	template<typename TVertex>
	static MeshResourceData CreateMesh(ID3D11Device* pDevice,TVertex* pVertices, UINT vNum, WORD* pIndices, UINT iNum)
	{
		MeshResourceData meshData = {};
		meshData.VertexStride = sizeof(TVertex); // ここで型のサイズを固定値にする
		meshData.NumVertex = vNum;
		meshData.NumIndex = iNum;

		/////////////////////////////////////////////////////////////////////
		// 頂点バッファの作成
		/////////////////////////////////////////////////////////////////////
		D3D11_BUFFER_DESC bd{};
		bd.Usage = D3D11_USAGE_DEFAULT;						// 標準設定
		bd.ByteWidth = meshData.VertexStride * vNum;		// バッファのサイズ
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;			// 頂点バッファとして使う
		bd.CPUAccessFlags = 0;								// CPUから書き込みしない

		// 頂点バッファのデータ初期化構造体
		D3D11_SUBRESOURCE_DATA initData{};
		initData.pSysMem = pVertices;

		// 頂点バッファの生成
		HRESULT hr = pDevice->CreateBuffer(&bd, &initData, &meshData.pVertexBuffer);
		if (FAILED(hr)) {
			MessageBoxA(NULL, "頂点バッファの作成に失敗しました。", "Error", MB_OK);
			return meshData;
		}

		/////////////////////////////////////////////////////////////////////
		// インデックスバッファの作成
		/////////////////////////////////////////////////////////////////////
		bd.Usage = D3D11_USAGE_DEFAULT;						// 標準設定
		bd.ByteWidth = sizeof(WORD) * iNum;					// バッファのサイズ
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;				// インデックスバッファとして使う
		bd.CPUAccessFlags = 0;								// CPUから書き込みしない

		// インデックスバッファのデータ初期化構造体
		initData.pSysMem = pIndices;

		// インデックスバッファの生成
		hr = pDevice->CreateBuffer(&bd, &initData, &meshData.pIndexBuffer);
		if (FAILED(hr)) {
			// インデックスバッファに失敗したら、作った頂点バッファを逃がさず消す
			if (meshData.pVertexBuffer) {
				meshData.pVertexBuffer->Release();
				meshData.pVertexBuffer = nullptr;
			}
			MessageBoxA(NULL, "インデックスバッファの作成に失敗しました。", "Error", MB_OK);
			return meshData;
		}

		return meshData;
	}

	static std::shared_ptr<MeshResourceData> CreateQuadInfo(RendererEngine& renderer,MATERIAL *materials, UINT matNum, bool isNormalMap);
	static std::shared_ptr<MeshResourceData> CreateCubeInfo(RendererEngine& renderer,MATERIAL* materials, UINT matNum, bool isNormalMap);
	static std::shared_ptr<MeshResourceData> CreateSphereInfo(RendererEngine& renderer, MATERIAL* materials, UINT matNum, bool isNormalMap);
	static std::shared_ptr<MeshResourceData> CreatePlaneInfo(RendererEngine& renderer, MATERIAL* materials, UINT matNum, bool isNormalMap);
	static std::shared_ptr<MeshResourceData> CreateTriangleInfo(RendererEngine& renderer);
	static std::shared_ptr<MeshResourceData> CreateCylinderInfo(RendererEngine& renderer);
	static std::shared_ptr<MeshResourceData> CreateTorusInfo(RendererEngine& renderer);
	static std::shared_ptr<MeshResourceData> CreateSpriteQuadInfo(RendererEngine& renderer, float w, float h);
	static std::shared_ptr<MeshResourceData> CreateRTSpriteInfo(RendererEngine& renderer, float w, float h);
};

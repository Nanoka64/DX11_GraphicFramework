#include "pch.h"
#include "ModelMesh.h"
#include "Model.h"
#include "RendererManager.h"
#include <assimp/mesh.h>

using namespace BASE_VERTEX;

//*---------------------------------------------------------------------------------------
//* @:ModelMesh Class 
//*【?】コンストラクタ
// 
//* 引数：なし
//*----------------------------------------------------------------------------------------
ModelMesh::ModelMesh():
	m_pVertex(nullptr),
	m_pIndices(nullptr),
	m_VertexNum(0),
	m_IndexNum(0),
	m_pVertexBuffer(nullptr),
	m_pIndexBuffer(nullptr)
{

}


//*---------------------------------------------------------------------------------------
//* @:ModelMesh Class 
//*【?】デストラクタ
// 
//* 引数：なし
//*----------------------------------------------------------------------------------------
ModelMesh::~ModelMesh()
{
	this->Term();
}


//*---------------------------------------------------------------------------------------
//* @:ModelMesh Class 
//*【?】描画
// 
//* 引数：1.RendererManager
//* 返値：void
//*----------------------------------------------------------------------------------------
void ModelMesh::Draw(RendererManager& render)
{
	auto pDeviceContext = render.get_DeviceContext();
	uint32_t stride[1] = { sizeof(MODEL_VERTEX) };
	uint32_t offset[1] = { 0 };

	pDeviceContext->UpdateSubresource(m_pVertexBuffer, 0, NULL, m_pVertex, 0, 0);

	// 頂点・インデックスバッファを設定
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, stride, offset);
	pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// レイアウト設定
	//pDeviceContext->IASetInputLayout(render.get_ShaderData().m_pVertexLayout);

	// index
	pDeviceContext->DrawIndexed(m_IndexNum, 0, 0);
}


//*---------------------------------------------------------------------------------------
//* @:ModelMesh Class 
//*【?】終了
//* 引数：なし
//* 返値：void
//*----------------------------------------------------------------------------------------
void ModelMesh::Term()
{
	DestroyVertexBuffer();
	DestroyIndexBuffer();
	SAFE_DELETE_ARRAY(m_pVertex);
	m_VertexNum = 0;
	SAFE_DELETE_ARRAY(m_pIndices);
	m_IndexNum = 0;

}


//*---------------------------------------------------------------------------------------
//* @:ModelMesh Class 
//*【?】メッシュのセットアップ
// 
//* 引数：1.RendererManager
//* 引数：2.モデルクラスで抽出したメッシュ情報 
//* 引数：3.モデルクラスのポインタ（今は使ってない）
//* 返値：bool
//*----------------------------------------------------------------------------------------
bool ModelMesh:: Setup(RendererManager& render, aiMesh* pMeshData)
{
	aiVector3D zero3D(0.0f, 0.0f, 0.0f);
	aiColor4D oneColor(1.0f, 1.0f, 1.0f, 1.0f);

	// 頂点データ取得
	m_VertexNum = pMeshData->mNumVertices;	// メッシュの頂点数取得
	m_pVertex   = new MODEL_VERTEX[m_VertexNum];	// 取得した頂点数分、頂点データ作る mVerticesはm_VertexNum分存在する

	// 頂点数分、データを格納
	for (u_int vertexIdx = 0; vertexIdx < m_VertexNum; ++vertexIdx)
	{
		/* 
		* サイト：https://qiita.com/kyooooooooma/items/c43dd8b96cc104cb6713
		* ↓この辺から
		* void AssimpLoader::LoadMesh(ModelMesh& dst, const aiMesh* src, bool inverseU, bool inverseV)
		*/

		/*********** 頂点情報の取得 ***********/
		auto pos   = pMeshData->HasPositions()		? &(pMeshData->mVertices[vertexIdx]) : &zero3D;			// 頂点の座標を取得
		auto norm  = pMeshData->HasNormals()		? &(pMeshData->mNormals[vertexIdx]) : &zero3D;			// 法線情報を取得
		auto uv    = pMeshData->HasTextureCoords(0) ? &(pMeshData->mTextureCoords[0][vertexIdx]) : &zero3D; // HasTextureCoords : uv情報があればtrue、無ければfalseをかえす
		auto color = pMeshData->HasVertexColors(0)  ? &(pMeshData->mColors[0][vertexIdx]) : &oneColor;		// カラー情報の取得
		auto tan   = pMeshData->HasTangentsAndBitangents() ? &(pMeshData->mTangents[vertexIdx]) : &zero3D;
		auto biN   = pMeshData->HasTangentsAndBitangents() ? &(pMeshData->mBitangents[vertexIdx]) : &zero3D;

		m_pVertex[vertexIdx].pos	  = VEC3(pos->x, pos->y, pos->z);					// 取得した座標を設定
		m_pVertex[vertexIdx].color	  = VEC4(color->r, color->g, color->b, color->a);	// カラー設定
		m_pVertex[vertexIdx].normal	  = VEC3(norm->x, norm->y, norm->z);				// 法線情報を取得 
		m_pVertex[vertexIdx].uv		  = VEC2(uv->x, uv->y);// UV情報を取得  (モデルデータとDirectXの座標系の違いでV軸を反転させないと上手く表示されない ※ロード時のフラグで解決) 
		m_pVertex[vertexIdx].tangent  = VEC3(tan->x,tan->y,tan->z);
		m_pVertex[vertexIdx].biNormal = VEC3(biN->x, biN->y, biN->z);


		// --- ボーン情報の初期化 ---
		for (int i = 0; i < 4; ++i) {
			m_pVertex[vertexIdx].boneIDs[i] = -1;
			m_pVertex[vertexIdx].boneWeights[i] = 0.0f;
		}
	}

	// メッシュに含まれるボーン情報を頂点に関連付ける
	if (pMeshData->HasBones())
	{
		for (UINT boneIdx = 0; boneIdx < pMeshData->mNumBones; boneIdx++)
		{
			aiBone* bone = pMeshData->mBones[boneIdx];
			for (UINT weightIdx = 0; weightIdx < bone->mNumWeights; weightIdx++)
			{
				UINT vtxId = bone->mWeights[weightIdx].mVertexId;	// ボーンの影響を受ける頂点インデックス
				float weight = bone->mWeights[weightIdx].mWeight;	// ボーンウェイト

				SetVertexBoneData(m_pVertex[vtxId], boneIdx, weight);	// 頂点へボーン情報をセット
			}
		}
	}

	// 頂点バッファの作成
	if (CreateVertexBuffer(render) == false)
	{
		MessageBox(NULL, "頂点バッファが作成できませんでした", "Error", MB_OK);
		return false;
	}

	// 頂点インデックスデータ取得（TriangleList前提）
	m_IndexNum = pMeshData->mNumFaces * 3;	// メッシュに含まれるプリミティブの数
	m_pIndices = new u_int[m_IndexNum];

	for (u_int faceIdx = 0; faceIdx < pMeshData->mNumFaces; ++faceIdx)
	{
		auto& face = pMeshData->mFaces[faceIdx];	// 面データを取得する
		assert(face.mNumIndices == 3);

		for (u_int idx = 0; idx < 3; ++idx)
		{
			/*
			* @ mIndices
			* インデックス配列へのポインタ
			*/
			m_pIndices[faceIdx * 3 + idx] = face.mIndices[idx];
		}
	}

	// インデックスバッファの作成
	if (CreateIndexBuffer(render) == false)
	{
		return false;
	}

	return true;
}


//*---------------------------------------------------------------------------------------
//* @:ModelMesh Class 
//*【?】頂点へボーン情報をセット
// 
//* 引数：1.頂点の参照
//* 引数：2.影響を与えるボーンID
//* 引数：3.影響度
//* 返値：void
//*----------------------------------------------------------------------------------------
void ModelMesh::SetVertexBoneData(BASE_VERTEX::MODEL_VERTEX& vertex, int boneId, float weight)
{
	for (int i = 0; i < 4; ++i)
	{
		if (vertex.boneIDs[i] < 0)
		{
			vertex.boneIDs[i] = boneId;
			vertex.boneWeights[i] = weight;
			return;
		}
		else
		{
			if (vertex.boneWeights[i] < weight)
			{
				vertex.boneIDs[i] = boneId;
				vertex.boneWeights[i] = weight;
				return;
			}
		}
	}
	// 4つ以上のボーンが影響を与える場合は無視
}


//*---------------------------------------------------------------------------------------
//* @:ModelMesh Class 
//*【?】頂点カラーの設定
// 
//* 引数：1.カラー値
//* 引数：2.頂点インデックス
//* 返値：bool
//*----------------------------------------------------------------------------------------
void ModelMesh::set_Color(const VECTOR4::VEC4& color, int vtxIdx )
{
	// メッシュインデックスが指定されていなければ全てに適用
	if (vtxIdx == -1){
		// 頂点数分、データを格納
		for (u_int vertexIdx = 0; vertexIdx < m_VertexNum; ++vertexIdx)
		{
			m_pVertex[vertexIdx].color = color;
		}
	}
	else {	// 指定メッシュのカラー設定
		m_pVertex[vtxIdx].color = color;
	}
}


//*---------------------------------------------------------------------------------------
//* @:ModelMesh Class 
//*【?】頂点カラーの取得 頂点指定がなければ適当に０番目の頂点カラー返す
// 
//* 引数：1.頂点インデックス
//* 返値：VEC4&
//*----------------------------------------------------------------------------------------
VEC4& ModelMesh::get_Color(int vtxIdx)const
{
	if (vtxIdx == -1)
	{
		return m_pVertex[0].color;
	}

	return m_pVertex[vtxIdx].color;
}


//*---------------------------------------------------------------------------------------
//* @:ModelMesh Class 
//*【?】頂点バッファの作成
// 
//* 引数：1.RendererManager
//* 返値：bool
//*----------------------------------------------------------------------------------------
bool ModelMesh::CreateVertexBuffer(RendererManager& render)
{
	// リソース設定
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Usage	   = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(MODEL_VERTEX) * m_VertexNum;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	// サブリソース設定
	D3D11_SUBRESOURCE_DATA vertexSubData;
	ZeroMemory(&vertexSubData, sizeof(vertexSubData));
	vertexSubData.pSysMem = m_pVertex;

	// 頂点バッファ作成
	auto hr = render.get_Device()->CreateBuffer(
		&desc,
		&vertexSubData,
		&m_pVertexBuffer
	);

	if (FAILED(hr))return false;

	return true;

}


//*---------------------------------------------------------------------------------------
//* @:ModelMesh Class 
//*【?】インデックスバッファ作成
// 
//* 引数：1.RendererManager
//* 返値：bool
//*----------------------------------------------------------------------------------------
bool ModelMesh::CreateIndexBuffer(RendererManager& render)
{
	// リソース設定
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Usage		= D3D11_USAGE_DEFAULT;
	desc.ByteWidth = m_IndexNum * sizeof(u_int);
	desc.BindFlags  = D3D11_BIND_INDEX_BUFFER;

	// サブリソース設定
	D3D11_SUBRESOURCE_DATA indexSubData;
	ZeroMemory(&indexSubData, sizeof(indexSubData));
	indexSubData.pSysMem = m_pIndices;

	// インデックスバッファ作成
	auto hr = render.get_Device()->CreateBuffer(
		&desc,
		&indexSubData,
		&m_pIndexBuffer
	);

	if (FAILED(hr))return false;

	return true;
}


//*---------------------------------------------------------------------------------------
//* @:ModelMesh Class 
//*【?】頂点バッファの削除
// 
//* 引数：なし
//* 返値：void
//*----------------------------------------------------------------------------------------
void ModelMesh::DestroyVertexBuffer()
{
	SAFE_RELEASE(m_pVertexBuffer);
}


//*---------------------------------------------------------------------------------------
//* @:ModelMesh Class 
//*【?】インデックスバッファの削除
// 
//* 引数：なし
//* 返値：void
//*----------------------------------------------------------------------------------------
void ModelMesh::DestroyIndexBuffer()
{
	SAFE_RELEASE(m_pIndexBuffer);
}










#include "pch.h"
#include "DebugMesh.h"
#include "RendererEngine.h"
#include "MeshInfoFactory.h"

//*---------------------------------------------------------------------------------------
//*【?】コンストラクタ
//*----------------------------------------------------------------------------------------
DebugMesh::DebugMesh():
m_VertexNum(0),
m_IndexNum(0),
m_MeshType(DEBUG_MESHS_TYPE::CUBE)
{
}

//*---------------------------------------------------------------------------------------
//*【?】デストラクタ
//*----------------------------------------------------------------------------------------
DebugMesh::~DebugMesh()
{
}

//*---------------------------------------------------------------------------------------
//*【?】作成
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//* _type : メッシュタイプ
//*
//* [返値]
//* true : 成功
//* false : 失敗
//*----------------------------------------------------------------------------------------
bool DebugMesh::Setup(RendererEngine& renderer, DEBUG_MESHS_TYPE _type)
{
	if (!CreateVertexIndexBuffer(renderer, g_CubeVertices, g_CubeVertexNum, g_CubeIndices, g_CubeIndexNum) )
	{
		MessageBoxA(NULL, "頂点バッファの作成に失敗しました。", "Error", MB_OK);
		return false;
	}
	return true;
}



//*---------------------------------------------------------------------------------------
//*【?】描画
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//*
//* [返値] なし
//*----------------------------------------------------------------------------------------
void DebugMesh::Draw(RendererEngine& renderer, const DirectX::XMMATRIX& world)
{
	auto pContext = renderer.get_DeviceContext();
	CB_TRANSFORM cbTransform{};

	Master::m_pShaderManager->DeviceToSetShader(SHADER_TYPE::DEFERRED_STD_STATIC);

	/* ========== 定数バッファの更新 ========== */
	// ワールド行列セット ==========================
	XMMATRIX mtx = XMMatrixTranspose(world);        // 行列の転置
	XMStoreFloat4x4(&cbTransform.WorldMtx, mtx);  // XMMATRIX → XMFLOAT4X4変換

	// 定数バッファに転送
	Master::m_pShaderManager->BindConstantBuffer(CONSTANT_BUFFER_TYPE::TRANSFORM, (void*)&cbTransform, sizeof(CB_TRANSFORM));

	// 頂点＆インデックスバッファ設定 ==========================
	UINT offset = 0;
	UINT stride = sizeof(VERTEX::VERTEX_Static);
	pContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &stride, &offset);      // 頂点バッファをセット
	pContext->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);           // インデックスバッファをセット
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	// 描画コール ==========================
	pContext->DrawIndexed(m_IndexNum, 0, 0);
}


//*---------------------------------------------------------------------------------------
//*【?】頂点・インデックスバッファの作成
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//* pVertices : 頂点情報
//* vNum      : 頂点数
//* pIndices  : インデックス情報
//* iNum      : インデックス数
//*
//* [返値] 
//* true : 成功
//* false : 失敗
//*----------------------------------------------------------------------------------------
bool DebugMesh::CreateVertexIndexBuffer(RendererEngine& renderer,const VERTEX::VERTEX_Static* pVertices, UINT vNum, const WORD* pIndices, UINT iNum)
{
	auto pDevice = renderer.get_Device();
	m_VertexNum = vNum;
	m_IndexNum = iNum;

	/////////////////////////////////////////////////////////////////////
	// 頂点バッファの作成
	/////////////////////////////////////////////////////////////////////
	D3D11_BUFFER_DESC bd{};
	bd.Usage = D3D11_USAGE_DEFAULT;						// 標準設定
	bd.ByteWidth = sizeof(VERTEX::VERTEX_Static) * vNum;// バッファのサイズ
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;			// 頂点バッファとして使う
	bd.CPUAccessFlags = 0;								// CPUから書き込みしない

	// 頂点バッファのデータ初期化構造体
	D3D11_SUBRESOURCE_DATA initData{};
	initData.pSysMem = pVertices;

	// 頂点バッファの生成
	HRESULT hr = pDevice->CreateBuffer(&bd, &initData, m_pVertexBuffer.GetAddressOf());
	if (FAILED(hr)) {
		MessageBoxA(NULL, "頂点バッファの作成に失敗しました。", "Error", MB_OK);
		return false;
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
	hr = pDevice->CreateBuffer(&bd, &initData, m_pIndexBuffer.GetAddressOf());
	if (FAILED(hr)) {
		// インデックスバッファに失敗したら、作った頂点バッファを消す
		if (m_pIndexBuffer) {
			m_pIndexBuffer->Release();
			m_pIndexBuffer = nullptr;
		}
		MessageBoxA(NULL, "インデックスバッファの作成に失敗しました。", "Error", MB_OK);
		return false;
	}

	return true;
}

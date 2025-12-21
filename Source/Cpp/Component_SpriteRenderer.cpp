#include "pch.h"
#include "Component_SpriteRenderer.h"
#include "GameObject.h"
#include "RendererEngine.h"
#include "Texture.h"
#include "MeshFactory.h"

using namespace DirectX;
using namespace VERTEX;
using namespace Tool::UV;

//*---------------------------------------------------------------------------------------
//* @:SpriteRenderer Class 
//*【?】コンストラクタ
//* 引数：1.オーナーオブジェクト
//* 引数：2.更新レイヤー
//*----------------------------------------------------------------------------------------
SpriteRenderer::SpriteRenderer(std::weak_ptr<GameObject> pOwner, int updateRank) : IComponent(pOwner, updateRank),
m_Width(0.0f),
m_Height(0.0f),
m_pVertexBuffer(nullptr),
m_pIndexBuffer(nullptr),
m_pCBTransformSet(nullptr),
m_pMeshInfo(nullptr),
m_ShaderType(SHADER_TYPE::NONE),
m_pVSUserExpandCBuffers(nullptr),
m_pPSUserExpandCBuffers(nullptr),
m_VSUserExpandCBNum(0),
m_PSUserExpandCBNum(0)
{
    this->set_Tag("SpriteRenderer");
}


//*---------------------------------------------------------------------------------------
//* @:SpriteRenderer Class 
//*【?】デストラクタ
//*----------------------------------------------------------------------------------------
SpriteRenderer::~SpriteRenderer()
{

}


//*---------------------------------------------------------------------------------------
//* @:SpriteRenderer Class 
//*【?】初期化
//* 引数：1.RendererEngine
//* 返値：void
//*----------------------------------------------------------------------------------------
void SpriteRenderer::Init(RendererEngine &renderer)
{

}


//*---------------------------------------------------------------------------------------
//* @:SpriteRenderer Class 
//*【?】更新
//* 引数：1.RendererEngine
//* 返値：void
//*----------------------------------------------------------------------------------------
void SpriteRenderer::Update(RendererEngine &renderer)
{

}


//*---------------------------------------------------------------------------------------
//* @:SpriteRenderer Class 
//*【?】描画
//* 引数：1.RendererEngine
//* 返値：void
//*----------------------------------------------------------------------------------------
void SpriteRenderer::Draw(RendererEngine &renderer)
{
    auto pContext = renderer.get_DeviceContext();

    // シェーダセット ==========================
	Master::m_pShaderManager->DeviceToSetShader(m_ShaderType);
	
	// 頂点情報の更新
	VertexUpdate(renderer);

    // テクスチャセット ==========================
	for (auto it = m_pTextureMap.begin(); it != m_pTextureMap.end(); it++)
	{
		int slot = it->first;
		ID3D11ShaderResourceView *srv = it->second.lock()->get_SRV();
		pContext->PSSetShaderResources(slot, 1, &srv);
		pContext->VSSetShaderResources(slot, 1, &srv);
	}


	// 定数バッファセット ==========================

	// 頂点シェーダ
	if (m_VSUserExpandCBNum > 0)
	{
		for (int i = 0; i < m_VSUserExpandCBNum; i++)
		{
			int slot = m_pVSUserExpandCBuffers[i].Slot;
			pContext->VSSetConstantBuffers(slot, 1, &m_pVSUserExpandCBuffers[i].pBuff);
		}
	}

	// ピクセルシェーダ
	if (m_PSUserExpandCBNum > 0)
	{
		for (int i = 0; i < m_PSUserExpandCBNum; i++)
		{
			int slot = m_pPSUserExpandCBuffers[i].Slot;
			pContext->PSSetConstantBuffers(slot, 1, &m_pPSUserExpandCBuffers[i].pBuff);
		}
	}

    // 頂点＆インデックスバッファ設定 ==========================
    UINT stride = sizeof(VERTEX_Static);
    UINT offset = 0;
    pContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset); // 頂点バッファをセット
    pContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);    // インデックスバッファをセット
    pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);// Set primitive topology 頂点の組み合わせ方

    // 描画コール：インデックス数は6（三角形2個 × 3頂点） ==========================
    pContext->DrawIndexed(6, 0, 0);
}


//*---------------------------------------------------------------------------------------
//* @:SpriteRenderer Class 
//*【?】スプライトのセットアップ
//* 引数：1.RendererEngine
//* 引数：2. std::weak_ptr<class Texture>
//* 引数：3. 幅
//* 引数：4. 高さ
//* 返値：bool
//*----------------------------------------------------------------------------------------
bool SpriteRenderer::Setup(const CreateSpriteInfo& info)
{
	auto pDevice = info.pRenderer->get_Device();

	m_Width = info.Width;
	m_Height = info.Height;
	m_pTextureMap = info.pTextureMap;
	m_ShaderType = info.ShaderType;

	// ユーザー拡張用定数バッファ数の取得
	m_VSUserExpandCBNum = info.VSConstBufferNum;
	m_PSUserExpandCBNum = info.PSConstBufferNum;
	
	// 頂点シェーダー用ユーザー拡張用定数バッファ情報のセットアップ
	if (m_VSUserExpandCBNum > 0)
	{
		m_pVSUserExpandCBuffers = new CB_USER_EXPAND_SET[m_VSUserExpandCBNum];

		// ユーザー拡張用定数バッファ情報のセットアップ
		for (int i = 0; i < m_VSUserExpandCBNum; i++)
		{
			m_pVSUserExpandCBuffers[i].pData = info.pVSConstantBuffers[i].pUserExpandConstantBuffer;
			m_pVSUserExpandCBuffers[i].Size = info.pVSConstantBuffers[i].UserExpandConstantBufferSize;
			m_pVSUserExpandCBuffers[i].Slot = info.pVSConstantBuffers[i].SetSlot;
		}
	}

	// ピクセルシェーダー用ユーザー拡張用定数バッファ情報のセットアップ
	if (m_PSUserExpandCBNum > 0)
	{
		m_pPSUserExpandCBuffers = new CB_USER_EXPAND_SET[m_PSUserExpandCBNum];
	
		// ユーザー拡張用定数バッファ情報のセットアップ
		for (int i = 0; i < m_PSUserExpandCBNum; i++)
		{
			m_pPSUserExpandCBuffers[i].pData = info.pPSConstantBuffers[i].pUserExpandConstantBuffer;
			m_pPSUserExpandCBuffers[i].Size = info.pPSConstantBuffers[i].UserExpandConstantBufferSize;
			m_pPSUserExpandCBuffers[i].Slot = info.pPSConstantBuffers[i].SetSlot;
		}
	}

	// クアッド生成
	switch (info.Type)
	{
	case SPRITE_USAGE_TYPE::NORMAL:
		m_pMeshInfo = MeshInfoFactory::CreateSpriteQuadInfo(m_Width, m_Height);
		break;
	case SPRITE_USAGE_TYPE::RENDER_TARGET:
		m_pMeshInfo = MeshInfoFactory::CreateRTSpriteInfo(m_Width, m_Height);
		break;
	default:
		break;
	}

	if (m_pMeshInfo == nullptr){
		assert(false);
		return false;
	}

	// 各バッファの生成
	// 頂点バッファの作成
	if (!CreateVertexBuffer(pDevice, m_pMeshInfo->pVertices, sizeof(VERTEX_Static), m_pMeshInfo->NumVertex)) {
		return false;
	}
	// インデックスバッファの作成
	if (!CreateIndexBuffer(pDevice, m_pMeshInfo->pIndices, sizeof(WORD), m_pMeshInfo->NumIndex)) {
		return false;
	}
	// 定数バッファの作成
	if (!CreateCBuffer(pDevice)) {
		return false;
	}
	// ユーザー拡張用定数バッファの作成 
	if (m_PSUserExpandCBNum != 0) {
		if (!CreateUserExpandCBuffer(*info.pRenderer, m_pPSUserExpandCBuffers)) {
			return false;
		}
	}	
	if (m_VSUserExpandCBNum != 0) {
		if (!CreateUserExpandCBuffer(*info.pRenderer, m_pVSUserExpandCBuffers)) {
			return false;
		}
	}

	return true;
}


//*---------------------------------------------------------------------------------------
//* @:SpriteRenderer Class 
//*【?】頂点情報の更新
//* 引数：なし
//* 返値：void
//*----------------------------------------------------------------------------------------
void SpriteRenderer::VertexUpdate(RendererEngine& renderer)
{
	auto pContext = renderer.get_DeviceContext();

	float hw = m_Width;
	float hh = m_Height;
	VEC3 centerPos = m_pOwner.lock()->get_Transform().lock()->get_VEC3ToPos();

	m_pMeshInfo->pVertices[0].pos = VEC3(centerPos.x - hw, centerPos.y - hh,  0.0f);
	m_pMeshInfo->pVertices[1].pos = VEC3(centerPos.x + hw, centerPos.y - hh,  0.0f);
	m_pMeshInfo->pVertices[2].pos = VEC3(centerPos.x - hw, centerPos.y + hh,  0.0f);
	m_pMeshInfo->pVertices[3].pos = VEC3(centerPos.x + hw, centerPos.y + hh,  0.0f);
	pContext->UpdateSubresource(m_pVertexBuffer, 0, nullptr, m_pMeshInfo->pVertices, 0, 0);
}

// ----------------------------------------------------------------------------------------------------------------------
//       * IPolyResource Class - 頂点バッファの作成- *
// ----------------------------------------------------------------------------------------------------------------------
bool SpriteRenderer::CreateVertexBuffer(ID3D11Device *pDevice, const void *pVertices, UINT vertexStride, UINT numVertices)
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
bool SpriteRenderer::CreateIndexBuffer(ID3D11Device *pDevice, const void *pIndices, UINT indexStride, UINT numIndices)
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
//       * IPolyResource Class - 定数バッファの作成- *
// ----------------------------------------------------------------------------------------------------------------------
bool SpriteRenderer::CreateCBuffer(ID3D11Device *pDevice)
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

	return true;
}


//*---------------------------------------------------------------------------------------
//*【?】ユーザー定義拡張用定数バッファの作成
//*
//* [引数]
//* ID3D11Device* : 生成時に使用するデバイス
//* const CreateSpriteInfo& : スプライト生成情報
//* [返値]
//* true : 成功
//* false : 失敗
//*----------------------------------------------------------------------------------------
bool SpriteRenderer::CreateUserExpandCBuffer(RendererEngine& renderer, CB_USER_EXPAND_SET* cbData)
{
	auto pDevice = renderer.get_Device();
	auto pContext = renderer.get_DeviceContext();

	// 定数バッファの設定
	D3D11_BUFFER_DESC bd{};
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;						// 動的に更新
	bd.ByteWidth = cbData->Size;							// バッファのサイズ
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;			// 定数バッファとして使う
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;			// CPU書き込み
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;

	// 定数バッファの生成
	HRESULT hr = pDevice->CreateBuffer(&bd, nullptr, &cbData->pBuff);
	if (FAILED(hr)) {
		return false;
	}

	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// GPUメモリにアクセス
	pContext->Map(cbData->pBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	// データのコピー 
	memcpy(mappedResource.pData, cbData->pData, cbData->Size);

	// アクセス終了
	pContext->Unmap(cbData->pBuff, 0);

	return true;
}


//*---------------------------------------------------------------------------------------
//* @:SpriteRenderer Class 
//*【?】スプライトの横幅セット
//* 引数：横幅
//* 返値：void
//*----------------------------------------------------------------------------------------
void SpriteRenderer::set_Width(float w)
{
	m_Width = w;
}

//*---------------------------------------------------------------------------------------
//* @:SpriteRenderer Class 
//*【?】スプライトの縦幅セット
//* 引数：縦幅
//* 返値：void
//*----------------------------------------------------------------------------------------
void SpriteRenderer::set_Height(float h)
{
	m_Height = h;
}


//*---------------------------------------------------------------------------------------
//* @:SpriteRenderer Class 
//*【?】スプライトの縦幅ゲット
//* 引数：なし
//* 返値：横幅
//*----------------------------------------------------------------------------------------
float SpriteRenderer::get_Width()const
{
	return m_Width;
}


//*---------------------------------------------------------------------------------------
//* @:SpriteRenderer Class 
//*【?】スプライトの縦幅ゲット
//* 引数：なし
//* 返値：縦幅
//*----------------------------------------------------------------------------------------
float SpriteRenderer::get_Height()const
{
	return m_Height;
}

//*---------------------------------------------------------------------------------------
//* @:SpriteRenderer Class 
//*【?】初期化時に設定したユーザー拡張用頂点定数バッファをGPUにセットする
//*		あくまで臨時の初期化時のデータ更新用なので全く違うものを入れないでね
//* 引数：1.描画エンジン
//* 引数：2.初期化時にセットした配列番号
//* 引数：3.更新データ
//* 返値：void
//*----------------------------------------------------------------------------------------
void SpriteRenderer::setToGPU_ExtendUserVS_CBuffer(RendererEngine& renderer, int arrayNumber, void* _pSrn)
{
	if (m_pVSUserExpandCBuffers == nullptr) {
		return;
	}

	auto pContext = renderer.get_DeviceContext();

	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// GPUメモリにアクセス
	pContext->Map(m_pVSUserExpandCBuffers[arrayNumber].pBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	// データのコピー 
	memcpy(mappedResource.pData, _pSrn, m_pVSUserExpandCBuffers[arrayNumber].Size);

	// アクセス終了
	pContext->Unmap(m_pVSUserExpandCBuffers[arrayNumber].pBuff, 0);
}

//*---------------------------------------------------------------------------------------
//* @:SpriteRenderer Class 
//*【?】初期化時に設定したユーザー拡張用ピクセル定数バッファをGPUにセットする
//*		あくまで臨時の初期化時のデータ更新用なので全く違うものを入れないでね
//* 引数：1.描画エンジン
//* 引数：2.初期化時にセットした配列番号
//* 引数：3.更新データ
//* 返値：void
//*----------------------------------------------------------------------------------------
void SpriteRenderer::setToGPU_ExtendUserPS_CBuffer(RendererEngine& renderer, int arrayNumber, void* _pSrn)
{
	if (m_pPSUserExpandCBuffers == nullptr) {
		return;
	}

	auto pContext = renderer.get_DeviceContext();

	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// GPUメモリにアクセス
	pContext->Map(m_pPSUserExpandCBuffers[arrayNumber].pBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	// データのコピー 
	memcpy(mappedResource.pData, _pSrn, m_pPSUserExpandCBuffers[arrayNumber].Size);

	// アクセス終了
	pContext->Unmap(m_pPSUserExpandCBuffers[arrayNumber].pBuff, 0);
}
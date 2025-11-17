#include "pch.h"
#include "Component_SpriteRenderer.h"
#include "GameObject.h"
#include "RendererManager.h"
#include "MeshInfoFactory.h"
#include "Texture.h"
#include "MeshFactory.h"

using namespace DirectX;
using namespace BASE_VERTEX;
using namespace Tool::UV;

//*---------------------------------------------------------------------------------------
//* @:SpriteRenderer Class 
//*【?】コンストラクタ
//* 引数：1.
//*----------------------------------------------------------------------------------------
SpriteRenderer::SpriteRenderer(std::weak_ptr<GameObject> pOwner, int updateRank) : IComponent(pOwner, updateRank),
m_Width(0.0f),
m_Height(0.0f),
m_pVertexBuffer(nullptr),
m_pIndexBuffer(nullptr),
m_pCBTransformSet(nullptr)
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
//* 引数：1.RendererManager
//* 返値：void
//*----------------------------------------------------------------------------------------
void SpriteRenderer::Init(RendererManager &renderer)
{

}


//*---------------------------------------------------------------------------------------
//* @:SpriteRenderer Class 
//*【?】更新
//* 引数：1.RendererManager
//* 返値：void
//*----------------------------------------------------------------------------------------
void SpriteRenderer::Update(RendererManager &renderer)
{

}


//*---------------------------------------------------------------------------------------
//* @:SpriteRenderer Class 
//*【?】描画
//* 引数：1.RendererManager
//* 返値：void
//*----------------------------------------------------------------------------------------
void SpriteRenderer::Draw(RendererManager &renderer)
{
    auto pContext = renderer.get_DeviceContext();

    // シェーダセット ==========================
    ShaderManager::Instance().DeviceToSetShader(m_ShaderType);
	
	// 頂点情報の更新
	VertexUpdate(renderer);

    // テクスチャセット ==========================
	for (auto it = m_pTextureMap.begin(); it != m_pTextureMap.end(); it++)
	{
		int slot = it->first;
		ID3D11ShaderResourceView *srv = it->second.lock()->get_SRV();
		pContext->PSSetShaderResources(slot, 1, &srv);
	}

    // 頂点＆インデックスバッファ設定 ==========================
    UINT stride = sizeof(VERTEX);
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
//* 引数：1.RendererManager
//* 引数：2. std::weak_ptr<class Texture>
//* 引数：3. 幅
//* 引数：4. 高さ
//* 返値：bool
//*----------------------------------------------------------------------------------------
bool SpriteRenderer::Setup(RendererManager &renderer, SPRITE_USAGE_TYPE type, const std::map<int, std::weak_ptr<class Texture>> &pTexMap, float w, float h)
{
	auto pDevice = renderer.get_Device();

	m_Width = w;
	m_Height = h;
	m_pTextureMap = pTexMap;

	// クアッド生成
	switch (type)
	{
	case SPRITE_USAGE_TYPE::NORMAL:
		m_pMeshInfo = MeshInfoFactory::CreateSpriteQuadInfo(w, h);
		break;
	case SPRITE_USAGE_TYPE::RENDER_TARGET:
		m_pMeshInfo = MeshInfoFactory::CreateRTSpriteInfo(w, h);
		break;
	default:
		break;
	}

	if (m_pMeshInfo == nullptr){
		assert(false);
		return false;
	}

	// 各バッファの生成
	if (!CreateVertexBuffer(pDevice, m_pMeshInfo->pVertices, sizeof(VERTEX), m_pMeshInfo->NumVertex))return false;   // 頂点バッファの作成
	if (!CreateIndexBuffer(pDevice, m_pMeshInfo->pIndices, sizeof(WORD), m_pMeshInfo->NumIndex)) return false;       // インデックスバッファの作成
	if (!CreateCBuffer(pDevice))	return false;																	 // 定数バッファの作成

	return true;
}


//*---------------------------------------------------------------------------------------
//* @:SpriteRenderer Class 
//*【?】頂点情報の更新
//* 引数：なし
//* 返値：void
//*----------------------------------------------------------------------------------------
void SpriteRenderer::VertexUpdate(RendererManager& renderer)
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
//*【?】スプライトの縦幅ゲット
//* 引数：なし
//* 返値：縦幅
//*----------------------------------------------------------------------------------------
void SpriteRenderer::set_ShaderType(SHADER_TYPE type)
{
	m_ShaderType = type;
}


#include "pch.h"
#include "Component_TrailRenderer.h"
#include "GameObject.h"
#include "RendererEngine.h"

using namespace VERTEX;

//*---------------------------------------------------------------------------------------
//*【?】コンストラクタ
//* [引数]
//* pOwner : オーナーオブジェクト
//* updateRank : 更新レイヤー
//*----------------------------------------------------------------------------------------
TrailRenderer::TrailRenderer(std::weak_ptr<GameObject> pOwner, int updateRank) 
	: IComponent(pOwner, updateRank),
	m_MinVertexDistance(100.0f),
	m_DrawTime(10.0f),
	m_Width(10.0f),
	m_CrntTrailPos(VEC3())
{
	this->set_Tag("TrailRenderer");
}


//*---------------------------------------------------------------------------------------
//*【?】デストラクタ
//*----------------------------------------------------------------------------------------
TrailRenderer::~TrailRenderer()
{

}


//*---------------------------------------------------------------------------------------
//*【?】初期化
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//* [返値]なし
//*----------------------------------------------------------------------------------------
void TrailRenderer::Start(RendererEngine &renderer)
{
	if (!Setup(renderer))
	{
		MessageBox(NULL,"セットアップができませんでした","TrailRenderer Error",MB_OK);
		return;
	}
}


//*---------------------------------------------------------------------------------------
//*【?】更新
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//* [返値]なし
//*----------------------------------------------------------------------------------------
void TrailRenderer::Update(RendererEngine &renderer)
{
	auto transform = m_pOwner.lock()->get_Transform().lock();
	m_CrntTrailPos = transform->get_VEC3ToPos();	// 現在位置を末端位置とする

	// 距離判定（最後にリストに入れた地点と比較）
	float dist = 0.0f;
	if (!m_TrailPosList.empty()) {
		dist = VEC3::Distance(m_CrntTrailPos, m_TrailPosList.back());
	}
	else {	// 初回用
		dist = m_MinVertexDistance;
	}
	if (dist >= m_MinVertexDistance)
	{
		// 現在位置を追加
		m_TrailPosList.push_back(m_CrntTrailPos);
	}

	// 頂点数が最大になったら後ろから消していく
	if ((m_TrailPosList.size() * 2) >= MAX_TRAIL_VERTEX_NUM)
	{
		m_TrailPosList.pop_front();
	}
}

//*---------------------------------------------------------------------------------------
//*【?】描画
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//* [返値]なし
//*----------------------------------------------------------------------------------------
void TrailRenderer::Draw(RendererEngine &renderer)
{
	// シャドウパス時には返す
	if (renderer.get_CrntRenderPass() == RENDER_PASS::SHADOW){
		return;
	}

	auto pContext = renderer.get_DeviceContext();

	// 頂点更新
	VertexUpdate(pContext);

	// 定数バッファ更新
	ConstantBufferUpdate(renderer);

	// 頂点バッファの設定
	UINT stride = sizeof(VERTEX_Static);
	UINT offset = 0;
	pContext->IASetVertexBuffers(0, 1, m_pVertesBuffer.GetAddressOf(), &stride, &offset); // 頂点バッファをセット

	// シェーダの設定
	Master::m_pShaderManager->DeviceToSetShader(SHADER_TYPE::DEFERRED_STD_TRAIL);

	// 三角形ストリップ指定
	pContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	renderer.RegisterCullMode(CULL_MODE::NONE);
	// 描画（頂点数は一つの座標につき左右で2つあるので2倍）
	pContext->Draw(static_cast<UINT>(m_TrailPosList.size() * 2), 0);
	renderer.RegisterCullMode(CULL_MODE::BACK);
}

void TrailRenderer::VertexUpdate(ID3D11DeviceContext *pContext)
{
	// GPUメモリにアクセス
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	pContext->Map(m_pVertesBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	XMMATRIX viewMat;

	VERTEX_Static *pVertices = (VERTEX_Static *)mappedResource.pData;
	for (int i = 0; i < m_TrailPosList.size(); i++)
	{
		VEC3 pos = m_TrailPosList[i];

		VERTEX_Static r_V = VERTEX_Static();
		VERTEX_Static l_V = VERTEX_Static();

		r_V = pVertices[i * 2 + 0];	// 左側
		l_V = pVertices[i * 2 + 1];	// 右側

		// 座標
		r_V.pos = VEC3(pos.x - m_Width, pos.y, pos.z);
		l_V.pos = VEC3(pos.x + m_Width, pos.y, pos.z);

		// カラー
		r_V.color.AllOne();
		l_V.color.AllOne();

		pVertices[i * 2 + 0] = r_V;
		pVertices[i * 2 + 1] = l_V;
	}

	// データのコピー 
	memcpy(mappedResource.pData, pVertices, sizeof(VERTEX_Static) * m_TrailPosList.size() * 2);

	// アクセス終了
	pContext->Unmap(m_pVertesBuffer.Get(), 0);
}

bool TrailRenderer::Setup(RendererEngine &renderer)
{
	// 頂点バッファの作成
	if (!CreateVertexBuffer(renderer))
	{
		MessageBox(NULL, "頂点バッファの作成が出来ませんでした", "TrailRenderer Error", MB_OK);
		return false;
	}
	// 定数バッファの作成
	if (!CreateConstantBuffer(renderer))
	{
		MessageBox(NULL, "定数バッファの作成が出来ませんでした", "TrailRenderer Error", MB_OK);
		return false;
	}

	return true;
}

bool TrailRenderer::CreateVertexBuffer(RendererEngine &renderer)
{
	auto pDevice = renderer.get_Device();

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.Usage = D3D11_USAGE_DYNAMIC;								// 動的書き込み
	bd.ByteWidth = sizeof(VERTEX_Static) * MAX_TRAIL_VERTEX_NUM;// バッファのサイズ
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;					// 頂点バッファとして使う
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;					// CPUから書き込み

	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
	initData.pSysMem = nullptr;

	// バッファの作成
	HRESULT hr = pDevice->CreateBuffer(&bd, nullptr, m_pVertesBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

bool TrailRenderer::CreateConstantBuffer(RendererEngine &renderer)
{
	auto pDevice = renderer.get_Device();

	// マテリアル --------------------------------------------------------------
	m_pCBMaterialDataSet = new CB_MATERIAL_SET();

	D3D11_BUFFER_DESC mat_BufferDesc;
	ZeroMemory(&mat_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	mat_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	mat_BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	mat_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	mat_BufferDesc.ByteWidth = sizeof(CB_MATERIAL);

	// バッファの作成
	hr = pDevice->CreateBuffer(&mat_BufferDesc, nullptr, &m_pCBMaterialDataSet->pBuff);
	if (FAILED(hr))
	{
		delete m_pCBMaterialDataSet;
		return false;
	}

	return true;
}


void TrailRenderer::set_TrailPos(VECTOR3::VEC3 _head, VECTOR3::VEC3 _tail)
{
}
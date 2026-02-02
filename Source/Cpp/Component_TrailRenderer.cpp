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
	m_CrntTrailPos(VEC3()),
	m_pCBMaterialDataSet(nullptr),
	m_IsView(true)
{
	this->set_Tag("TrailRenderer");
}


//*---------------------------------------------------------------------------------------
//*【?】デストラクタ
//*----------------------------------------------------------------------------------------
TrailRenderer::~TrailRenderer()
{
	if (m_pCBMaterialDataSet) {
		if (m_pCBMaterialDataSet->pBuff) {
			m_pCBMaterialDataSet->pBuff->Release();
		}
		delete m_pCBMaterialDataSet;
		m_pCBMaterialDataSet = nullptr;
	}

	m_pVertesBuffer.Reset();
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
	m_pTex = Master::m_pResourceManager->LoadWIC_Texture(L"Resource/Texture/rust_coarse_01_arm_1k.jpg");

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
	VertexUpdate(renderer);

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
	
	Master::m_pBlendManager->DeviceToSetBlendState(BLEND_MODE::ALPHA);

	// 描画（頂点数は一つの座標につき左右で2つあるので2倍）
	pContext->Draw(static_cast<UINT>(m_TrailPosList.size() * 2), 0);
	Master::m_pBlendManager->DeviceToSetBlendState(BLEND_MODE::NONE);
}


//*---------------------------------------------------------------------------------------
//*【?】頂点情報の更新
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//*
//* [返値]
//* true : 成功
//* false : 失敗
//*----------------------------------------------------------------------------------------
void TrailRenderer::VertexUpdate(RendererEngine& renderer)
{
	auto pContext = renderer.get_DeviceContext();

	if (m_TrailPosList.empty() || m_TrailPosList.size() == 1)return;

	// GPUメモリにアクセス
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	pContext->Map(m_pVertesBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	// カメラ座標
	VEC3 cameraPos = renderer.get_CameraPosition();

	VERTEX_Static *pVertices = (VERTEX_Static *)mappedResource.pData;
	for (int i = 0; i < m_TrailPosList.size(); i++)
	{
		VEC3 tail = m_TrailPosList[i];	// 末尾（今作るのはこれ）
		VEC3 head = VEC3();				// 先頭

		// 先頭位置を取得
		if (i < (m_TrailPosList.size() - 1)){
			head = m_TrailPosList[i + 1];
		}
		else{
			// 先頭位置がない場合、前の情報から仮想の位置を作る
			VEC3 prev = m_TrailPosList[i - 1];
			head = head + (tail - prev);
		}

		// カメラへの方向
		VEC3 viewDir = (tail - cameraPos).Normalize();
		
		// 先頭位置への方向
		VEC3 headDir = (head - tail).Normalize();

		// メッシュの広がる方向ベクトルを作る
		VEC3 dir = VEC3::Cross(headDir, viewDir);

		VEC3 r_NewPos = tail + (dir * m_Width);
		VEC3 l_NewPos = tail + (dir * -m_Width);

		VERTEX_Static r_V = VERTEX_Static();
		VERTEX_Static l_V = VERTEX_Static();
		r_V = pVertices[i * 2 + 0];	// 左側
		l_V = pVertices[i * 2 + 1];	// 右側

		// 座標
		r_V.pos = r_NewPos;
		l_V.pos = l_NewPos;

		// カラー
		r_V.color.AllOne();
		l_V.color.AllOne();

		r_V.uv = VEC2(1.0f, (float)i / (float)(m_TrailPosList.size() - 1));
		l_V.uv = VEC2(0.0f, (float)i / (float)(m_TrailPosList.size() - 1));

		r_V.normal.AllOne();
		l_V.normal.AllOne();
		
		pVertices[i * 2 + 0] = r_V;
		pVertices[i * 2 + 1] = l_V;
	}

	// データのコピー 
	memcpy(mappedResource.pData, pVertices, sizeof(VERTEX_Static) * m_TrailPosList.size() * 2);

	// アクセス終了
	pContext->Unmap(m_pVertesBuffer.Get(), 0);
}


void CalcTrailSegmentToViewDirection(RendererEngine& renderer)
{

}

void CalcTrailSegment(RendererEngine& renderer)
{

}

//*---------------------------------------------------------------------------------------
//*【?】定数バッファの更新
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//*
//* [返値]
//* なし
//*----------------------------------------------------------------------------------------
void TrailRenderer::ConstantBufferUpdate(RendererEngine& renderer)
{
	auto pContext = renderer.get_DeviceContext();

	// GPUメモリにアクセス
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	pContext->Map(m_pCBMaterialDataSet->pBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	m_pCBMaterialDataSet->Data.Diffuse = VEC4(1.0f, 1.0f, 1.0f, 1.0f);
	m_pCBMaterialDataSet->Data.EmissivePower = 0.0f;
	m_pCBMaterialDataSet->Data.EmissiveColor = VEC3(1.0f, 0.0f, 0.0f);
	m_pCBMaterialDataSet->Data.Specular = VEC4(1.0f, 1.0f, 1.0f, 1.0f);
	m_pCBMaterialDataSet->Data.SpecularPower = 150.0f;

	// データのコピー 
	memcpy(mappedResource.pData, &m_pCBMaterialDataSet->Data, sizeof(CB_MATERIAL));

	// アクセス終了
	pContext->Unmap(m_pCBMaterialDataSet->pBuff, 0);

	// 定数バッファへ送信
	pContext->PSSetConstantBuffers(4,1, &m_pCBMaterialDataSet->pBuff);

	ID3D11ShaderResourceView* tex = nullptr;
	tex = m_pTex->get_SRV();
	pContext->PSSetShaderResources(0, 1, &tex);
}

//*---------------------------------------------------------------------------------------
//*【?】セットアップ
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//*
//* [返値]
//* true : 成功
//* false : 失敗
//*----------------------------------------------------------------------------------------
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


//*---------------------------------------------------------------------------------------
//*【?】頂点バッファの作成
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//*
//* [返値]
//* true : 成功
//* false : 失敗
//*----------------------------------------------------------------------------------------
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


//*---------------------------------------------------------------------------------------
//*【?】定数バッファの作成
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//*
//* [返値]
//* true : 成功
//* false : 失敗
//*----------------------------------------------------------------------------------------
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
	HRESULT hr = pDevice->CreateBuffer(&mat_BufferDesc, nullptr, &m_pCBMaterialDataSet->pBuff);
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
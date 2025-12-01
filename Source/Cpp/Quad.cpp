#include "pch.h"
//#include "Quad.h"
//#include "RendererEngine.h"
//#include "Texture.h"
//
//using namespace DirectX;
//using namespace BASE_VERTEX;
//using namespace Tool::UV;
////*---------------------------------------------------------------------------------------
////* @:Quad Class 
////*【?】コンストラクタ
////* 引数：なし
////*----------------------------------------------------------------------------------------
//Quad::Quad()
//{
//
//}
//
//
////*---------------------------------------------------------------------------------------
////* @:Quad Class 
////*【?】デストラクタ
////* 引数：なし
////*----------------------------------------------------------------------------------------
//Quad::~Quad()
//{
//
//}
//
//
////*---------------------------------------------------------------------------------------
////* @:Quad Class 
////*【?】初期化
////* 引数：1.RendererEngine
////* 返値：bool
////*----------------------------------------------------------------------------------------
//bool Quad::Init(RendererEngine &renderer)
//{
//    return true;
//}
//
//
////*---------------------------------------------------------------------------------------
////* @:Quad Class 
////*【?】更新
////* 引数：1.RendererEngine
////* 返値：bool
////*----------------------------------------------------------------------------------------
//void Quad::Update(RendererEngine &renderer)
//{
//
//}
//
//
////*---------------------------------------------------------------------------------------
////* @:Quad Class 
////*【?】描画
////* 引数：1.RendererEngine
////* 返値：bool
////*----------------------------------------------------------------------------------------
//void Quad::Draw(RendererEngine &renderer)
//{
//    auto pContext = renderer.get_DeviceContext();
//
//    /* ========== 定数バッファの更新 ========== */
//
//    // ワールド行列セット ==========================
//    XMMATRIX worldMtx = this->get_Transform().lock()->get_WorldMtx();
//    XMMATRIX mtx = XMMatrixTranspose(worldMtx);   // 行列の転置
//    XMStoreFloat4x4(&m_pCBTransformSet->Data.WorldMtx, mtx);  // XMMATRIX → XMFLOAT4X4変換
//
//    // 定数バッファに転送
//    pContext->UpdateSubresource(
//        m_pCBTransformSet->pBuff,
//        0,
//        nullptr,
//        &m_pCBTransformSet->Data,
//        0,
//        0
//    );
//
//    // マテリアル情報セット ==========================
//    CB_MATERIAL mat{};
//    mat.Diffuse = m_pMeshInfo->pMaterials[0].DiffuseColor;
//    mat.Specular = m_pMeshInfo->pMaterials[0].SpecularColor;
//    mat.Normal = m_pMeshInfo->pMaterials[0].NormalColor;
//    mat.SpecularPower = m_pMeshInfo->pMaterials[0].SpecularPower;
//    m_pCBMaterialDataSet->Data = mat;
//
//    // 定数バッファに転送
//    pContext->UpdateSubresource(
//        m_pCBMaterialDataSet->pBuff,
//        0,
//        nullptr,
//        &m_pCBMaterialDataSet->Data,
//        0,
//        0
//    );
//
//    // 定数バッファをセット ==========================
//    pContext->VSSetConstantBuffers(0, 1, &m_pCBTransformSet->pBuff);
//    pContext->VSSetConstantBuffers(4, 1, &m_pCBMaterialDataSet->pBuff);
//    //pContext->PSSetConstantBuffers(0, 1, &m_pCB3DObjectSet->pBuff);
//
//    // シェーダセット ==========================
//    ShaderManager::Instance().DeviceToSetShader(SHADER_TYPE::SIMPLE);
//
//
//    // テクスチャセット ==========================
//    ID3D11ShaderResourceView *diffuseSRV = nullptr;
//    ID3D11ShaderResourceView *normalSRV = nullptr;
//    ID3D11ShaderResourceView *specularSRV = nullptr;
//    if (auto tex = m_pMeshInfo->pMaterials->Diffuse.Texture.lock()) {
//        diffuseSRV = tex.get()->get_SRV();
//    }
//    if (auto tex = m_pMeshInfo->pMaterials->Normal.Texture.lock()) {
//        normalSRV = tex.get()->get_SRV();
//    }
//    if (auto tex = m_pMeshInfo->pMaterials->Specular.Texture.lock()) {
//        specularSRV = tex.get()->get_SRV();
//    }
//
//    pContext->PSSetShaderResources(0, 1, &diffuseSRV);
//    pContext->PSSetShaderResources(1, 1, &normalSRV);
//    pContext->PSSetShaderResources(2, 1, &specularSRV);
//
//    // 頂点＆インデックスバッファ設定 ==========================
//    UINT stride = sizeof(VERTEX);
//    UINT offset = 0;
//    pContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset); // 頂点バッファをセット
//    pContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);    // インデックスバッファをセット
//    pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);// Set primitive topology 頂点の組み合わせ方
//
//    // 描画コール：インデックス数は6（三角形2個 × 3頂点） ==========================
//    pContext->DrawIndexed(m_pMeshInfo->NumIndex, 0, 0);
//}
//
//
////*---------------------------------------------------------------------------------------
////* @:Quad Class 
////*【?】終了
////* 引数：1.RendererEngine
////* 返値：bool
////*----------------------------------------------------------------------------------------
//bool Quad::Term(RendererEngine &renderer)
//{
//    return true;
//}
//
//
//// ----------------------------------------------------------------------------------------------------------------------
////       * Quad Class - Quadのセットアップ- *
//// 引数 1.描画マネージャー
////      2.マテリアルのセット（生成済みのもの）
//// 		3.マテリアル数
//// ----------------------------------------------------------------------------------------------------------------------
//bool Quad::Setup(RendererEngine& renderer, MATERIAL* materials, UINT materialNum)
//{
//    auto pDevice  = renderer.get_Device();
//    /*
//    VERTEX vertices[4]{
//        // 座標                       // 法線                  // カラー                      // uv
//        { VEC3( 1.0f,  -1.0f,  1.0f), VEC3(0.0f, -1.0f, 0.0f), VEC4(1.0f, 1.0f, 1.0f, 1.0f),  VEC2(uv[3].u, 0.0f) },            // 8 左上
//        { VEC3(-1.0f,  -1.0f,  1.0f), VEC3(0.0f, -1.0f, 0.0f), VEC4(1.0f, 1.0f, 1.0f, 1.0f),  VEC2(uv[3].u + uv[3].uw, 0.0f) }, // 9 右上
//        { VEC3( 1.0f,  -1.0f, -1.0f), VEC3(0.0f, -1.0f, 0.0f), VEC4(1.0f, 1.0f, 1.0f, 1.0f),  VEC2(uv[3].u,            1.0f) }, // 10左下
//        { VEC3(-1.0f,  -1.0f, -1.0f), VEC3(0.0f, -1.0f, 0.0f), VEC4(1.0f, 1.0f, 1.0f, 1.0f),  VEC2(uv[3].u + uv[3].uw, 1.0f) }, // 11右下
//    };
//    */
//
//    // メッシュ情報の作成
//    m_pMeshInfo = MeshFactory::CreateQuadInfo(materials, materialNum);
//
//    if ( !CreateVertexBuffer(pDevice, m_pMeshInfo->pVertices, sizeof(VERTEX), m_pMeshInfo->NumVertex) )return false;   // 頂点バッファの作成
//    if ( !CreateIndexBuffer(pDevice, m_pMeshInfo->pIndices, sizeof(WORD), m_pMeshInfo->NumIndex) ) return false;      // インデックスバッファの作成
//    if ( !CreateCBuffer(pDevice) )	return false;							        // 定数バッファの作成
//
//    return true;
//}
//

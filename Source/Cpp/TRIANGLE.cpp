#include "pch.h"
#include "RendererManager.h"
#include "TRIANGLE.h"

//
/* 頂点バッファの作成 */
//
bool VERTEX3::CreateVertexBuffer(RendererManager& render)
{
    SAFE_RELEASE(vBuf); // 二重に作成しないよう念のため開放

    auto pDevice = render.get_Device();

    // 頂点バッファ定義
    D3D11_BUFFER_DESC vertexBuffDesc;
    ZeroMemory(&vertexBuffDesc, sizeof(vertexBuffDesc));
    vertexBuffDesc.Usage     = Usage;                       // 動的バッファ
    vertexBuffDesc.ByteWidth = sizeof(BASE_VERTEX::VERTEX) * 3;       // ３つの頂点分
    vertexBuffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;    // 頂点バッファとの関連付け
    vertexBuffDesc.CPUAccessFlags = CPUAccessFlags;         // CPUからアクセス方法

    // データ本体
    D3D11_SUBRESOURCE_DATA vertexSubData;
    ZeroMemory(&vertexSubData, sizeof(vertexSubData));
    vertexSubData.pSysMem = vertex;     // 頂点情報入れる

    // バッファ作成
    HRESULT hr = pDevice->CreateBuffer(
        &vertexBuffDesc, 
        &vertexSubData, 
        &vBuf
    );

    if (FAILED(hr)) return false;

    return true;
}


//
/* ポリゴンの描画 */
//
void VERTEX3::Draw(RendererManager& render)
{
    auto pDeviceContext = render.get_DeviceContext();

    /* 動的更新の場合 */
    //if (Usage == D3D11_USAGE_DYNAMIC)
    //{
    //    // CPU が GPU バッファを書き換える流れ
    //    D3D11_MAPPED_SUBRESOURCE ms;
    //    pDeviceContext->Map(vBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);

    //    // ms.pData が CPU から触れるポインタになる
    //    memcpy(ms.pData, vertex, sizeof(BASE_VERTEX::VERTEX) * VERTEX_NUM);

    //    // 書き換え終了！
    //    pDeviceContext->Unmap(vBuf, 0);
    //}
    ///* デフォルトの場合 */
    //else if(Usage == D3D11_USAGE_DEFAULT)
    //{
    //    pDeviceContext->UpdateSubresource(vBuf, 0, NULL, vertex, 0, 0);
    //}
    //

    UpdateTransform();

    SetupTransform(render);

    uint32_t strides[1] = { sizeof(BASE_VERTEX::VERTEX) };
    uint32_t offsets[1] = { 0 };
    pDeviceContext->IASetVertexBuffers(0, 1, &vBuf, strides, offsets);  // 頂点バッファの設定
    //pDeviceContext->IASetInputLayout(render.get_ShaderData().m_pVertexLayout);
    //pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    pDeviceContext->Draw(VERTEX_NUM, 0);
}

void VERTEX3::UpdateTransform()
{
    //scale += 0.01f;
    //if (scale >= 5.0f)scale = 0.0f;

    //scale = 1.0f;

    //angle_Z += 0.05f;
    //angle_Z = 0.0f;

    //translate_X += 0.01f;
    //translate_Y += 0.01f;  
    //
    ////translate_X = 0.5f;
    ////translate_Y = 0.0f;

    //if (translate_X >= 1.25f) translate_X = -1.25f;
    //if (translate_Y >= 1.25f) translate_Y = -1.25f;
}

//
/* VERTEX3 Struct - 定数バッファに変換行列書き込む - */
//
void VERTEX3::SetupTransform(RendererManager& render)
{
    auto pDeviceContext = render.get_DeviceContext();
    auto& cb = render.get_RenderParam().cbTransformSet;

    //XMMATRIX mtx = XMMatrixTranslation(translate_X, translate_Y, 0.0f);
    //XMMATRIX mtx = XMMatrixRotationZ(angle_Z);
    //XMMATRIX mtx = XMMatrixScaling(scale, scale, 1.0f);

    XMMATRIX mtxS = XMMatrixScaling(scale, scale, scale);   // スケール
    XMMATRIX mtxR = XMMatrixRotationZ(angle_Z);             // 回転
    XMMATRIX mtxT = XMMatrixTranslation(translate_X, translate_Y, 0.0f); // 移動

    //// 変換順序
    //// スケールｘ回転ｘ移動
    XMMATRIX mtx = mtxS * mtxR * mtxT;
    
    //XMMATRIX mtx = XMMatrixIdentity();

    //auto mtx = XMMatrixIdentity();

    // XMMatrixTranspose 転置行列変換
    mtx = XMMatrixTranspose(mtx);

    // XMMATRIX を XMFLOAT4x4に格納
    XMStoreFloat4x4(&cb.Data.WorldMtx, mtx);
    
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    ZeroMemory(&mappedResource, sizeof(mappedResource));
    HRESULT hr = pDeviceContext->Map(
        cb.pBuff, 
        0, 
        D3D11_MAP_WRITE_DISCARD, 
        0, 
        &mappedResource
    );

    if (FAILED(hr))return;

    CopyMemory(mappedResource.pData, &cb.Data, sizeof(cb.Data));

    // マップ解除
    pDeviceContext->Unmap(cb.pBuff, 0);
    pDeviceContext->VSSetConstantBuffers(0, 1, &cb.pBuff);
}
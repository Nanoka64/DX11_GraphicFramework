#pragma once
//--------------------------------------------------------------------------------------
//      * Includes *
//--------------------------------------------------------------------------------------


//
/* シェーダ情報構造体 */
//
struct SHADER_DATA
{
    /* ↓メンバ変数↓ */
    ID3D11VertexShader *m_pVertexShader;  // 頂点シェーダ
    ID3D11PixelShader  *m_pPixelShader;   // ピクセルシェーダ
    ID3D11InputLayout  *m_pVertexLayout;  // 頂点フォーマットの設定

    /* ↓メソッド↓ */
    SHADER_DATA();
    ~SHADER_DATA();
};


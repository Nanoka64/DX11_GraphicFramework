// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//
/* - @:ピクセルシェーダ -*/
//
//  【?】Depth of Field 
//       被写界深度エフェクトのぼかし合成用シェーダ
//
// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
#pragma once
#include"ConstantBuffers_H.hlsli"
SamplerState g_sClampSampler : register(s2);
Texture2D g_tSceneBlurTexture0 : register(t0);  // メインシーンのぼかしテクスチャ
Texture2D g_tDepathTexture1 : register(t1);     // 深度テクスチャ


/* =========================================================================
/* - @:入力構造体 - */
/* =========================================================================*/
struct PS_IN
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR;
    float2 UV : TEXCOORD;
};

// **************************************************************************
/* - @:エントリーポイント - */
// **************************************************************************
float4 PSMain(PS_IN input) : SV_TARGET
{
    float4 blurTex = g_tSceneBlurTexture0.Sample(g_sClampSampler, input.UV);
    float depth = g_tDepathTexture1.Sample(g_sClampSampler, input.UV).r;
    float4 finalColor = float4(0.0, 0.0, 0.0, 1.0);
    
    // 深度が1.0f に近い背景は薄くぼかす
    if (depth >= 0.9999)
    {
        blurTex.a = 0.5f;
        return blurTex;
    }
    
    // ※ 深度値はNDC空間のZ値なのでビュー空間に戻す
    float4 ndcPos = float4(0, 0, 0, 0); // NDC空間（正規化デバイス座標）
    ndcPos.xy = input.UV * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f);
    ndcPos.z = depth;
    ndcPos.w = 1.0f;
    
    // 逆行列使ってNDC空間からビュー空間に戻す
    ndcPos = mul(ndcPos, cb_ProjectionInv);
    ndcPos /= ndcPos.w; // Wで除算
    
    // ビュー空間の深度値
    float viewDepth = ndcPos.z;
    
    // cb_DoF_MinRange以上からブラーがかかり、cb_DoF_MaxRangeで最大になる
    // ぼかし開始範囲より近い場合はぼかし無し
    clip(viewDepth - cb_DoF_MinRange);
    
    // 深度に応じてぼかしの強さを変化させる
    blurTex.a = min(1.0f, (viewDepth - cb_DoF_MinRange) / cb_DoF_MaxRange);
    
    finalColor = blurTex;
    
    return finalColor;
}
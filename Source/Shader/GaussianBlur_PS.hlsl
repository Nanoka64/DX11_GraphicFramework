// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//
/* - @:ピクセルシェーダ -*/
//
//  【?】ガウシアンブラー
//
// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
#pragma once
#include "ConstantBuffers_H.hlsli"
SamplerState g_sSampler : register(s0);
Texture2D g_tTexture : register(t0); 


/* =========================================================================
/* - @:入力構造体 - */
/* =========================================================================*/
struct PS_BLUR_IN
{
    float4 pos : SV_POSITION;
    float4 tex0 : TEXCOORD0;
    float4 tex1 : TEXCOORD1;
    float4 tex2 : TEXCOORD2;
    float4 tex3 : TEXCOORD3;
    float4 tex4 : TEXCOORD4;
    float4 tex5 : TEXCOORD5;
    float4 tex6 : TEXCOORD6;
    float4 tex7 : TEXCOORD7;
};

// **************************************************************************
/* - @:エントリーポイント - */
// **************************************************************************
float4 PSMain(PS_BLUR_IN input) : SV_TARGET
{
    float4 finalColor;

    // 基準テクセルからプラス方向に8テクセル、重み付きでサンプリング
    finalColor  = cb_BlurWeights[0].x  * g_tTexture.Sample(g_sSampler, input.tex0.xy);
    finalColor += cb_BlurWeights[0].y * g_tTexture.Sample(g_sSampler, input.tex1.xy);
    finalColor += cb_BlurWeights[0].z * g_tTexture.Sample(g_sSampler, input.tex2.xy);
    finalColor += cb_BlurWeights[0].w * g_tTexture.Sample(g_sSampler, input.tex3.xy);
    finalColor += cb_BlurWeights[1].x * g_tTexture.Sample(g_sSampler, input.tex4.xy);
    finalColor += cb_BlurWeights[1].y * g_tTexture.Sample(g_sSampler, input.tex5.xy);
    finalColor += cb_BlurWeights[1].z * g_tTexture.Sample(g_sSampler, input.tex6.xy);
    finalColor += cb_BlurWeights[1].w * g_tTexture.Sample(g_sSampler, input.tex7.xy);

    // 基準テクセルにマイナス方向に8テクセル、重み付きでサンプリング
    finalColor += cb_BlurWeights[0].x * g_tTexture.Sample(g_sSampler, input.tex0.zw);
    finalColor += cb_BlurWeights[0].y * g_tTexture.Sample(g_sSampler, input.tex1.zw);
    finalColor += cb_BlurWeights[0].z * g_tTexture.Sample(g_sSampler, input.tex2.zw);
    finalColor += cb_BlurWeights[0].w * g_tTexture.Sample(g_sSampler, input.tex3.zw);
    finalColor += cb_BlurWeights[1].x * g_tTexture.Sample(g_sSampler, input.tex4.zw);
    finalColor += cb_BlurWeights[1].y * g_tTexture.Sample(g_sSampler, input.tex5.zw);
    finalColor += cb_BlurWeights[1].z * g_tTexture.Sample(g_sSampler, input.tex6.zw);
    finalColor += cb_BlurWeights[1].w * g_tTexture.Sample(g_sSampler, input.tex7.zw);
    
    return float4(finalColor);
}
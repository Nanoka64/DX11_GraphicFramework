// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//
/* - @:ピクセルシェーダ -*/
//
//  【?】高輝度成分の抜出しを行う

// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
#pragma once
#include "ConstantBuffers_H.hlsli"
SamplerState g_sSampler : register(s0);
Texture2D g_tTexture : register(t0);

Texture2D g_tDepthTexture : register(t1);   // 深度


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
    float4 texColor = g_tTexture.Sample(g_sSampler, input.UV);
    float depth = g_tDepthTexture.Sample(g_sSampler, input.UV).r;
    float4 finalColor = float4(0.0, 0.0, 0.0, 1.0);
    
    // 高輝度成分の抜出し処理
    const float3 lumaWeight = half3(0.2125, 0.7154, 0.0721);
    float3 luminance = dot(texColor.xyz, lumaWeight);
    
    // 深度が1.0fに近い場合は背景なので輝度0にする
    if (depth >= 0.9999)
    {
        luminance = 0.0f;
    }
    //clip(luminance - 1.0f);
    
    // 1.0以下なら0.0になる
    float t = step(1.0f, luminance);
    texColor.xyz *= t;
    
    texColor.a = 1.0f;
    return texColor;
}
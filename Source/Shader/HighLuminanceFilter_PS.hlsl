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


/* =========================================================================
/* - @:入力構造体 - */
/* =========================================================================*/
struct PS_IN
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR;
    float2 UV : TEXCOORD;
};
float threshold = 1.0;
float softRange = 0.5;

// **************************************************************************
/* - @:エントリーポイント - */
// **************************************************************************
float4 PSMain(PS_IN input) : SV_TARGET
{
    float4 texColor = g_tTexture.Sample(g_sSampler, input.UV);
    float4 finalColor = float4(0.0, 0.0, 0.0, 1.0);
    
    // 高輝度成分の抜出し処理
    const float3 lumaWeight = half3(0.2125, 0.7154, 0.0721);
    float3 luminance = dot(texColor.xyz, lumaWeight);
    clip(luminance - 1.0f);
    
    return texColor;
}
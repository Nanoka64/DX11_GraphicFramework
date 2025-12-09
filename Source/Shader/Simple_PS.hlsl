// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//
/* - @:ピクセルシェーダ -*/
//
//  【?】フォワードシェーディング
//       簡易ピクセルシェーダ
//
// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
#pragma once
SamplerState g_sSampler : register(s0);
Texture2D g_tTexture : register(t0); // ディフューズ

/* =========================================================================
/* - @:出力構造体 - */
/* =========================================================================*/
struct PS_SimpleIntput
{
    float4 Pos : SV_Position;
    float4 World : WORLD0;
    float3 Normal : NORMAL0;
    float4 Color : COLOR0;
    float2 UV : TEXCOORD0;
};


// **************************************************************************
/* - @:エントリーポイント - */
// **************************************************************************
float4 PSMain(PS_SimpleIntput input) : SV_TARGET
{
    float4 texColor = g_tTexture.Sample(g_sSampler, input.UV);
    float4 finalColor = float4(0.0, 0.0, 0.0, 1.0);
    finalColor = texColor * input.Color;
    return finalColor;
}
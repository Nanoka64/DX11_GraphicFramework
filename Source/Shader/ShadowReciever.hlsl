// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//
/* - @:シャドウレシーバー -*/
//
//  【?】シャドウの書き込み側
//       
//
// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
#pragma once
#include "ConstantBuffers_H.hlsli"  // 共通ヘッダー


/* =========================================================================
/* - @:入力構造体 - */
/* =========================================================================*/
struct VS_INPUT
{
    float3 Pos : POSITION;
    float2 UV : TEXCOORD;
    float4 Color : COLOR;
    float3 Normal : NORMAL;
};

/* =========================================================================
/* - @:出力構造体 - */
/* =========================================================================*/
struct PS_INPUT
{
    float4 Pos : SV_Position;
    float4 World : WORLD0;
    float3 Normal : NORMAL0;
    float4 Color : COLOR0;
    float2 UV : TEXCOORD0;
    float4 PosInLVP : TEXCOORD1;
};

// **************************************************************************
/* - @:頂点 エントリーポイント - */
// **************************************************************************
PS_INPUT VSMain(VS_INPUT input)
{
    PS_INPUT output;
    float4 pos = float4(input.Pos, 1.0f);
    float3 norm = input.Normal;
    float2 uv = input.UV;
    float4 color = input.Color;
    
    norm = normalize(mul(norm, (float3x3) cb_Transform));
    
    pos = mul(pos, cb_Transform);   // ワールド変換（ワールド空間）
    float4 worldPos = pos;
    pos = mul(pos, cb_View);        // ビュー変換（ビュー空間）
    pos = mul(pos, cb_Projection);  // 投影変換（クリッピング空間）
    
    // ライトスクリーン空間での座標（今のところ平行光源は一つだけなので）
    output.PosInLVP = mul(worldPos, cb_DirLightData[0].LightViewProj);
    
    output.Pos = pos;           // 画面空間の頂点座標
    output.Normal = norm.xyz;   // 法線
    output.UV = input.UV;       // テクスチャ座標
    output.Color = color;
    output.World = worldPos;
    return output;
}

SamplerState g_sSampler : register(s0);
Texture2D g_tTexture : register(t0); // ディフューズ
Texture2D g_tShadowMap : register(t1); // ディフューズ

// **************************************************************************
/* - @:ピクセル エントリーポイント - */
// **************************************************************************
float4 PSMain(PS_INPUT input) : SV_TARGET0
{
    float4 finalColor = g_tTexture.Sample(g_sSampler, input.UV);
    
    float2 shadowUV = input.PosInLVP.xy / input.PosInLVP.w;
    shadowUV *= float2(0.5f, -0.5f);
    shadowUV += 0.5f;
    
    float zInLVP = input.PosInLVP.z / input.PosInLVP.w;
    
    if (shadowUV.x > 0 && shadowUV.x < 1 && shadowUV.y > 0 && shadowUV.y < 1)
    {
        float zInShadowMap = g_tShadowMap.Sample(g_sSampler, shadowUV);
        if (zInShadowMap < zInLVP)
        {
            finalColor.xyz *= 0.5f;
        }
    }
    
    return finalColor;
}
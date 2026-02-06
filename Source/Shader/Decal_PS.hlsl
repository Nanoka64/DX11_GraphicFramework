// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//
/* - @:ピクセルシェーダ -*/
//
//  【?】デカールの書き込み用
//       
//
// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
#pragma once
#include "ConstantBuffers_H.hlsli"
#include "UtilityFunctions_H.hlsli"
SamplerState g_sSampler : register(s0);
Texture2D g_tDecalTexture : register(t0);   // デカールテクスチャ
Texture2D g_tDepthTexture : register(t1);   // 深度値Gバッファ


/* =========================================================================
/* - @:入力構造体 - */
/* =========================================================================*/
struct PS_IN
{
    float4 Pos : SV_Position;
    float3 Normal : NORMAL0;
    float4 Color : COLOR0;
    float2 UV : TEXCOORD0;
};

/* =========================================================================
/* - @:MRT出力構造体 - */
/* =========================================================================*/
struct PS_OUT
{
    float4 Albedo : SV_Target0;
    float4 Normal : SV_Target1;
};


// **************************************************************************
/* - @:エントリーポイント - */
// **************************************************************************
float4 PSMain(PS_IN input) : SV_TARGET
{
    float4 finalColor = float4(0.0, 0.0, 0.0, 1.0);
    
    // 深度値
    float depth = g_tDepthTexture.Sample(g_sSampler, input.UV).r;
    
    // 深度値からワールド座標求める
    float4 worldPos = GetConvertPosFromDepthToWorld(depth, input.UV);
    
    // デカールボックスワールド逆行列を掛けて、デカールボックスのローカル空間に変換する
    float4 localPosToDecal = mul(worldPos, cb_InvDecalTransform);
    
    //float2 decalSize = float2(0.5f, 0.5f);
    
    // デカールボックスの範囲内か
    if (localPosToDecal.x < 0.5f && localPosToDecal.x > -0.5f &&
        localPosToDecal.y < 0.5f && localPosToDecal.y > -0.5f &&
        localPosToDecal.z < 0.5f && localPosToDecal.z > -0.5f)
    {
        // UV座標へ変換
        float2 decalUV = localPosToDecal.xy;
        decalUV += 0.5f;
        
        // デカールテクスチャをサンプリング
        float4 decalTex = g_tDecalTexture.Sample(g_sSampler, decalUV);
        finalColor.xyz = decalTex.xyz;
    }
    
    return finalColor;
}
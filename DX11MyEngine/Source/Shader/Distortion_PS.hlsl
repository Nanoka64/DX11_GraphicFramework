// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//
/* - @:ピクセルシェーダ -*/
//
//  【?】フォワードシェーディング
//       ディストーションの描画
//
// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
#pragma once
#include "ConstantBuffers_H.hlsli"
SamplerState g_sSampler : register(s0);
SamplerState g_sClampSampler : register(s2);
Texture2D g_tSceneTexture : register(t6); // シーンテクスチャ
Texture2D g_tNoizeTexture : register(t1); // ノイズテクスチャ
Texture2D g_tDepathTexture1 : register(t1); // 深度テクスチャ

/* =========================================================================
/* - @:入力構造体 - */
/* =========================================================================*/
struct PS_IN
{
    float4 Pos       : SV_Position;
    float4 PosInProj : TEXCOORD0; // 投影空間の頂点座標
    float2 UV        : TEXCOORD1;
};


// **************************************************************************
/* - @:エントリーポイント - */
// **************************************************************************
float4 PSMain(PS_IN input) : SV_TARGET
{
    float4 finalColor = float4(0.0, 0.0, 0.0, 1.0);
    float depth = g_tDepathTexture1.Sample(g_sClampSampler, input.UV).r;
    float2 screenUV;
    screenUV.x = input.PosInProj.x;
    screenUV.y = -input.PosInProj.y;
    
    // 現在のピクセルの深度値を取得
    float myDepth = input.PosInProj.z;
    
    // ノイズテクスチャを使ってUVを歪ませる
    float2 noiseUV = input.UV + float2(cb_DistortionPower, cb_DistortionPower) * cb_DistortionTime;
    float3 noise = g_tNoizeTexture.Sample(g_sSampler, noiseUV).rgb;
    
    // 0～1のノイズを -1～1 に変換して強度を調整
    float2 distortion = (noise.xy * 2.0f - 1.0f) * 0.01;
    
    float2 distortedUV = screenUV + (distortion * cb_DistortionPower);
    
    //float sceneDepth = SceneDepth.Sample(PointSampler, distortedUV).r; // 歪ませたUVでシーンの深度値を取得
    
    // シーンテクスチャを歪ませたUVでサンプリング
    finalColor = g_tSceneTexture.Sample(g_sSampler, distortedUV);
    
    return finalColor;
}
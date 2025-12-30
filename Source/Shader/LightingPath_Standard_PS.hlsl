// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//
/* - @:ピクセルシェーダ -*/
//
//  【?】ディファードシェーディング
//       ライティングパス用
//
// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
#pragma once
#include "LightFunctions_H.hlsli"
#include "UtilityFunctions_H.hlsli"
#include "DebugFunctions_H.hlsli"


// ↓これがあるとRenderDocでソース単位のデバッグができるらしいがはてさて
//#pragma enable_d3d11_debug_symbols
// 追記：CSOファイル読み込みにすればデバッグ可能

SamplerState g_sSampler : register(s0);
SamplerComparisonState g_sShadowSampler : register(s1); // シャドウマップ用
Texture2D<float4> g_tAlbedoTexture : register(t0);    // xyzにアルベド
Texture2D<float4> g_tNormalTexture : register(t1);    // xyzに法線
Texture2D<float4> g_tSpecularTexture : register(t2);  // xyzにスペキュラ色  wにスペキュラ強度
Texture2D<float4> g_tDepthTexture : register(t3);     // xに深度値
Texture2D<float4> g_tShadowMapTexture : register(t4); // シャドウマップ


/* =========================================================================
/* - @:出力構造体 - */
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
    float4 albedoTex = g_tAlbedoTexture.Sample(g_sSampler, input.UV);
    float4 normalTex = g_tNormalTexture.Sample(g_sSampler, input.UV);
    float4 depthTex = g_tDepthTexture.Sample(g_sSampler, input.UV);
    float4 specularTex = g_tSpecularTexture.Sample(g_sSampler, input.UV);
    

    float4 finalCol = float4(0.0, 0.0, 0.0, 1.0);
    
    float depth = depthTex.r; // 深度値
    float4 worldPos; // 深度情報からワールド座標を計算する。
    
    // 遠すぎる物体にはライティングしない( ´Д｀)ﾉ~ﾊﾞｲﾊﾞｲ
    if (depth >= 1.0f)
    {
        return float4(0, 0, 0, 1);
    }

    // 深度値からワールド座標を求める
    worldPos = GetConvertPosFromDepthToWorld(depth, input.UV);
    
    // 法線取り出す（0～1 を -1～1 に）
    float3 normal = normalTex.xyz * 2.0f - 1.0f;
    //normal = normalize(normal); // 頂点シェーダですでに正規化済み
    
    float4 posInLVP = mul(worldPos, cb_DirLightData[0].LightViewProj);
    
    // ライトビュースクリーン空間（NDC）からUV空間に座標変換
    // -1.0 ～ 1.0 を 0.0～1.0に
    
    // 平行投影の場合はw除算は要らないっぽい
    
    float2 shadowMapUV = posInLVP.xy;
    shadowMapUV *= float2(0.5f, -0.5f);
    shadowMapUV += 0.5f;
    
    // ライトから見た深度値を計算
    float zInLVP = posInLVP.z;
    
    float shadowFactor = 1.0f;
    
    // シャドウマップの範囲内か
    if (shadowMapUV.x > 0.0f && shadowMapUV.x < 1.0f && 
        shadowMapUV.y > 0.0f && shadowMapUV.y < 1.0f)
    {
        // シャドウマップから深度値をサンプリング
        shadowFactor = g_tShadowMapTexture.SampleCmpLevelZero(g_sShadowSampler, shadowMapUV, zInLVP).r;
    }
    
    // スペキュラ強度・カラー
        float spcPow = specularTex.a;
    float3 spcColor = specularTex.rgb;
    
    spcPow *= 255;
    
    OUT_DiffAndSpec dirLig;             // ディレクション用
    dirLig.Diffuse = float3(0, 0, 0);
    dirLig.Specular = float3(0, 0, 0);
    OUT_DiffAndSpec pointLig;           // ポイント用
    pointLig.Diffuse = float3(0, 0, 0);
    pointLig.Specular = float3(0, 0, 0);
    
    // ディレクションライト計算
    for (int dirIdx = 0; dirIdx < DIRECTIONLIGHT_MAX_NUM; dirIdx++)
    { 
        OUT_DiffAndSpec res = DirectionLightCalc(cb_DirLightData[dirIdx], cb_EyePos, spcColor, spcPow, worldPos.xyz, normal);
        dirLig.Diffuse += res.Diffuse;
        dirLig.Specular += res.Specular;
    }
    
    // ポイントライト計算 
    // TODO : 定数にする
    for (int pointIdx = 0; pointIdx < 50; pointIdx++)
    {
        OUT_DiffAndSpec res = PointLightCalc(cb_PointLightData[pointIdx], cb_EyePos, spcColor, spcPow, worldPos.xyz, normal);
        pointLig.Diffuse += res.Diffuse;
        pointLig.Specular += res.Specular;
    }
    
    // 天球ライト
    float3 hemiLig = HemisphereLightCalc(normal);
    
    // ディレクションライト + ポイントライト + 天球 + アンビエント
    //float3 lighting = dirLig + pointLig + hemiLig + 0.1f;
    // 平行光源のみシャドウの影響を受ける
    float3 diffuse  = (dirLig.Diffuse * shadowFactor) + pointLig.Diffuse + hemiLig + 0.1f;
    float3 specular = (dirLig.Specular * shadowFactor) + pointLig.Specular;
    
    // 最終色 アルベド * 光度 + スペキュラ
    finalCol.xyz = albedoTex.xyz * diffuse + specular;
    finalCol.a = 1.0f;
    

    return (finalCol);
}

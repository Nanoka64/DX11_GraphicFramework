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
    worldPos.w = 1.0f;
    
    // 法線取り出す（0～1 を -1～1 に）
    float3 normal = normalTex.xyz * 2.0f - 1.0f;
    //normal = normalize(normal); // 頂点シェーダですでに正規化済み
    
    // スペキュラ強度・カラー
    float spcPow = specularTex.a;
    float3 spcColor = specularTex.rgb;
    
    // スペキュラ値を復元
    spcPow *= 255;
    
    OUT_DiffAndSpec dirLig;             // ディレクション用
    dirLig.Diffuse = float3(0, 0, 0);
    dirLig.Specular = float3(0, 0, 0);
    OUT_DiffAndSpec pointLig;           // ポイント用
    pointLig.Diffuse = float3(0, 0, 0);
    pointLig.Specular = float3(0, 0, 0);
    
    //************************************************************************
    // ディレクションライト計算
    //************************************************************************
    for (int dirIdx = 0; dirIdx < DIRECTIONLIGHT_MAX_NUM; dirIdx++)
    { 
        OUT_DiffAndSpec res = DirectionLightCalc(cb_DirLightData[dirIdx], cb_EyePos, spcColor, spcPow, worldPos.xyz, normal);
        dirLig.Diffuse += res.Diffuse;
        dirLig.Specular += res.Specular;
    }
    
    //************************************************************************
    // ポイントライト計算 
    //************************************************************************
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
    // 平行光源のみシャドウの影響を受けるので、一旦別で保持
    float3 dirDiffuse = dirLig.Diffuse;
    float3 dirSpecular = dirLig.Specular;
    float3 diffuse  =  + pointLig.Diffuse + hemiLig + 0.1f;
    float3 specular =  + pointLig.Specular;
    
    
    //************************************************************************
    // シャドウ
    //************************************************************************
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
    
    
    //return float4(shadowMapUV, 0, 1);
    // シャドウマップの範囲内か
    if (shadowMapUV.x > 0.0f && shadowMapUV.x < 1.0f &&
        shadowMapUV.y > 0.0f && shadowMapUV.y < 1.0f)
    {
        // シャドウマップから深度値をサンプリング
        // zinLVP : この値が比較するテクセルより大きければ1.0、小さければ0.0
        shadowFactor = g_tShadowMapTexture.SampleCmpLevelZero(
            g_sShadowSampler, shadowMapUV, zInLVP - 0.0005f
        );

        //return float4(1, 1, 1, 1);
        
        // 1.0 = 光が当たっている, 0.0 = 影
        dirDiffuse *= shadowFactor;
        dirSpecular *= shadowFactor;
    }
    
    //return float4(shadowFactor, shadowFactor, shadowFactor, 1.0f);
    
    //************************************************************************
    // 最終調整
    //************************************************************************
    // シャドウ計算後のディレクションライト加算
    diffuse += dirDiffuse;
    specular += dirSpecular;
    
    // 最終色 アルベド * 光度 + スペキュラ
    finalCol.xyz = albedoTex.xyz * diffuse + specular;
    finalCol.a = 1.0f;
    
    return (finalCol);
}

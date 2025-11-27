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


SamplerState g_sSampler : register(s0);
Texture2D<float4> g_tAlbedoTexture : register(t0);
Texture2D<float4> g_tNormalTexture : register(t1);   
Texture2D<float4> g_tDepthTexture : register(t2);
Texture2D<float4> g_tSpecularTexture : register(t3); // wにスペキュラ強度

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
    float4 albedoTex    = g_tAlbedoTexture.Sample(g_sSampler, input.UV);
    float4 normalTex    = g_tNormalTexture.Sample(g_sSampler, input.UV);
    float4 depthTex     = g_tDepthTexture.Sample(g_sSampler, input.UV);
    float4 specularTex  = g_tSpecularTexture.Sample(g_sSampler, input.UV);
    
    float4 finalCol = float4(1.0, 1.0, 1.0, 1.0);
    
    float depth = depthTex.r; // 深度値
    float4 worldPos;          // 深度情報からワールド座標を計算する。
    
    // 遠すぎる物体にはライティングしない( ´Д｀)ﾉ~ﾊﾞｲﾊﾞｲ
    if (depth >= 1.0f)
    {
        return float4(0, 0, 0, 1);
    }

    // 深度値からワールド座標を求める
    worldPos = GetConvertPosFromDepthToWorld(depth, input.UV);
    
    //worldPos = specularTex; スペキュラにワールド座標そのまま入れた場合
    
    // 法線取り出す（0～1 を -1～1 に）
    float3 normal = normalTex.xyz * 2.0f - 1.0f;
    //normal = normalize(normal);
    
    // スペキュラ強度・カラー
    float spcPow = specularTex.a;
    float3 spcColor = specularTex.rgb;
    
    // ディレクションライト計算
    OUT_DiffAndSpec dirLig = DirectionLightCalc(cb_DirLightData, cb_EyePos, spcColor, spcPow, worldPos.xyz, normal);
    OUT_DiffAndSpec pointLig;
    for (int i = 0; i < 2; i++)
    {
        PointLight plig;
        plig.DiffuseColor = float4(1,1,1,1);
        plig.SpecularColor = float4(1,1,1,1);
        plig.Range = cb_PointLightData[0].Range;
        plig.Pos = float3(cb_PointLightData[0].Pos.x  + 800 * i, cb_PointLightData[0].Pos.y, cb_PointLightData[0].Pos.z);
        
        // ポイントライト計算
        OUT_DiffAndSpec res = PointLightCalc(plig, cb_EyePos, spcColor, spcPow, worldPos.xyz, normal);
        pointLig.Diffuse += res.Diffuse;
        pointLig.Specular += res.Specular;
    }
    
    // 天球ライト
    float3 hemiLig = HemisphereLightCalc(normal);

    // ディレクションライト + ポイントライト + 天球 + アンビエント
    //float3 lighting = dirLig + pointLig + hemiLig + 0.1f;
    float3 diffuse = dirLig.Diffuse + pointLig.Diffuse + hemiLig + 0.1f;
    float3 specular = dirLig.Specular + pointLig.Specular;
    
    
    // 最終色 アルベド * 光度 + スペキュラ
    finalCol.xyz = albedoTex.rgb * diffuse + specular ;
    finalCol.a = 1.0f;
    return finalCol;
}

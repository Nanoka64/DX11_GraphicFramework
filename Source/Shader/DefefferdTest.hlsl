// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//
/* - @:ピクセルシェーダ -*/
//
//  【?】ディファードシェーディング
//       ライティングパス用
//
// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
#pragma once
#include "LightFunctionsHeader.hlsli"
#include "UtilityFunctionsHeader.hlsli"
#include "DebugFunctionsHeader.hlsli"

// ↓これがあるとRenderDocでソース単位のデバッグができるらしいがはてさて
//#pragma enable_d3d11_debug_symbols


SamplerState mySampler : register(s0);
Texture2D<float4> g_AlbedoTexture : register(t0);
Texture2D<float4> g_NormalTexture : register(t1);   
Texture2D<float4> g_DepthTexture : register(t2);
Texture2D<float4> g_SpecularTexture : register(t3); // wにスペキュラ強度

/* =========================================================================
/* - @:出力構造体 - */
/* =========================================================================*/
struct PS_IN
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR;
    float2 UV : TEXCOORD;
};
float rand(float2 uv)
{
    return frac(sin(dot(uv.xy, float2(150.9898, 701.233))) * 43758.5453);
}
float rand2(float2 uv)
{
    return frac(sin(dot(uv.xy, float2(502.9898, 181.233))) * 43758.5453);
}

// **************************************************************************
/* - @:エントリーポイント - */
// **************************************************************************
float4 PSMain(PS_IN input) : SV_TARGET
{
    float4 albedoTex   = g_AlbedoTexture.Sample(mySampler, input.UV);
    float4 normalTex   = g_NormalTexture.Sample(mySampler, input.UV);
    float4 depthTex    = g_DepthTexture.Sample(mySampler, input.UV);
    float4 specularTex = g_SpecularTexture.Sample(mySampler, input.UV);
    
    float4 finalCol = float4(1.0, 1.0, 1.0, 1.0);
    
    float depth = depthTex.r; // 深度値
    float4 worldPos;          // 深度情報からワールド座標を計算する。
    
    // 遠すぎる物体にはライティングしない( ´Д｀)ﾉ~ﾊﾞｲﾊﾞｲ
    if (depth >= 1.0f)
    {
        return float4(0, 0, 0, 1);
    }

    // 深度値からワールド座標を求める
    //worldPos = GetConvertPosFromDepthToWorld(depth, input.UV);
    worldPos = specularTex;
    
    //return Debug_Pos(worldPos);
    
    // 法線取り出す（0～1 を -1～1 に）
    float3 normal = normalTex.xyz * 2.0f - 1.0f;
    //normal = normalize(normal);
    
    
    // スペキュラ強度
    //float spcPow = specularTex.w;
    //float3 speColor = specularTex.rgb;
    float spcPow = 1;
    float3 spcColor = float3(1, 1, 1);
    
    // ディレクションライト計算
    OUT_DiffAndSpec dirLig = DirectionLightCalc(cb_DirLightData, EyePos, spcColor, spcPow, worldPos.xyz, normal);
    OUT_DiffAndSpec pointLig;
    for (int i = 0; i < 100; i++)
    {
        PointLight plig;
        plig.DiffuseColor = float4(i * 0.01, i * 0.03, i * 0.02, 1);
        plig.SpecularColor = float4(i * 0.01, i * 0.03, i * 0.02, 1);
        plig.Range = 150;
        plig.Pos = float3(worldPos.x + i * rand(input.UV), cb_PointLightData.Pos.y, worldPos.z + i * rand2(input.UV));
        
        // ポイントライト計算
        OUT_DiffAndSpec res = PointLightCalc(plig, EyePos, spcColor, spcPow, worldPos.xyz, normal);
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
    finalCol.xyz = saturate(finalCol.xyz);
    finalCol.a = 1.0f;
    return finalCol;
}

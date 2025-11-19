// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//
/* - @:ピクセルシェーダ -*/
//
//  【?】ディファードシェーディング
//       ライティングパス用
//
// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
#pragma once
#include "LightFunctionHeader.hlsli"

SamplerState mySampler : register(s0);

Texture2D<float4> g_AlbedoTexture : register(t0);
Texture2D<float4> g_NormalTexture : register(t1);   
Texture2D<float>  g_DepthTexture : register(t2);
Texture2D<float4> g_SpecularTexture : register(t3);    // wにスペキュラ強度

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
    float4 albedoTex = g_AlbedoTexture.Sample(mySampler, input.UV);
    float4 normalTex = g_NormalTexture.Sample(mySampler, input.UV);
    float depthTex = g_DepthTexture.Sample(mySampler, input.UV);
    float4 specularTex = g_SpecularTexture.Sample(mySampler, input.UV);
    
    float4 finalCol = float4(1.0, 1.0, 1.0, 1.0);
    
    // 深度情報からワールド座標を計算する。
    float4 worldPos;

    // 正規化スクリーン座標系での座標を計算する。
    // z座標は深度テクスチャから引っ張ってくる。
    worldPos.z = depthTex;
    
    // xy座標はUV座標から計算する。
    worldPos.xy = input.UV * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f);
    worldPos.w = 1.0f;
    
    // クリップ座標にビュープロジェクション行列の逆行列を乗算して、ワールド座標に戻す。
    worldPos = mul(viewProjInvMatrix, worldPos );
    //worldPos.xyz /= worldPos.w;   // wで割るとうまく反映されない
    
    // 法線取り出す
    normalTex.xyz = (normalTex.xyz * 2.0f) - 1.0f;
    float3 normal = normalize(normalTex.xyz);
    
    // スペキュラ強度
    float spcPow = specularTex.w;
    
    // ディレクションライト計算
    OUT_DiffAndSpec dirLig = DirectionLightCalc(cb_DirLightData, specularTex.rgb, spcPow, worldPos.xyz, normal);

    // ポイントライト計算
    OUT_DiffAndSpec pointLig = PointLightCalc(cb_PointLightData, specularTex.rgb, spcPow, worldPos.xyz, normal);
    
    // 天球ライト
    float3 hemiLig = HemisphereLightCalc(normal);
    
    // ディレクションライト + ポイントライト + 天球 + アンビエント
    //float3 lighting = dirLig + pointLig + hemiLig + 0.1f;
    float3 lighting = dirLig.Diffuse + pointLig.Diffuse + hemiLig + 0.5f;
    float3 specular = dirLig.Specular + pointLig.Specular;
    
    // スペキュラ強度の反映
    specular *= spcPow;
    
    // 最終色 アルベド * 光度 + スペキュラ
    finalCol.xyz = albedoTex.rgb * lighting + specular;
    finalCol.a = 1.0f;
    return finalCol;
}
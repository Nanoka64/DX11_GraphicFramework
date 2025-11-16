// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//
/* - @:ピクセルシェーダ -*/
//
//  【?】特に凝った処理を行わないシンプルなピクセルシェーダ
//		 頂点カラーをそのまま出力
//
// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
#pragma once
#include "LightFunctionHeader.hlsli"    // ライトヘッダー

SamplerState mySampler : register(s0);
Texture2D g_DiffuseTex : register(t0); // ディフューズ
Texture2D g_NormalTex : register(t1); // ノーマル
Texture2D g_SpecularTex : register(t2); // スペキュラ


//ピクセルシェーダーの出力構造体
struct PSOutPut
{
    float4 Color : SV_Target0;
    float4 Normal : SV_Target1;
    float4 Depth : SV_Target2;
};


// **************************************************************************
/* - @:エントリーポイント - */
// **************************************************************************
PSOutPut SimplePSMain(PS_SimpleLightingInput input) : SV_Target0
{
    float4 diffuseMap = g_DiffuseTex.Sample(mySampler, input.UV);
    float4 normalMap = g_NormalTex.Sample(mySampler, input.UV);
    
    float4 finalCol = float4(1.0, 1.0, 1.0, 1.0);
    
    finalCol = diffuseMap * DiffuseColor;
    
    // テスト出力
    PSOutPut output;
    output.Color   = finalCol;
    output.Normal  = float4(input.Normal.xyz, 1.0);
    output.Depth.x = input.WPos.z;
    return output;
    
    
    // 法線をTBN空間 ワールドスペースに変換して取得
    float3 normal = GetNorm(normalMap, float3(1.0, 1.0, 1.0), float3(1.0, 1.0, 1.0), input.Normal);
    
    // スペキュラ色
    float3 specularCol = SpecularColor.xyz * cb_PointLightData.SpecularColor;
    
    // ディレクションライト計算
    float3 dirLig = DirectionLightCalc(cb_DirLightData, specularCol, SpecularPower, input.WPos.xyz, input.Normal);
    
    // ポイントライト計算
    float3 pointLig = PointLightCalc(cb_PointLightData, specularCol, SpecularPower, input.WPos.xyz, input.Normal);
    
    // 天球ライト
    float3 hemiLig = HemisphereLightCalc(input.Normal);
    
    // ディレクションライト + ポイントライト + 天球 + アンビエント
    float3 lighting = dirLig + pointLig + hemiLig + 0.1f;

    // 最終色
    finalCol.xyz *= lighting;
    
    //return float4(finalCol);
}
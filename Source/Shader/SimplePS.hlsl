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


// **************************************************************************
/* - @:エントリーポイント - */
// **************************************************************************
float4 SimplePSMain(PS_SimpleLightingInput input) : SV_TARGET
{
    float4 diffuseMap = g_DiffuseTex.Sample(mySampler, input.UV);
    float4 normalMap = g_DiffuseTex.Sample(mySampler, input.UV);
    
    float4 finalCol = float4(1.0, 1.0, 1.0, 1.0);
    finalCol = diffuseMap * DiffuseColor;
    
    // 法線をTBN空間 ワールドスペースに変換して取得
    float3 normal = GetNorm(normalMap, float3(1.0, 1.0, 1.0), float3(1.0, 1.0, 1.0), input.Normal);
    
    // スペキュラ色
    float3 specularCol = SpecularColor.xyz * cb_PointLightData.SpecularColor;
    
    // ディレクションライト計算
    float3 dirLig = DirectionLightCalc(cb_DirLightData, specularCol, SpecularPower, input.WPos.xyz, normal);
    
    // ポイントライト計算
    float3 pointLig = PointLightCalc(cb_PointLightData, specularCol, SpecularPower, input.WPos.xyz, normal);
    
    // 天球ライト
    float3 hemiLig = HemisphereLightCalc(normal);
    
    // ディレクションライト + ポイントライト + 天球 + アンビエント
    float3 lighting = dirLig + pointLig + hemiLig + 0.1f;

    // 最終色
    finalCol.xyz *= lighting;
    
    return float4(finalCol);
}
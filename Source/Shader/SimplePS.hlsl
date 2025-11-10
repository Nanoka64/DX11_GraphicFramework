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
    float4 diffuseColor = g_DiffuseTex.Sample(mySampler, input.UV);
    
    float4 finalCol = float4(1.0, 1.0, 1.0, 1.0);
    finalCol = diffuseColor;
    
    // ディレクションライト計算
    float3 dirLig = DirectionLightCalc(cb_DirLightData, 2.0f, input.World.xyz, input.Normal.xyz);
    
    // ポイントライト計算
    float3 pointLig = PointLightCalc(cb_PointLightData, 2.0f,input.World.xyz, input.Normal);
    
    // 天球ライト
    float3 hemiLig = HemisphereLightCalc(input.Normal);

    
    // 天球ライト加算
    finalCol.xyz += hemiLig;
    
    // 最終色
    finalCol.xyz *= dirLig + pointLig;
    
    return float4(finalCol);
}
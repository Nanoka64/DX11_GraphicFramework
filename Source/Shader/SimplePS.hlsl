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
    float3 dirLig = DirectionLightCalc(cb_DirLightData, input.World.xyz, input.Normal.xyz);
    
    // ポイントライト計算
    float3 pointLig = PointLightCalc(cb_PointLightData, input.World.xyz, input.Normal);
    
    // 最終色
    finalCol.xyz *= pointLig;
    
    return float4(finalCol);
}
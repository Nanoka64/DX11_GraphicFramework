// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//
/* - @:ピクセルシェーダ -*/
//
//  【?】モデル用
//
// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
#pragma once
#include "LightFunctionHeader.hlsli"    // ライトヘッダー


/* =========================================================================
/* - @:PS_INPUT構造体 - */
/* =========================================================================*/
struct PS_INPUT
{
    float4 Pos      : SV_POSITION;
    float4 WPos     : POSITION;
    float3 Normal   : NORMAL0;
    float4 Col      : COLOR0;
    float2 UV       : TEXCOORD0;
    
    float3 Tan      : TANGENT;  // 接ベクトル
    float3 BiNorm   : BINORMAL; // 従ベクトル
};


// **************************************************************************
/* - @:エントリーポイント - */
// **************************************************************************
float4 PS(PS_INPUT input) : SV_TARGET // ピクセルシェーダの出力はSV_Targetを指定しないといけない(描画ターゲットごとに末尾変更0,1,2...)
{
    float4 diffuseMap = g_DiffuseTex.Sample(mySampler, input.UV);
    float4 normalMap = g_NormalTex.Sample(mySampler, input.UV);
    float specularMap = g_SpecularTex.Sample(mySampler, input.UV).r;
    float smooth = g_SpecularTex.Sample(mySampler, input.UV).a;
    
    // 最終出力用
    float4 finalCol = float4(1.0, 1.0, 1.0, 1.0);
    
    // ディフューズマップとcpp側で設定したカラーを足す
    finalCol = diffuseMap * DiffuseColor;
    
    // 法線をTBN空間 ワールドスペースに変換して取得
    float3 normal = GetNorm(normalMap, input.Tan, input.BiNorm, input.Normal);
    
    
    //finalCol.xyz = normal.xyz;
    //return finalCol;

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
    
    //finalCol = c * diffuseColor;    // 頂点カラーとディフューズを乗算
    //return float4(c);               // 頂点カラー
    //return float4(t, 1, 1);         // UVカラー
    return saturate(finalCol);        // テクスチャカラー
}





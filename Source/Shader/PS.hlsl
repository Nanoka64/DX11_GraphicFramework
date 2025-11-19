// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//
/* - @:ピクセルシェーダ -*/
//
//  【?】モデル用
//
// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
#pragma once
#include "LightFunctionHeader.hlsli"    // ライトヘッダー
SamplerState mySampler : register(s0);
Texture2D g_DiffuseTex : register(t0); // ディフューズ
Texture2D g_NormalTex : register(t1); // ノーマル
Texture2D g_SpecularTex : register(t2); // スペキュラ

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


//ピクセルシェーダーの出力構造体
struct PSOutPut
{
    float4 Albedo : SV_Target0;
    float4 Normal : SV_Target1;
    float Depth : SV_Target2;
    float4 Specular : SV_Target3;
};


// **************************************************************************
/* - @:エントリーポイント - */
// **************************************************************************
PSOutPut PS(PS_INPUT input) 
{
    float4 diffuseMap = g_DiffuseTex.Sample(mySampler, input.UV);
    float4 normalMap = g_NormalTex.Sample(mySampler, input.UV);
    float specularMap = g_SpecularTex.Sample(mySampler, input.UV).r;
    float smooth = g_SpecularTex.Sample(mySampler, input.UV).a;
    
    // 最終出力用
    float4 finalCol = float4(1.0, 1.0, 1.0, 1.0);
    
    // ディフューズマップとcpp側で設定したカラーを足す
    finalCol = diffuseMap * DiffuseColor;
    float3 normal = GetNorm(normalMap, input.Tan, input.BiNorm, input.Normal);
    
    // テスト出力
    PSOutPut output;
    output.Albedo = finalCol;
    output.Normal.xyz = normal;
    output.Normal.w = 1.0f;
    output.Depth = input.Pos.z;
    output.Specular.xyz = SpecularColor.xyz;
    output.Specular.w = SpecularPower; // wに反射強度入れる
    return output;
    
    //// 法線をTBN空間 ワールドスペースに変換して取得
    //float3 normal = GetNorm(normalMap, input.Tan, input.BiNorm, input.Normal);
    
    ////finalCol.xyz = normal.xyz;
    ////return finalCol;

    //float3 specularCol = SpecularColor.xyz * cb_PointLightData.SpecularColor;
    
    //// ディレクションライト計算
    //float3 dirLig = DirectionLightCalc(cb_DirLightData, specularCol, SpecularPower, input.WPos.xyz, normal);
    
    //// ポイントライト計算
    //float3 pointLig = PointLightCalc(cb_PointLightData, specularCol, SpecularPower, input.WPos.xyz, normal);
    
    //// 天球ライト
    //float3 hemiLig = HemisphereLightCalc(normal);

    //// ディレクションライト + ポイントライト + 天球 + アンビエント
    //float3 lighting = dirLig + pointLig + hemiLig + 0.1f;
    
    //// 最終色
    //finalCol.xyz *= lighting;
    
    //finalCol = c * diffuseColor;    // 頂点カラーとディフューズを乗算
    //return float4(c);               // 頂点カラー
    //return float4(t, 1, 1);         // UVカラー
    //return saturate(finalCol);        // テクスチャカラー
}





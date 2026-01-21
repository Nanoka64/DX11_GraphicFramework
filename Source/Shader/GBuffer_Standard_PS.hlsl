// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//
/* - @:Gパス -*/
//
//  【?】標準GBuffer
//       Simpleとの違いは接ベクトルの、従ベクトルがあるかどうか
//
// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
#pragma once
#include "LightFunctions_H.hlsli"           // ライトヘッダー
SamplerState g_sSampler : register(s0);
Texture2D g_tDiffuseTex : register(t0);     // ディフューズ
Texture2D g_tNormalTex : register(t1);      // ノーマル
Texture2D g_tSpecularTex : register(t2);    // スペキュラ

/* =========================================================================
/* - @:PS_INPUT構造体 - */
/* =========================================================================*/
struct PS_IN
{
    float4 Pos      : SV_POSITION;
    float4 WPos     : POSITION;
    float3 Normal   : NORMAL0;
    float4 Color    : COLOR0;
    float2 UV       : TEXCOORD0;
    
    float3 Tan      : TANGENT;  // 接ベクトル
    float3 BiNorm   : BINORMAL; // 従ベクトル
};


//ピクセルシェーダーの出力構造体
struct PS_OUT
{
    float4 Albedo : SV_Target0;
    float4 Normal : SV_Target1;
    float4 Specular : SV_Target2;
    float4 Emissive : SV_Target3;
    float4 Depth : SV_Target4;
};
// **************************************************************************
/* - @:エントリーポイント - */
// **************************************************************************
PS_OUT PSMain(PS_IN input)
{
    float4 diffuseMap   = g_tDiffuseTex.Sample(g_sSampler, input.UV);
    float4 normalMap    = g_tNormalTex.Sample(g_sSampler, input.UV);
    float4 specularMap   = g_tSpecularTex.Sample(g_sSampler, input.UV);
    float smooth        = g_tSpecularTex.Sample(g_sSampler, input.UV).a;
    
    // 最終出力用
    float4 finalCol = float4(1.0, 1.0, 1.0, 1.0);
    
    // ディフューズマップとcpp側で設定したカラーを足す
    finalCol = diffuseMap * cb_DiffuseColor;
    float3 normal = GetNorm(normalMap, input.Tan, input.BiNorm, input.Normal);
    
    float3 emissiveColor = cb_EmissiveColor * cb_EmissivePower;
    
    // テスト出力
    PS_OUT output;
    output.Albedo   = finalCol;
    output.Normal.rgb   = (normal * 0.5f) + 0.5f;
    output.Normal.a     = 1.0f; 
    output.Specular.rgb = specularMap.rgb + cb_SpecularColor.rgb;
    output.Specular.a   = (cb_SpecularPower) / (255.0f); // wに反射強度入れる
    output.Emissive.rgb = emissiveColor; // 発光カラー
    output.Emissive.a = 1.0f; 
    
    output.Depth.gba;
    output.Depth.r;
    //output.Specular = input.WPos;
    return output;
}





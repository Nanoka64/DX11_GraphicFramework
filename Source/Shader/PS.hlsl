// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//
/* - @:ピクセルシェーダ -*/
//
//  【?】モデル用
//
// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
#pragma once
#include "LightFunctions_H.hlsli"    // ライトヘッダー
SamplerState g_sSampler : register(s0);
Texture2D g_tDiffuseTex : register(t0); // ディフューズ
Texture2D g_tNormalTex : register(t1); // ノーマル
Texture2D g_tSpecularTex : register(t2); // スペキュラ

/* =========================================================================
/* - @:PS_INPUT構造体 - */
/* =========================================================================*/
struct PS_IN
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
struct PS_OUT
{
    float4 Albedo   : SV_Target0;
    float4 Normal   : SV_Target1;
    float4 Specular : SV_Target2;
    float4 Depth    : SV_Target3;
};


// **************************************************************************
/* - @:エントリーポイント - */
// **************************************************************************
PS_OUT PS(PS_IN input)
{
    float4 diffuseMap   = g_tDiffuseTex.Sample(g_sSampler, input.UV);
    float4 normalMap    = g_tNormalTex.Sample(g_sSampler, input.UV);
    float specularMap   = g_tSpecularTex.Sample(g_sSampler, input.UV).r;
    float smooth        = g_tSpecularTex.Sample(g_sSampler, input.UV).a;
    
    // 最終出力用
    float4 finalCol = float4(1.0, 1.0, 1.0, 1.0);
    
    // ディフューズマップとcpp側で設定したカラーを足す
    finalCol = diffuseMap * cb_DiffuseColor;
    float3 normal = GetNorm(normalMap, input.Tan, input.BiNorm, input.Normal);
    
    // テスト出力
    PS_OUT output;
    output.Albedo = finalCol;
    output.Normal.xyz = (normal * 0.5f) + 0.5f;
    output.Normal.w = 1.0f;
    output.Depth = float4(0, 0, 0, 0);
    output.Specular.xyz = cb_SpecularColor.xyz;
    output.Specular.w = (cb_SpecularPower) / (255.0f); // wに反射強度入れる
    //output.Specular = input.WPos;
    return output;
}





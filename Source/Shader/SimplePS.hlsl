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
Texture2D g_DiffuseTex : register(t0);  // ディフューズ
Texture2D g_NormalTex : register(t1);   // ノーマル
Texture2D g_SpecularTex : register(t2); // スペキュラ


//ピクセルシェーダーの出力構造体
struct PSOutPut
{
    float4 Albedo : SV_Target0;
    float4 Normal : SV_Target1;
    float Depth : SV_Target2;
    float4 Specular : SV_Target3;
};


// ディザパターン
static const int ditherPattern[4][4] =
{
    { 0, 32,8, 40 },
    { 48,16,56,24 },
    { 12,44,4, 36 },
    { 60,28,52,20 },
};

// **************************************************************************
/* - @:エントリーポイント - */
// **************************************************************************
PSOutPut SimplePSMain(PS_SimpleLightingInput input)
{
    float4 diffuseMap = g_DiffuseTex.Sample(mySampler, input.UV);
    float4 normalMap = g_NormalTex.Sample(mySampler, input.UV);
    
    normalMap.y *= -1.0f;
    
    float4 finalCol = float4(1.0, 1.0, 1.0, 1.0);
    
    finalCol = diffuseMap * DiffuseColor;
    float3 normal = GetNorm(normalMap, float3(0.0, 1.0, 0.0), float3(0.0, 1.0, 0.0), input.Normal);
    
    // このピクセルのスクリーン座標系でのX座標、Y座標を4で割った余りを求める
    int x = (int) fmod(input.Pos.x, 4.0f);
    int y = (int) fmod(input.Pos.y, 4.0f);
    
    // このピクセルの閾値を取得
    int dither = ditherPattern[x][y];
    
    // 閾値が 10 以下のピクセルはピクセルキルする
    //clip(dither - 50);

    
    // テスト出力
    PSOutPut output;
    output.Albedo = finalCol;
    output.Normal.xyz = input.Normal;
    output.Normal.w = 1.0f;
    output.Depth = input.Pos.z;    
    output.Specular.xyz = SpecularColor.xyz;
    output.Specular.w = SpecularPower; // wに反射強度入れる
    return output;
}
// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//
/* - @:Gパス -*/
//
//  【?】簡易GBuffer
//      接ベクトル、従ベクトルを使わないシンプルな実装
//
// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
#pragma once
#include "LightFunctions_H.hlsli"           // ライトヘッダー
SamplerState g_sSampler : register(s0);
Texture2D g_tDiffuseTex : register(t0);     // ディフューズ
Texture2D g_tNormalTex : register(t1);      // ノーマル
Texture2D g_tSpecularTex : register(t2);    // スペキュラ




// ディザパターン
static const int ditherPattern[4][4] =
{
    { 0, 32,8, 40 },
    { 48,16,56,24 },
    { 12,44,4, 36 },
    { 60,28,52,20 },
};

/* =========================================================================
/* - @:入力構造体 - */
//* 【?】ライティング用
/* =========================================================================*/
struct PS_IN
{
    float4 Pos : SV_Position;
    float4 WPos : WORLD0;
    float3 Normal : NORMAL0;
    float4 Color : COLOR0;
    float2 UV : TEXCOORD0;
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
PS_OUT PSMain(PS_IN input)
{
    float4 diffuseMap = g_tDiffuseTex.Sample(g_sSampler, input.UV);
    float4 normalMap = g_tNormalTex.Sample(g_sSampler, input.UV);
    
    float4 finalCol = float4(1.0, 1.0, 1.0, 1.0);
    
    finalCol = diffuseMap * cb_DiffuseColor;
    float3 normal = GetNorm(normalMap, float3(1.0, 1.0, 1.0), float3(1.0, 1.0, 1.0), input.Normal);
    
    // このピクセルのスクリーン座標系でのX座標、Y座標を4で割った余りを求める
    int x = (int) fmod(input.Pos.x, 4.0f);
    int y = (int) fmod(input.Pos.y, 4.0f);
    
    // このピクセルの閾値を取得
    int dither = ditherPattern[x][y];
    
    // 閾値が 10 以下のピクセルはピクセルキルする
    //clip(dither - 10);
    

    // テスト出力
    PS_OUT output;
    output.Albedo       = finalCol;
    output.Normal.xyz   = (normal * 0.5f) + 0.5f; // 0～1に収める
    output.Normal.w     = 1.0f;
    output.Specular.xyz = cb_SpecularColor.xyz;
    output.Specular.w   = (cb_SpecularPower) / (255.0f);        // wに反射強度入れる（0～1に)
    output.Depth;
    
    //output.Depth        = input.WPos;   // ワールド座標そのまま入れる
    
    // 以下のように深度値を手動で入れてもライティングパス時には反映されないよ
    // 理由はDSVをパイプラインにバインドしているので、ハードウェア側が自動で深度値を入れてくれている。
    // output.Depth.x      = 0;
    
    return output;
}


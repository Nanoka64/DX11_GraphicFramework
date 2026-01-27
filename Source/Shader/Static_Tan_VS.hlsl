// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//
/* - @:頂点シェーダ -*/
//
//  【?】静的3Dオブジェクト用
//       法線マップあり
//
// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
#pragma once
#include "ConstantBuffers_H.hlsli"


/* =========================================================================
/* - @:VS_INPUT構造体 - */
/* =========================================================================*/
struct VS_INPUT
{
    float3 Pos      : POSITION; // 頂点座標
    float2 UV       : TEXCOORD; // テクスチャ座標
    float4 Color    : COLOR;    // 頂点色
    float3 Normal   : NORMAL;   // 法線
    float3 Tan      : TANGENT;  // 接ベクトル
    float3 BiNorm   : BINORMAL; // 従ベクトル
};

/* =========================================================================
/* - @:VS_OUTPUT構造体 - */
/* =========================================================================*/
struct VS_OUTPUT
{
    float4 Pos      : SV_POSITION;  // 頂点座標(画面空間)
    float4 WPos     : POSITION;     // ライティング用のワールド座標
    float3 Normal   : NORMAL0;      // 法線
    float4 Color    : COLOR0;       // 頂点色
    float2 UV       : TEXCOORD0;    // テクスチャ座標
    
    float3 Tan      : TANGENT;      // 接ベクトル
    float3 BiNorm   : BINORMAL;     // 従ベクトル
};

// **************************************************************************
/* - @:エントリーポイント - */
// **************************************************************************
VS_OUTPUT VSMain(VS_INPUT input)
{
    VS_OUTPUT output;
    float4 pos = float4(input.Pos, 1.0f);
    float3 norm = input.Normal;
    float2 uv = input.UV;
    float4 color = input.Color;
    
    norm = normalize(mul(norm, (float3x3) cb_Transform));
    
    pos = mul(pos, cb_Transform);   // ワールド変換（ワールド空間）
    output.WPos = pos;
    pos = mul(pos, cb_View);        // ビュー変換（ビュー空間）
    pos = mul(pos, cb_Projection);  // 投影変換（クリッピング空間）
    
    // UVオフセットを足す
    // frac : 0以上1未満を返す
    input.UV = (input.UV + cb_OffsetUV);
    
    output.Pos = pos;               // 画面空間の頂点座標
    output.Normal = norm.xyz;       // 法線
    output.UV = input.UV;           // テクスチャ座標
    output.Color = color;
    
    // 接ベクトルと従ベクトルをワールド空間に変換する
    output.Tan = normalize(mul(input.Tan, (float3x3) cb_Transform));
    output.BiNorm = normalize(mul(input.BiNorm, (float3x3) cb_Transform));
    
    return output;
}
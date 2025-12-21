// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//
/* - @:頂点シェーダ -*/
//
//  【?】特に凝った処理を行わないシンプルな頂点シェーダ
//
// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
#pragma once
#include "ConstantBuffers_H.hlsli"  // 共通ヘッダー


/* =========================================================================
/* - @:入力構造体 - */
/* =========================================================================*/
struct VS_SimpleInput
{
    float3 Pos      : POSITION;
    float2 UV       : TEXCOORD;
    float4 Color    : COLOR;
    float3 Normal   : NORMAL;
};

/* =========================================================================
/* - @:出力構造体 - */
/* =========================================================================*/
struct VS_SimpleOutput
{
    float4 Pos      : SV_Position;
    float4 World    : WORLD0;
    float3 Normal   : NORMAL0;
    float4 Color    : COLOR0;
    float2 UV       : TEXCOORD0;
};

// **************************************************************************
/* - @:エントリーポイント - */
// **************************************************************************
VS_SimpleOutput VSMain(VS_SimpleInput input)
{
    VS_SimpleOutput output;
    float4 pos   = float4(input.Pos, 1.0f);
    float3 norm  = input.Normal;
    float2 uv    = input.UV;
    float4 color = input.Color;
    
    norm = normalize(mul(norm, (float3x3) cb_Transform));
    
    pos = mul(pos, cb_Transform);  // ワールド変換（ワールド空間）
    output.World = pos;
    pos = mul(pos, cb_View);       // ビュー変換（ビュー空間）
    pos = mul(pos, cb_Projection); // 投影変換（クリッピング空間）
    
    output.Pos    = pos;        // 画面空間の頂点座標
    output.Normal = norm.xyz;   // 法線
    output.UV     = input.UV;   // テクスチャ座標
    output.Color  = color;
    
    return output;
}
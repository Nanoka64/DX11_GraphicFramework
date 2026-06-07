// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//
/* - @:頂点シェーダ -*/
//
//
//  【?】静的簡易3Dオブジェクト用
//       法線マップなし
//
//
// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
#pragma once
#include "ConstantBuffers_H.hlsli"  // 共通ヘッダー


/* =========================================================================
/* - @:入力構造体 - */
/* =========================================================================*/
struct VS_INPUT
{
    float3 Pos : POSITION;
    float2 UV : TEXCOORD;
    float4 Color : COLOR;
    float3 Normal : NORMAL;
};

/* =========================================================================
/* - @:出力構造体 - */
/* =========================================================================*/
struct VS_OUTPUT
{
    float4 Pos       : SV_Position;
    float4 PosInProj : TEXCOORD0; // 投影空間の頂点座標
    float2 UV        : TEXCOORD1;
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
    
    output.Pos = mul(pos, cb_Transform);            // ワールド変換
    output.Pos = mul(output.Pos, cb_View);          // ビュー返還
    output.Pos = mul(output.Pos, cb_Projection);    // 投影変換
    
    output.UV = input.UV;       // テクスチャ座標
    
    // 正規化スクリーン座標系の座標を求める
    output.PosInProj = output.Pos; 
    output.PosInProj.xyz /= output.PosInProj.w;
    
    
    return output;
}
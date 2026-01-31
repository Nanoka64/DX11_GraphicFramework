// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//
/* - @:頂点シェーダ -*/
//
//  【?】スプライト用頂点シェーダ
//
// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
#pragma once;
#include "ConstantBuffers_H.hlsli"
/* =========================================================================
/* - @:スプライト入力構造体 - */
/* =========================================================================*/
struct VS_IN
{
    float3 Pos      : POSITION; // 頂点座標
    float2 UV       : TEXCOORD; // 法線（cpp側の都合で入れているが基本使わない）
    float4 Color    : COLOR;    // 頂点色
    float3 Normal   : NORMAL;   // テクスチャ座標
};
/* =========================================================================
/* - @:スプライト出力構造体 - */
/* =========================================================================*/
struct VS_OUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR;
    float2 UV : TEXCOORD;
};

Texture2D g_tTexture : register(t0);

// **************************************************************************
/* - @:エントリーポイント - */
// **************************************************************************
VS_OUT VSMain(VS_IN input)
{
    VS_OUT output;
    float4 pos = float4(input.Pos, 1.0f);
    float2 uv = input.UV;
    float4 color = input.Color;
    
    // uvオフセット
    uv += cb_Sprite_OffsetUV;
    
    pos = mul(pos, cb_Transform);
    //pos = mul(cb_View, pos);
    pos = mul(pos, cb_OrthographicProjection);
    
    output.Pos   = pos;
    output.Color = color;
    output.UV    = uv;
    return output;
}
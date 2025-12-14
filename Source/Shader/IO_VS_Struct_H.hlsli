#ifndef IO_VS_STRUCT_HLSLI
#define IO_VS_STRUCT_HLSLI

/* =========================================================================
/* - @:スプライト入力構造体 - */
/* =========================================================================*/
struct VS_IN
{
    float3 Pos : POSITION; // 頂点座標
    float3 Normal : NORMAL; // 法線（cpp側の都合で入れているが基本使わない）
    float4 Color : COLOR; // 頂点色
    float2 UV : TEXCOORD; // テクスチャ座標
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
#endif
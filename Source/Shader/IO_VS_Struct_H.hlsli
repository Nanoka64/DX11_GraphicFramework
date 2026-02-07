#ifndef IO_VS_STRUCT_HLSLI
#define IO_VS_STRUCT_HLSLI


struct VS_STATIC_INPUT
{
    float3 Pos : POSITION; // 頂点座標
    float3 Normal : NORMAL; // 法線（cpp側の都合で入れているが基本使わない）
    float4 Color : COLOR; // 頂点色
    float2 UV : TEXCOORD; // テクスチャ座標
};

struct VS_STATIC_TAN_INPUT
{
    float3 Pos      : POSITION; // 頂点座標
    float2 UV       : TEXCOORD; // テクスチャ座標
    float4 Color    : COLOR;    // 頂点色
    float3 Normal   : NORMAL;   // 法線
    float3 Tan      : TANGENT;  // 接ベクトル
    float3 BiNorm   : BINORMAL; // 従ベクトル
};

struct VS_SKINNED_INPUT
{
    float3 Pos         : POSITION;    // 頂点座標
    float2 UV          : TEXCOORD;    // テクスチャ座標
    float4 Color       : COLOR;       // 頂点色
    float3 Normal      : NORMAL;      // 法線
    float3 Tan         : TANGENT;     // 接ベクトル
    float3 BiNorm      : BINORMAL;    // 従ベクトル
    uint4 boneIDs      : BONEIDS;     // ボーンID
    float4 boneWeights : BONEWEIGHTS; // ボーンウェイト
};

struct VS_OUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR;
    float2 UV : TEXCOORD;
};
#endif
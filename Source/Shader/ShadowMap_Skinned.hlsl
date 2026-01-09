// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//
/* - @:シャドウマップシェーダ -*/
//
//  【?】スキニングモデル用
//       
//
//
// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
#pragma once
#include "ConstantBuffers_H.hlsli"  // 共通ヘッダー


/* =========================================================================
/* - @:VS_INPUT構造体 - */
/* =========================================================================*/
struct VS_INPUT
{
    float3 Pos          : POSITION;     // 頂点座標
    float2 UV           : TEXCOORD;     // テクスチャ座標
    float4 Color        : COLOR;        // 頂点色
    float3 Normal       : NORMAL;       // 法線
    float3 Tan          : TANGENT;      // 接ベクトル
    float3 BiNorm       : BINORMAL;     // 従ベクトル
    uint4 boneIDs       : BONEIDS;      // ボーンID
    float4 boneWeights  : BONEWEIGHTS;  // ボーンウェイト
};
/* =========================================================================
/* - @:出力構造体 - */
/* =========================================================================*/
struct PS_INPUT
{
    float4 Pos : SV_Position;
    float2 UV : TEXCOORD0;
};

// **************************************************************************
/* - @:頂点 エントリーポイント - */
// **************************************************************************
PS_INPUT VSMain(VS_INPUT input)
{
    PS_INPUT output;
    float4 pos = float4(input.Pos, 1.0f);
    float2 uv = input.UV;
    
    float4 skinnedPos = float4(0, 0, 0, 0); // スキニング後の頂点座標
    
    // 4つのボーンでスキニング
    for (int i = 0; i < 4; i++)
    {
        // ボーンIDが-1以上のときだけ計算する
        if (input.boneIDs[i] >= 0)
        {
            // 頂点座標を求める
            skinnedPos += mul(cb_BoneMatrices[input.boneIDs[i]], float4(input.Pos, 1.0f)) * input.boneWeights[i];
        }
    }
    pos = skinnedPos;
    
    pos = mul(pos, cb_Transform); // ワールド変換（ワールド空間）
    
    // ライトのビュープロジェクション行列でライトから見た座標に変換     
    pos = mul(pos, cb_DirLightData[0].LightViewProj);
    
    output.Pos = pos; // 画面空間の頂点座標
    output.UV = input.UV; // テクスチャ座標
    
    return output;
}

// **************************************************************************
/* - @:ピクセル エントリーポイント - */
// **************************************************************************
float4 PSMain(PS_INPUT input) : SV_TARGET0
{
    float4 finalColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    
    finalColor.xyz = 0.1f;
    
    return finalColor;
}
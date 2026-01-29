// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//
/* - @:ピクセルシェーダ -*/
//
//  【?】スカイボックス用
//      参考サイト：https://www.braynzarsoft.net/viewtutorial/q16390-20-cube-mapping-skybox
//                  https://rovecoder.net/article/directx-11/skybox
//
// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
#pragma once
#include "UtilityFunctions_H.hlsli"
SamplerState g_sSampler : register(s0);
TextureCube g_tSkyboxTexture : register(t0); // キューブマップ用


/* =========================================================================
/* - @:入力構造体 - */
/* =========================================================================*/
struct PS_IN
{
    float4 Pos : SV_POSITION;
    float3 TexCoord : TEXCOORD;
};


// **************************************************************************
/* - @:エントリーポイント - */
// **************************************************************************
float4 PSMain(PS_IN input) : SV_TARGET
{
    // テクスチャ座標はそのまま頂点位置を入れる
    float4 skyTex = g_tSkyboxTexture.Sample(g_sSampler, input.TexCoord.xyz);
    
    skyTex += 0.15f;
    skyTex.r += 1.8f;
    
    return float4(skyTex.xyz, 1.0f);
}
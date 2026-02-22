// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//
/* - @:ピクセルシェーダ -*/
//
//  【?】川瀬式ブルームのためのPS
//       ダウンサンプリングしたガウスフィルタを掛けたテクスチャの平均を求める    
//
// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
#pragma once
SamplerState g_sClampSampler : register(s2);
Texture2D g_tTexture0 : register(t0); // 高輝度にブラーをかけたテクスチャ
Texture2D g_tTexture1 : register(t1); 
Texture2D g_tTexture2 : register(t2); 
Texture2D g_tTexture3 : register(t3); 


/* =========================================================================
/* - @:入力構造体 - */
/* =========================================================================*/
struct PS_IN
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR;
    float2 UV : TEXCOORD;
};


// **************************************************************************
/* - @:エントリーポイント - */
// **************************************************************************
float4 PSMain(PS_IN input) : SV_TARGET
{
    // ブラー画像をサンプリングし平均を取って出力
    float4 combineColor = g_tTexture0.Sample(g_sClampSampler, input.UV);
    combineColor  += g_tTexture1.Sample(g_sClampSampler, input.UV);
    combineColor  += g_tTexture2.Sample(g_sClampSampler, input.UV);
    combineColor += g_tTexture3.Sample(g_sClampSampler, input.UV);
    
    combineColor /= 4.0f;
    combineColor.a = 1.0f;
    
    return combineColor;
}
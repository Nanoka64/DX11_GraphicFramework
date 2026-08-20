// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//
/* - @:ピクセルシェーダ -*/
//
//  【?】Depth of Field 
//       被写界深度エフェクトのぼかし合成用シェーダ
//
// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
#pragma once
#include"ConstantBuffers_H.hlsli"
SamplerState g_sClampSampler : register(s2);
Texture2D g_tSceneBlurTexture0 : register(t0);  // メインシーンのぼかしテクスチャ
Texture2D g_tSceneTexture1 : register(t1);     // 深度テクスチャ
Texture2D g_tDepathTexture1 : register(t2);     // 深度テクスチャ

float CalcDoF(float viewDepth);
float CalcFog(float viewDepth);

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
    float4 blurColor = g_tSceneBlurTexture0.Sample(g_sClampSampler, input.UV);
    float4 sceneColor = g_tSceneTexture1.Sample(g_sClampSampler, input.UV);
    float depth = g_tDepathTexture1.Sample(g_sClampSampler, input.UV).r;
    float4 finalColor = float4(0.0, 0.0, 0.0, 1.0);
    
    // 深度が1.0f に近い背景は薄くぼかす
    //if (depth >= 0.9999)
    //{
    //    blurColor.a = 0.5f;
    //    return blurColor;
    //}
    
    // ※ 深度値はNDC空間のZ値なのでビュー空間に戻す
    float4 ndcPos = float4(0, 0, 0, 0); // NDC空間（正規化デバイス座標）
    ndcPos.xy = input.UV * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f);
    ndcPos.z = depth;
    ndcPos.w = 1.0f;
    
    // 逆行列使ってNDC空間からビュー空間に戻す
    ndcPos = mul(ndcPos, cb_ProjectionInv);
    ndcPos /= ndcPos.w; // Wで除算
    
    // ビュー空間の深度値
    float viewDepth = ndcPos.z;
    
    // cb_DoF_MinRange以上からブラーがかかり、cb_DoF_MaxRangeで最大になる
    // ぼかし開始範囲より近い場合はぼかし無し
    //clip(viewDepth - cb_DoF_MinRange);
    
    //--------------------------------
    // DoF
    //--------------------------------
    // 深度に応じてぼかしの強さを変化させる
    float blurFactor = CalcDoF(viewDepth);
    finalColor = lerp(sceneColor, blurColor, blurFactor);
    
    //--------------------------------
    // Fog
    //--------------------------------
    if (cb_Fog.End > 1.0f)
    {
        float fogFactor = CalcFog(viewDepth);
    
        finalColor.rgb =
        lerp(
            finalColor.rgb,
            cb_Fog.Color,
            fogFactor
        );
    }
    
    return finalColor;
}

//*---------------------------------------------------------------------------------------
//*【?】DOFを求める
//* [引数]
//* viewDepth : ビュー空間深度値
//* [返値]
//* float : 
//*----------------------------------------------------------------------------------------
float CalcDoF(float viewDepth)
{
    return saturate(
        (viewDepth - cb_Dof.MinRange)
        / cb_Dof.MaxRange
    );
}

//*---------------------------------------------------------------------------------------
//*【?】フォグを求める
//* [引数]
//* viewDepth : ビュー空間深度値
//* [返値]
//* float : 
//*----------------------------------------------------------------------------------------
float CalcFog(float viewDepth)
{
    float fogRange = max(cb_Fog.End - cb_Fog.Start, 0.0001f);   // 0 除算防止
    
    return saturate(
        (viewDepth - cb_Fog.Start)
        / fogRange
    );
}
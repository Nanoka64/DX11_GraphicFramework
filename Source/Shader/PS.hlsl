// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//
/* - @:ピクセルシェーダ -*/
//
//  【?】モデル用
//
// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
#pragma once
#include "LightFunctionHeader.hlsli"    // ライトヘッダー


/* =========================================================================
/* - @:PS_INPUT構造体 - */
/* =========================================================================*/
struct PS_INPUT
{
    float4 Pos      : SV_POSITION;
    float4 WPos     : POSITION;
    float3 Normal   : NORMAL0;
    float4 Col      : COLOR0;
    float2 UV       : TEXCOORD0;
};

// **************************************************************************
/* - @:エントリーポイント - */
// **************************************************************************
float4 PS(PS_INPUT input) : SV_TARGET // ピクセルシェーダの出力はSV_Targetを指定しないといけない(描画ターゲットごとに末尾変更0,1,2...)
{
    float4 diffuseColor = g_DiffuseTex.Sample(mySampler, input.UV);
    float4 normalColor = g_NormalTex.Sample(mySampler, input.UV);
    float4 specularColor = g_SpecularTex.Sample(mySampler, input.UV);
    
    float4 finalCol = float4(1.0, 1.0, 1.0, 1.0);
    finalCol = diffuseColor + DiffuseColor;
    
    /* ライティング処理 */
    /* ////////////////////////////////////////////////////////////////////////////////////////////
    *  頂点の法線とライトの向きの内積を取り、値が小さいならライトの向きと近いということであり、
    *  近いということはライトの効果が及びにくい場所なので、暗くする。
    *  計算した内積を反転したものをクランプし、カラー値にしている。
    */ ////////////////////////////////////////////////////////////////////////////////////////////
  
    // ディレクションライト計算
    float3 dirLig = DirectionLightCalc(cb_DirLightData, input.WPos.xyz, input.Normal);
    
    // ポイントライト計算
    float3 pointLig = PointLightCalc(cb_PointLightData, input.WPos.xyz, input.Normal);
    
    // 最終色
    finalCol.xyz *= dirLig + pointLig;
    
    //finalCol = c * diffuseColor;    // 頂点カラーとディフューズを乗算
    //return float4(c);               // 頂点カラー
    //return float4(t, 1, 1);         // UVカラー
    return float4(finalCol);          // テクスチャカラー
}





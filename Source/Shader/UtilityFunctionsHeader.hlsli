/* ◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇
// 
/* - @:汎用関数ヘッダー - */
//
//  【?】汎用関数をまとめたもの
// 
/* ◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇ */
#ifndef UTILITYFUNCTIONS_HLSLI
#define UTILITYFUNCTIONS_HLSLI

#include "UtilityHeader.hlsli"


//*---------------------------------------------------------------------------------------
//*【?】深度値からワールド座標に変換
//* 引数：1.深度値
//* 引数：2.UV座標
//* 返値：ワールド座標
//*----------------------------------------------------------------------------------------
float4 GetConvertPosFromDepthToWorld(float depth, float2 uv)
{
    float4 worldPos = float4(0, 0, 0, 0); // ワールド座標
    float4 ndcPos = float4(0, 0, 0, 0); // NDC空間（正規化デバイス座標）
    
    // 正規化デバイス座標系での座標を計算する。
    // z座標は深度テクスチャから引っ張ってくる。
    ndcPos.z = depth;
    
    // xy座標はUV座標から計算する。
    ndcPos.xy = uv * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f);
    ndcPos.w = 1.0f;
    
    // クリッピング座標
    float4 clipPos = ndcPos;
    
    // クリップ座標にビュープロジェクション行列の逆行列を乗算して、ワールド座標に戻す。
    worldPos = mul(clipPos, viewProjInvMatrix);
    
    // Wで除算し、3D空間に戻す
    worldPos.xyz /= worldPos.w;
    
    return worldPos;
}

#endif
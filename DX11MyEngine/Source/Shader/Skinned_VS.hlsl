// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//
/* - @:頂点シェーダ -*/
//
//  【?】モデルスキニング用
//
// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
#pragma once
#include "ConstantBuffers_H.hlsli"




//　セマンティック名こっちは自由らしい！
//  c++側のコードからもらう
/* =========================================================================
/* - @:VS_INPUT構造体 - */
/* =========================================================================*/
struct VS_INPUT
{
    float3 Pos    : POSITION;           // 頂点座標
    float2 UV     : TEXCOORD;           // テクスチャ座標
    float4 Color  : COLOR;              // 頂点色
    float3 Normal : NORMAL;             // 法線
    float3 Tan    : TANGENT;            // 接ベクトル
    float3 BiNorm : BINORMAL;           // 従ベクトル
    uint4 boneIDs : BONEIDS;            // ボーンID
    float4 boneWeights : BONEWEIGHTS;   // ボーンウェイト
};

//　セマンティック名こっちは名前が決まってる！
// 多分PSの方と同じにしないと情報がいかない
// ピクセルシェーダの入力に行ってるっぽい
/* =========================================================================
/* - @:VS_OUTPUT構造体 - */
/* =========================================================================*/
struct VS_OUTPUT
{
    float4 Pos    : SV_POSITION;    // 頂点座標(画面空間)
    //float4 WPos   : POSITION;       // ライティング用のワールド座標
    float3 Normal : NORMAL0;        // 法線
    float4 Color  : COLOR0;         // 頂点色
    float2 UV     : TEXCOORD0;      // テクスチャ座標
    float3 Tan    : TANGENT;        // 接ベクトル
    float3 BiNorm : BINORMAL;       // 従ベクトル
};

/* ---------------------------------------------------------------------------------------
/* - @:ShaderManager Class - */
/* 【?】ボーンIDに応じたデバッグカラーを返す関数 aiに頼んだ
/* ---------------------------------------------------------------------------------------*/
float4 GetBoneDebugColor(int boneID)
{
    // 最大8種類の簡易カラー
    static const float4 boneColors[33] =
    {
        float4(1.0, 0.0, 0.0, 1.0), // 0 赤
        float4(0.0, 1.0, 0.0, 1.0), // 1 緑
        float4(0.0, 0.0, 1.0, 1.0), // 2 青
        float4(1.0, 1.0, 0.0, 1.0), // 3 黄
        float4(1.0, 0.0, 1.0, 1.0), // 4 マゼンタ
        float4(0.0, 1.0, 1.0, 1.0), // 5 シアン
        float4(1.0, 0.5, 0.0, 1.0), // 6 オレンジ
        float4(0.5, 0.0, 1.0, 1.0), // 7 紫
        float4(0.0, 0.5, 1.0, 1.0), // 8 水色
        float4(0.5, 1.0, 0.0, 1.0), // 9 黄緑
        float4(1.0, 0.0, 0.5, 1.0), // 10 ピンク
        float4(0.5, 0.5, 0.5, 1.0), // 11 灰色
        float4(0.8, 0.3, 0.0, 1.0), // 12 焦げオレンジ
        float4(0.3, 0.8, 0.0, 1.0), // 13 若草色
        float4(0.0, 0.8, 0.3, 1.0), // 14 エメラルド
        float4(0.0, 0.3, 0.8, 1.0), // 15 濃青
        float4(0.3, 0.0, 0.8, 1.0), // 16 濃紫
        float4(0.8, 0.0, 0.3, 1.0), // 17 濃赤
        float4(1.0, 0.7, 0.7, 1.0), // 18 薄赤
        float4(0.7, 1.0, 0.7, 1.0), // 19 薄緑
        float4(0.7, 0.7, 1.0, 1.0), // 20 薄青
        float4(1.0, 1.0, 0.7, 1.0), // 21 薄黄
        float4(1.0, 0.7, 1.0, 1.0), // 22 薄マゼンタ
        float4(0.7, 1.0, 1.0, 1.0), // 23 薄シアン
        float4(0.5, 0.2, 0.2, 1.0), // 24 暗赤
        float4(0.2, 0.5, 0.2, 1.0), // 25 暗緑
        float4(0.2, 0.2, 0.5, 1.0), // 26 暗青
        float4(0.5, 0.5, 0.2, 1.0), // 27 暗黄
        float4(0.5, 0.2, 0.5, 1.0), // 28 暗紫
        float4(0.2, 0.5, 0.5, 1.0), // 29 暗シアン
        float4(0.9, 0.4, 0.6, 1.0), // 30 ローズ
        float4(0.6, 0.9, 0.4, 1.0), // 31 黄緑っぽい
        float4(0.4, 0.6, 0.9, 1.0), // 32 ライトブルー
    };

    // 範囲外対策（128ボーンとかある場合）
    return boneColors[boneID % 33];
}


// **************************************************************************
/* - @:エントリーポイント - */
// **************************************************************************
VS_OUTPUT VSMain(VS_INPUT input)
{
    VS_OUTPUT output;   
    float4 skinnedPos    = float4(0, 0, 0, 0); // スキニング後の頂点座標
    float3 skinnedNormal = float3(0, 0, 0); // スキニング後の法線
    float4 pos;
    float3 norm;
    
    // 4つのボーンでスキニング
    for (int i = 0; i < 4; i++)
    {
        // ボーンIDが-1以上のときだけ計算する
        if (input.boneIDs[i] >= 0)
        {
            // 頂点座標を求める
            skinnedPos += mul(cb_BoneMatrices[input.boneIDs[i]], float4(input.Pos, 1.0f)) * input.boneWeights[i];
            
            // 法線を求める
            skinnedNormal += mul((float3x3) cb_BoneMatrices[input.boneIDs[i]], input.Normal) * input.boneWeights[i];
        }
    }
    
    skinnedNormal = normalize(skinnedNormal); // 正規化
    
    /* 入力情報をそのまま出力 */
    //pos = float4(input.Pos, 1.0f);
    //norm = input.Normal;          
    
    /* スキニング結果を出力 */
    pos  = skinnedPos; // スキニング後の頂点座標
    norm = skinnedNormal; // スキニング後の法線
    
    // ワールド空間法線に変換
    norm = normalize(mul(norm, (float3x3) cb_Transform));
    
    pos = mul(pos,cb_Transform);       // ワールド変換
    //output.WPos = pos;
    pos = mul(pos, cb_View);           // ビュー変換
    pos = mul(pos, cb_Projection);     // 投影変換
    
    output.Pos    = pos;            // 画面空間の頂点座標
    output.Normal = norm;           // スキニング後の法線
    output.UV     = input.UV;       // テクスチャ座標
    output.Color  = input.Color; // カラー

    
    // 接ベクトルと従ベクトルをワールド空間に変換する
    output.Tan    = normalize(mul(input.Tan, (float3x3) cb_Transform));
    output.BiNorm = normalize(mul(input.BiNorm, (float3x3) cb_Transform));
    
    //if (input.boneIDs[0].x == 0)
    //{
    //    output.Col = float4(1.0f,0.0f,0.0f,1.0f);
    //}    
    //if (input.boneIDs[0].x == 0)
    //{
    //    output.Col = float4(1.0f,0.0f,0.0f,1.0f);   // 赤
    //}   
    
    // 主ボーンID（最も重みの高いボーン）を取得
    //int mainBone = input.boneIDs[0];
    //float maxWeight = input.boneWeights[0];
    //for (int j = 1; j < 4; j++)
    //{
    //    if (input.boneWeights[j] > maxWeight)
    //    {
    //        mainBone = input.boneIDs[j];
    //        maxWeight = input.boneWeights[j];
    //    }
    //}

    // ボーンの影響度デバッグカラー設定
    //output.Col = GetBoneDebugColor(mainBone);
    
    // 出力
    return output;
}
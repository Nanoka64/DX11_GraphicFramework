// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//
/* - @:ピクセルシェーダ -*/
//
//  【?】ディファードシェーディング
//       ライティングパス用
//
// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
#pragma once
#include "LightFunctionHeader.hlsli"

SamplerState mySampler : register(s0);

Texture2D g_AlbedoTexture : register(t0);
Texture2D g_NormalTexture : register(t1);   
Texture2D g_DepthTexture : register(t2);
Texture2D g_SpecularTexture : register(t3);    // wにスペキュラ強度

/* =========================================================================
/* - @:出力構造体 - */
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
    float4 albedoTex = g_AlbedoTexture.Sample(mySampler, input.UV);
    float4 normalTex = g_NormalTexture.Sample(mySampler, input.UV);
    float4 depthTex = g_DepthTexture.Sample(mySampler, input.UV);
    float4 specularTex = g_SpecularTexture.Sample(mySampler, input.UV);
    
    float4 finalCol = float4(1.0, 1.0, 1.0, 1.0);
    
    float depth = depthTex.r; // 深度値
    float4 worldPos;    // 深度情報からワールド座標を計算する。
    float4 ndcPos;      // NDC空間（正規化デバイス座標）

        
    if (depth >= 1.0f)
        return float4(0, 0, 0, 1);
    
    
    // 正規化デバイス座標系での座標を計算する。
    // z座標は深度テクスチャから引っ張ってくる。
    ndcPos.z = depth;

    // xy座標はUV座標から計算する。
    ndcPos.xy = input.UV * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f);
    ndcPos.w = 1.0f;
    
    // クリッピング座標
    float4 clipPos = ndcPos;
    
    // クリップ座標にビュープロジェクション行列の逆行列を乗算して、ワールド座標に戻す。
    worldPos = mul(clipPos, viewProjInvMatrix);
    
    // Wで除算し、3D空間に戻す
    worldPos.xyz /= worldPos.w;

    // 法線取り出す
    float3 normal = (normalTex.xyz - 0.5f) * 2.0f;
    normal = normalize(normal);
    //return float4(abs(normalize(worldPos.xyz)), 1.0);
    
    // スペキュラ強度
    float spcPow = specularTex.w;
    
    // ディレクションライト計算
    OUT_DiffAndSpec dirLig = DirectionLightCalc(cb_DirLightData, specularTex.rgb, spcPow, worldPos.xyz, normal);

    // ポイントライト計算
    OUT_DiffAndSpec pointLig = PointLightCalc(cb_PointLightData, specularTex.rgb, spcPow, worldPos.xyz, normal);
    
    // 天球ライト
    float3 hemiLig = HemisphereLightCalc(normal);
    
    
    // ディレクションライト + ポイントライト + 天球 + アンビエント
    //float3 lighting = dirLig + pointLig + hemiLig + 0.1f;
    float3 lighting = dirLig.Diffuse  + pointLig.Diffuse + hemiLig + 0.1f;
    float3 specular = dirLig.Specular + pointLig.Specular;
    
    // スペキュラ強度の反映
    //specular *= spcPow;
    
    // 最終色 アルベド * 光度 + スペキュラ
    finalCol.xyz = albedoTex.rgb * lighting + specular;
    finalCol.a = 1.0f;
    return finalCol;
}
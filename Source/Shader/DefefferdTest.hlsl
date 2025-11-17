
#include "LightFunctionHeader.hlsli"

SamplerState mySampler : register(s0);

Texture2D<float4> g_AlbedoTexture : register(t0);
Texture2D<float4> g_NormalTexture : register(t1);
Texture2D<float>  g_DepthTexture : register(t2);
/* =========================================================================
/* - @:出力構造体 - */
/* =========================================================================*/
struct PS_IN
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR;
    float2 UV : TEXCOORD;
};


float4 PSMain(PS_IN input) : SV_TARGET
{
    float4 albedoTex = g_AlbedoTexture.Sample(mySampler, input.UV);
    float4 normalTex = g_NormalTexture.Sample(mySampler, input.UV);
    float depthTex = g_DepthTexture.Sample(mySampler, input.UV);
    // 深度情報からワールド座標を計算する。
    float4 worldPos;

    // まず正規化スクリーン座標系での座標を計算する。
    // z座標は深度テクスチャから引っ張ってくる。
    worldPos.z = depthTex;
    // xy座標はUV座標から計算する。
    worldPos.xy = input.UV * float2(2.0f, -2.0f) - 1.0f;
    worldPos.w = 1.0f;
    // ビュープロジェクション行列の逆行列を乗算して、ワールド座標に戻す。
    worldPos = mul(Projection, worldPos);
    worldPos.xyz /= worldPos.w;
    normalTex = (normalTex * 2.0f) - 1.0f;
    
    float4 finalCol = float4(1.0, 1.0, 1.0, 1.0);
    finalCol = albedoTex;
    
    // 法線をTBN空間 ワールドスペースに変換して取得
    //float3 normal = GetNorm(normalMap, float3(1.0, 1.0, 1.0), float3(1.0, 1.0, 1.0), input.Normal);
    
    // スペキュラ色
    float3 specularCol = SpecularColor.xyz * cb_PointLightData.SpecularColor;
    
    // ディレクションライト計算
    float3 dirLig = DirectionLightCalc(cb_DirLightData, specularCol, SpecularPower, worldPos.xyz, normalTex.xyz);
    
    PointLight pl;
    pl.DiffuseColor = float3(1.0,1.0,1.0);
    pl.SpecularColor = float3(1.0, 1.0, 1.0);
    pl.Range = cb_PointLightData.Range;
    pl.Pos = float3(0.0, 0.0, 0.0);
    
    // ポイントライト計算
    float3 pointLig = PointLightCalc(pl, specularCol, SpecularPower, worldPos.xyz, normalTex.xyz);
    
    // 天球ライト
    float3 hemiLig = HemisphereLightCalc(normalTex.xyz);
    
    // ディレクションライト + ポイントライト + 天球 + アンビエント
    float3 lighting = dirLig + pointLig + hemiLig + 0.1f;

    // 最終色
    finalCol.xyz *= lighting;
    return finalCol;
}
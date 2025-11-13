
#include "LightFunctionHeader.hlsli"

SamplerState mySampler : register(s0);

Texture2D<float4> ColorMap : register(t0);
Texture2D<float4> NormalMap : register(t1);
Texture2D<float> DepthMap : register(t2);


float4 PSMain(PS_SimpleLightingInput input) : SV_TARGET
{
    float4 color = ColorMap.Sample(mySampler, input.UV);
    float4 normalMap = NormalMap.Sample(mySampler, input.UV);
    float depth = DepthMap.Sample(mySampler, input.UV).r;
    // 深度情報からワールド座標を計算する。
    float4 worldPos;

    // まず正規化スクリーン座標系での座標を計算する。
    // z座標は深度テクスチャから引っ張ってくる。
    worldPos.z = depth;
    // xy座標はUV座標から計算する。
    worldPos.xy = input.UV * float2(2.0f, -2.0f) - 1.0f;
    worldPos.w = 1.0f;
    // ビュープロジェクション行列の逆行列を乗算して、ワールド座標に戻す。
    worldPos = mul(Projection, worldPos);
    worldPos.xyz /= worldPos.w;
    normalMap = (normalMap * 2.0f) - 1.0f;
    
    
    float4 finalCol = float4(1.0, 1.0, 1.0, 1.0);
    finalCol = color * DiffuseColor;
    
    // 法線をTBN空間 ワールドスペースに変換して取得
    float3 normal = GetNorm(normalMap, float3(1.0, 1.0, 1.0), float3(1.0, 1.0, 1.0), input.Normal);
    
    // スペキュラ色
    float3 specularCol = SpecularColor.xyz * cb_PointLightData.SpecularColor;
    
    // ディレクションライト計算
    float3 dirLig = DirectionLightCalc(cb_DirLightData, specularCol, SpecularPower, input.WPos.xyz, input.Normal);
    
    // ポイントライト計算
    float3 pointLig = PointLightCalc(cb_PointLightData, specularCol, SpecularPower, input.WPos.xyz, input.Normal);
    
    // 天球ライト
    float3 hemiLig = HemisphereLightCalc(input.Normal);
    
    // ディレクションライト + ポイントライト + 天球 + アンビエント
    float3 lighting = dirLig + pointLig + hemiLig + 0.1f;

    // 最終色
    finalCol.xyz *= lighting;
    return finalCol;
}
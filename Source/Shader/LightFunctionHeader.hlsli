/* ◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇
// 
/* - @:ライトヘッダー - */
//
//  【?】ライト処理をまとめたもの
// 
/* ◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇ */
#include "UtilityHeader.hlsli"

//*---------------------------------------------------------------------------------------
//*【?】Lambert拡散反射光（ディフューズ）の計算
//* 引数：1.ライトの向き
//* 引数：2.ライトの色
//* 引数：3.頂点法線
//* 返値：float3
//*----------------------------------------------------------------------------------------
float3 DiffuseLightCalc(float3 ligDir, float3 ligCol, float3 norm)
{
    float3 finalDfs = float3(0.0f, 0.0f, 0.0f);
    
    float diffuseFactor = -dot(norm, ligDir);
    diffuseFactor = saturate(diffuseFactor); // saturate：0～1にクランプする
    
    // 拡散反射求める
    finalDfs = ligCol * diffuseFactor;
    
    return finalDfs;
}

//*---------------------------------------------------------------------------------------
//*【?】Phong鏡面反射光（スペキュラ）の計算
//* 引数：1.ライトの向き
//* 引数：2.ライトの色
//* 引数：3.反射の強さ
//* 引数：4.頂点ワールド座標
//* 引数：5.頂点法線
//* 返値：float3
//*----------------------------------------------------------------------------------------
float3 SpecularLightCalc(float3 ligDir, float3 LigCol, float spcPower, float3 pos, float3 norm)
{
    float3 finalSpc = float3(0.0f, 0.0f, 0.0f);

    // 反射ベクトル
    float3 refVec = reflect(norm, ligDir);
    
    // 光が入射したベクトルから視点に向かって伸びるベクトル
    float3 toEye = EyePos - pos;
    
    toEye = normalize(toEye); // 正規化
    float refFactor = dot(refVec, toEye); // 内積を求める
    refFactor = pow(refFactor, spcPower); // 反射の強さ求める
    
    // 鏡面反射求める
    finalSpc = LigCol * refFactor;
    
    return finalSpc;
}


//*---------------------------------------------------------------------------------------
//*【?】ディレクションライトの計算
//* 引数：1.頂点ワールド座標
//* 引数：2.頂点法線
//* 返値：float3
//*----------------------------------------------------------------------------------------
float3 DirectionLightCalc(DirectionalLight ligData, float3 pos, float3 norm)
{
    float3 finalLig = float3(0.0f, 0.0f, 0.0f);

    // 拡散（ディフューズ）反射
    float3 diffuseLig = DiffuseLightCalc(ligData.Direction, ligData.DiffuseColor, norm);
    
    // 鏡面（スペキュラ）反射
    float3 specularLig = SpecularLightCalc(ligData.Direction, ligData.SpecularColor, 2.0f, pos, norm);
    
    // 拡散反射と鏡面反射を足して最終的な光を求める
    finalLig = diffuseLig + specularLig;
    
    // アンビエント
    finalLig += 0.5f;
    
    return finalLig;
}


//*---------------------------------------------------------------------------------------
//*【?】ポイントライトの計算
//* 引数：1.ライト座標
//* 引数：2.ディフューズカラー
//* 引数：3.スペキュラカラー
//* 引数：4.影響範囲
//* 引数：5.頂点ワールド座標
//* 引数：6.頂点法線
//* 返値：float3
//*----------------------------------------------------------------------------------------
float3 PointLightCalc(PointLight ligData, float3 worldPos, float3 norm)
{
    float3 finalLig = float3(0, 0, 0);

    // 光の向きを求める
    float3 ligDir = worldPos - ligData.Pos;
    ligDir = normalize(ligDir);
    
    // ディフューズ計算
    float3 diffPoint = DiffuseLightCalc(ligDir, ligData.DiffuseColor, norm);
    
    // スペキュラ計算
    float3 spcPoint = SpecularLightCalc(ligDir, ligData.SpecularColor, 2.0f, worldPos, norm);
    
    // 頂点とライトの距離
    float distance = length(worldPos - ligData.Pos);
    
    // 影響度計算
    float influencePower = 1.0f - 1.0f / ligData.Range * distance;
    
    // マイナスにならないように
    influencePower = max(0.0, influencePower);
    influencePower = pow(influencePower, 3.0f);
    
    // 影響度を拡散・鏡面に反映させる
    diffPoint *= influencePower;
    spcPoint *= influencePower;
    
    finalLig = diffPoint + spcPoint;
    
    return finalLig;
}




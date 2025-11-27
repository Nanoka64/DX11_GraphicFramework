/* ◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇
// 
/* - @:ライトヘッダー - */
//
//  【?】ライト処理をまとめたもの
// 
/* ◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇ */
#ifndef LIGHTFUNCTIONS_HLSLI
#define LIGHTFUNCTIONS_HLSLI

#include "ConstantBuffers_H.hlsli"
    
/* ライティング処理 */
/* ////////////////////////////////////////////////////////////////////////////////////////////
*  頂点の法線とライトの向きの内積を取り、値が小さいならライトの向きと近いということであり、
*  近いということはライトの効果が及びにくい場所なので、暗くする。
*  計算した内積を反転したものをクランプし、カラー値にしている。
*/ ////////////////////////////////////////////////////////////////////////////////////////////
  

//*---------------------------------------------------------------------------------------
//*【?】正規化 Lambert拡散反射光（ディフューズ）の計算
//* 引数：1.ライトの向き
//* 引数：2.ライトの色
//* 引数：3.頂点法線
//* 返値：float3
//*----------------------------------------------------------------------------------------
float3 LambertDiffuseLightCalc(float3 _ligDir, float3 _ligCol, float3 _norm)
{
    float3 finalDfs = float3(0.0f, 0.0f, 0.0f);
    
    float diffuseFactor = saturate(dot(_norm, _ligDir));
    
    // 拡散反射求める
    finalDfs = _ligCol * diffuseFactor;
    
    /*
    *   ・正規化Lambert反射
    *   エネルギー保存の法則を守るため、反射結果をπで除算する
    */
    finalDfs /= 3.1415926f;
    
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
float3 PhongSpecularLightCalc(float3 _ligDir, float3 _eyePos, float3 _LigCol, float _spcPower, float3 _worldPos, float3 _norm)
{
    float3 finalSpc = float3(0.0f, 0.0f, 0.0f);

    // 反射ベクトル
    float3 refVec = reflect(_ligDir, _norm);
    
    // サーフェイスから視点に向かって伸びるベクトル
    float3 toEye = _eyePos.xyz - _worldPos.xyz;
    toEye = normalize(toEye);             // 正規化
    
    
    // 内積を求める（鏡面反射の強さ）
    // 一旦、物理法則的アウトらしいけど、少しだけ補正（0.2）をかける
    float refFactor = max(0.0, dot(toEye, refVec));
    
    
    // ※ ポイントライトがおかしくなっていたのはここが原因…？
    // ここを元々0.0だったところを1.0にしたら円形にはなったけど、果たしてこれでいいのだろうか？
    // 魔導書などを見ても、0.0にするように書いてあったのでモヤモヤ...
    // まあ確かに0にしたら0だからカメラと反対側は消えるけども…
    //refFactor = max(0.0, refFactor);      // マイナスにならないよう
    
    refFactor = pow(refFactor, _spcPower); // 反射の強さを絞る

    // 鏡面反射求める
    finalSpc = _LigCol * refFactor;
    
    return finalSpc;
}


//*---------------------------------------------------------------------------------------
//*【?】Phong鏡面反射光（スペキュラ）の計算
//*     ポイントライト用！！！！
//* 引数：1.ライトの向き
//* 引数：2.ライトの色
//* 引数：3.反射の強さ
//* 引数：4.頂点ワールド座標
//* 引数：5.頂点法線
//* 返値：float3
//*----------------------------------------------------------------------------------------
float3 PointLight_PhongSpecularLightCalc(float3 _ligDir, float3 _eyePos, float3 _LigCol, float _spcPower, float3 _worldPos, float3 _norm)
{
    float3 finalSpc = float3(0.0f, 0.0f, 0.0f);

    // 反射ベクトル
    float3 refVec = reflect(_ligDir, _norm);
    
    // サーフェイスから視点に向かって伸びるベクトル
    float3 toEye = _eyePos.xyz - _worldPos.xyz;
    toEye = normalize(toEye); // 正規化
    
    
    // 内積を求める（鏡面反射の強さ）
    // 絶対値にする。
    float refFactor = abs(dot(toEye, refVec));
    
    refFactor = pow(refFactor, _spcPower); // 反射の強さを絞る

    // 鏡面反射求める
    finalSpc = _LigCol * refFactor;
    
    return finalSpc;
}

//*---------------------------------------------------------------------------------------
//*【?】Blinn_Phong鏡面反射光（スペキュラ）の計算
//*     こっちはPhongの改良版みたいなやつらしい
//*     ハーフベクトルを使って計算
//* 引数：1.ライトの向き
//* 引数：2.ライトの色
//* 引数：3.反射の強さ
//* 引数：4.頂点ワールド座標
//* 引数：5.頂点法線
//* 返値：float3
//*----------------------------------------------------------------------------------------
float3 Blinn_PhongSpecularLightCalc(float3 _ligDir, float3 _eyePos, float3 _LigCol, float _spcPower, float3 _worldPos, float3 _norm)
{
    float3 finalSpc = float3(0.0f, 0.0f, 0.0f);
    
    // サーフェイスから視点に向かって伸びるベクトル
    float3 toEye = _eyePos.xyz - _worldPos.xyz;
    toEye = normalize(toEye);               // 正規化

    float3 halfVec = normalize(_ligDir + toEye);     // ライトの向きと視線でハーフベクトルを求める
    float refFactor = saturate(dot(_norm, halfVec)); // 内積を求める（鏡面反射の強さ）
    
    refFactor = pow(refFactor, _spcPower); // 反射の強さを絞る

    // 鏡面反射求める
    finalSpc = _LigCol * refFactor;
    
    return finalSpc;
}


struct OUT_DiffAndSpec
{
    float3 Diffuse;
    float3 Specular;
};

//*---------------------------------------------------------------------------------------
//*【?】ディレクションライトの計算
//* 引数：1.ライト情報
//* 引数：2.カメラ座標
//* 引数：3.スペキュラカラー
//* 引数：4.スペキュラ強度
//* 引数：5.サーフェイスのワールド座標
//* 引数：6.サーフェイスのワールド法線
//* 返値：float3
//*----------------------------------------------------------------------------------------
OUT_DiffAndSpec DirectionLightCalc(DirectionalLight _ligData, float3 _eyePos, float3 _spcCol, float _spcPow, float3 _worldPos, float3 _norm)
{
    float3 finalLig = float3(0.0f, 0.0f, 0.0f);
    
    float3 ligDir = normalize(_ligData.Direction);
    
    // 拡散（ディフューズ）反射
    float3 diffuseLig = LambertDiffuseLightCalc(ligDir, _ligData.DiffuseColor, _norm);
    
    // 鏡面（スペキュラ）反射
    float3 specularLig = PhongSpecularLightCalc(ligDir, _eyePos, _spcCol, _spcPow, _worldPos, _norm);
    
    OUT_DiffAndSpec outData;
    outData.Diffuse = diffuseLig * _ligData.Intensity;
    outData.Specular = specularLig * _ligData.Intensity;
    return outData;
}

//*---------------------------------------------------------------------------------------
//*【?】ポイントライトの計算
//* 引数：1.ライト座標
//* 引数：2.カメラ座標
//* 引数：3.ディフューズカラー
//* 引数：4.スペキュラカラー
//* 引数：5.影響範囲
//* 引数：6.サーフェイスのワールド座標
//* 引数：7.サーフェイスのワールド法線
//* 返値：float3
//*----------------------------------------------------------------------------------------
OUT_DiffAndSpec PointLightCalc(PointLight _ligData, float3 _eyePos, float3 _spcCol, float _spcPow, float3 _worldPos, float3 _norm)
{
    float3 finalLig = float3(0, 0, 0);

    float3 ligDir = _worldPos -_ligData.Pos;    // 光の向きを求める（ライト→サーフェイス）
    float distance = length(ligDir);            // サーフェイス→ライトの距離
    ligDir = normalize(ligDir);                 // 正規化
    
    // ディフューズ計算
    float3 diffPoint = LambertDiffuseLightCalc(ligDir, _ligData.DiffuseColor, _norm);
    
    // フォン反射
    float3 spcPoint = PhongSpecularLightCalc(ligDir, _eyePos, _spcCol, _spcPow, _worldPos, _norm);
    
    // 影響度計算
    float affect = 1.0f - min(1.0f, distance / _ligData.Range);
    
    // マイナスにならないように
    affect = max(0.0, affect);
    affect = pow(affect, 3.0f);
    
    // 影響度を拡散・鏡面に反映させる
    diffPoint *= affect;
    spcPoint  *= affect;
    
    OUT_DiffAndSpec outData;
    outData.Diffuse = diffPoint;
    outData.Specular = spcPoint * _spcCol;
    return outData;
}

//*---------------------------------------------------------------------------------------
//*【?】半球ライト計算
//* 引数：1.頂点法線
//* 返値：float3
//*----------------------------------------------------------------------------------------
float3 HemisphereLightCalc(float3 _norm)
{
    float3 finalLig     = float3(0, 0, 0);
    float3 groundNorm   = float3(0.0f, 1.0f, 0.0f);
    float3 skyColor     = float3(0.35f, 0.6f, 0.85f);
    float3 groundColor  = float3(0.5f, 0.5f, 0.5f);
    
    float factor = dot(_norm, groundNorm);
    
    // 0～1に変換
    factor = (factor + 1.0f) * 0.5f;
    
    // 地面色と天球色を補間
    finalLig = lerp(groundColor, skyColor, factor);
    
    return finalLig;
}



//*---------------------------------------------------------------------------------------
//*【?】法線情報取得
//*     ノーマルマップの凹凸情報は接空間（TBN空間）として入っているので、
//*     ライティング計算時にはワールド空間に変換しなければならない。
//*     まあテクスチャ情報だからワールド空間として入ってるわけないよねっていう
//*     
//*     ・接ベクトルがテクスチャの U 座標
//*     ・従ベクトルがテクスチャの V 座標
//*
//* 引数：1.法線マップ
//* 引数：2.接ベクトル
//* 引数：3.従ベクトル
//* 引数：4.サーフェイスのローカル法線
//* 返値：float3
//*----------------------------------------------------------------------------------------
float3 GetNorm(float4 _normMap, float3 _tan, float3 _biNorm, float3 _localNorm)
{
    // 今のままだと0～1の値になっているので、-1～1に変換
    _normMap = (_normMap - 0.5f) * 2.0f;
    
    // タンジェントスペースの法線をワールドスペースに変換
    return normalize(
             _tan       * _normMap.x +
             _biNorm    * _normMap.y +
             _localNorm * _normMap.z);
};
#endif
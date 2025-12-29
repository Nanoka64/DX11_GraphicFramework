#pragma once
#include "IComponent.h"

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:Light Class --- */
//
//  ★継承：IComponet ★
//
// 【?】各ライトの基底クラス
//
// ***************************************************************************************
class Light : public IComponent
{
protected:
    std::weak_ptr<class Transform> m_pOwnerTransform;   // オーナーの方向を保持（ライトの方向に使うため）
    VECTOR3::VEC3 m_LightColor;                         // ライトカラー
    float m_Intensity;   // ディフューズ強度
    float m_SpecularIntensity;  // スペキュラ強度

    VECTOR3::VEC3 m_FocusPoint;
    VECTOR3::VEC3 m_UpVec;


public:
    Light(std::weak_ptr<GameObject> pOwner, int updateRank);
    virtual ~Light() = default;

    /// <summary>
    /// ライトカラーの設定
    /// </summary>
    /// <param name="col"></param>
    void set_LightColor(const VECTOR3::VEC3& col);


    /// <summary>
    /// 光の強さ設定
    /// </summary>
    /// <param name="ins"></param>
    void set_Intensity(float ins);
};


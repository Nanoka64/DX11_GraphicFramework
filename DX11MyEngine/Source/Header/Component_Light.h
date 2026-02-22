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
    std::weak_ptr<class MyTransform> m_pOwnerTransform;   // オーナーの方向を保持（ライトの方向に使うため）
    VECTOR3::VEC3 m_LightColor;                         // ライトカラー
    float m_Intensity;   // 強度

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
    /// ディフューズ光の強さ設定
    /// </summary>
    /// <param name="ins"></param>
    void set_Intensity(float ins);

    float get_Intensity() const {
        return m_Intensity;
    }

    VECTOR3::VEC3 get_LightColor() const {
        return m_LightColor;
    }
};


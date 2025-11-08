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
    std::weak_ptr<class Transform> m_pCameraTransform;  // カメラの座標取得のために保持
    std::weak_ptr<class Transform> m_pOwnerTransform;   // オーナーの方向を保持（ライトの方向に使うため）
    VECTOR3::VEC3 m_LightColor;                         // ライトカラー

public:
    Light(std::weak_ptr<GameObject> pOwner, int updateRank);
    virtual ~Light() = default;

    /// <summary>
    /// 定数バッファ作成
    /// </summary>
    /// <param name="pDevice"></param>
    /// <returns></returns>
    virtual bool CreateCBuffer(ID3D11Device *pDevice) = 0;


    /// <summary>
    /// カメラのトランスフォームセット
    /// </summary>
    /// <param name="pCam"></param>
    void set_CameraTransform(std::weak_ptr<class Transform> pCamTransform);


    /// <summary>
    /// ライトカラーの設定
    /// </summary>
    /// <param name="col"></param>
    void set_LightColor(const VECTOR3::VEC3& col);
};


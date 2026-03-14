#pragma once
#ifndef TRANSFORM_H
#define TRANSFORM_H
#include "IComponent.h"


// ************************************************************************************************************
// ------------------------------------------------------------------------------------------------------------
/* --- @:Transform Class --- */
//
//  ★継承：IComponent ★
//
// 【?】位置、回転、拡縮を管理するコンポーネント
// 
//      値は自作ベクトル構造体としては持っておらず、XMVECTORとして値を保持している   
//      メンバ変数はprivateとし、派生先で直接値を変更することを禁止とする（あくまで値の保持のみに使う）
//      値を変更する際は「set関数」を使用して変更すること.
//      ※ UnityのTransformに近い感じ
// 
//      ※ 回転値（Rotation ）はラジアンとして扱っている
//      なのでset/getでラジアンとデグリーで選ぶことができるが、
//      ラジアン指定のほうが変換処理が要らないため、効率がいい
//
// *************************************************************************************************************
class MyTransform : public IComponent
{
protected:
	DirectX::XMVECTOR m_Position;	// 位置
    DirectX::XMVECTOR m_RotationQ;	// 回転 クオータニオンで保持するようにしたが、set/getではオイラーで指定できるようにする
	DirectX::XMVECTOR m_Scale;		// 拡縮
    
    DirectX::XMVECTOR m_Local_Position;	    // ローカル空間オフセット位置
    DirectX::XMVECTOR m_Local_RotationQ;	// ローカル空間オフセット回転 ラジアンで保持
    DirectX::XMVECTOR m_Local_Scale;		// ローカル空間オフセット拡縮

    // オイラーとしても保持
    // クォータニオンからオイラー角を算出するのはコストがかかるため
    // 単位はラジアンで保持するが、set関数ではラジアンとデグリーの両方で指定できるようにする
    VECTOR3::VEC3 m_EulerAngles;
    VECTOR3::VEC3 m_Local_EulerAngles;


    bool m_isDirty; // 座標更新が行われたか

	std::weak_ptr<MyTransform> m_pParent;	        // 親オブジェクト

    void Start(RendererEngine &renderer) override {};	// 初期化
    void Update(RendererEngine &renderer) override {}; 	// 更新処理
    void Draw(RendererEngine &renderer)override {};		// 描画処理

public:
	MyTransform(std::weak_ptr<class GameObject> pOwner, int updateRank = 100);
	virtual~MyTransform();

    /// <summary>
    /// 目標方向へ向かせる
    /// </summary>
    /// <param name="target"></param>
    void LookAt(const VECTOR3::VEC3 &_targetPos);


    /// <summary>
    /// 球面線形補間を使用した目標方向への回転
    /// </summary>
    /// <param name="target"></param>
    /// <param name="t"></param>
    void SlerpLookAt(const VECTOR3::VEC3 &_targetPos, float _t);


    /****** セッタ ******/
    void set_Pos(float x, float y, float z);            // 位置設定 float 3要素
    void set_Pos(const VECTOR3::VEC3& vIn);             // 位置設定 - ベクトル指定
    void set_RotateToRad(float x, float y, float z);    // 回転設定 float 3要素 ラジアン
    void set_RotateToDeg(float x, float y, float z);    // 回転設定 float 3要素 デグリー
    void set_RotateToRad(const VECTOR3::VEC3& vIn);     // 回転設定 - ベクトル指定 ラジアン
    void set_RotateToDeg(const VECTOR3::VEC3& vIn);     // 回転設定 - ベクトル指定 デグリー
    void set_RotationQuaternion(const DirectX::XMVECTOR& _q);   // 回転設定 - クォータニオン
    void set_Scale(float x, float y, float z);          // 拡大設定 float 3要素
    void set_Scale(const VECTOR3::VEC3& vIn);           // 拡大設定 - ベクトル指定

    void set_VEC3ToLocal_Pos(const VECTOR3::VEC3 &vIn);        // ローカル空間オフセット位置設定 - ベクトル指定   
    void set_VEC3ToLocal_RotateToRad(const VECTOR3::VEC3 &vIn);// ローカル空間オフセット回転設定 - ベクトル指定 ラジアン
    void set_VEC3ToLocal_RotateToDeg(const VECTOR3::VEC3 &vIn);// ローカル空間オフセット回転設定 - ベクトル指定 デグリー
    void set_VEC3ToLocal_Scale(const VECTOR3::VEC3 &vIn);      // ローカル空間オフセット拡大設定 - ベクトル指定

    void set_Parent(std::weak_ptr<MyTransform> parent) { m_pParent = parent; }  // 親設定

    /****** ゲッタ ******/
    DirectX::XMVECTOR get_XMVecToPos() const;            // 位置取得 - XMVECTOR
    DirectX::XMVECTOR get_XMVecToRotateToRad() const;    // 回転取得 - XMVECTOR ラジアン 
    DirectX::XMVECTOR get_XMVecToRotateToDeg() const;    // 回転取得 - XMVECTOR デグリー 
    DirectX::XMVECTOR get_RotationQuaternion() const;    // 回転取得 - クオータニオン
    DirectX::XMVECTOR get_XMVecToScale() const;          // 拡大取得 - XMVECTOR   
    const VECTOR3::VEC3 get_VEC3ToPos() const;          // 位置取得 - VEC3
    const VECTOR3::VEC3 get_VEC3ToRotateToRad() const;  // 回転取得 - VEC3  ラジアン
    const VECTOR3::VEC3 get_VEC3ToRotateToDeg() const;  // 回転取得 - VEC3  デグリー
    const VECTOR3::VEC3 get_VEC3ToScale() const;        // 拡大取得 - VEC3

    const VECTOR3::VEC3 get_VEC3ToLocal_Pos()const;         // ローカル空間オフセット位置取得 - VEC3 
    const VECTOR3::VEC3 get_VEC3ToLocal_RotateToRad()const; // ローカル空間オフセット回転取得 - VEC3  ラジアン
    const VECTOR3::VEC3 get_VEC3ToLocal_RotateToDeg()const; // ローカル空間オフセット回転取得 - VEC3  デグリー
    const VECTOR3::VEC3 get_VEC3ToLocal_Scale()const;       // ローカル空間オフセット拡大取得 - VEC3

    const VECTOR3::VEC3 get_WorldVEC3ToPos()const;  // ワールド座標の取得

    DirectX::XMMATRIX get_MtxPos()const;    // 位置行列取得
    DirectX::XMMATRIX get_MtxRotate()const; // 回転行列取得
    DirectX::XMMATRIX get_MtxScale()const;  // 拡大行列取得

    
    virtual DirectX::XMMATRIX get_WorldMtx()const;              // ワールド行列取得
    virtual DirectX::XMMATRIX get_ExcludingRotWorldMtx()const;  // 回転を除くワールド行列取得

    /// <summary>
    /// パラメータを個別に設定するver
    /// </summary>
    /// <param name="scl"></param>
    /// <param name="rot"></param>
    /// <param name="trans"></param>
    /// <returns></returns>
    DirectX::XMMATRIX get_WorldMtx(const DirectX::XMMATRIX &scl, const DirectX::XMMATRIX &rot, const DirectX::XMMATRIX &trans)const;
    DirectX::XMMATRIX get_ExcludingRotWorldMtx(const DirectX::XMMATRIX &scl, const DirectX::XMMATRIX &trans)const;

    std::weak_ptr<MyTransform> get_Parent()const;

    const VECTOR3::VEC3 get_Forward()const;
    VECTOR3::VEC3 get_Up()const;
    VECTOR3::VEC3 get_Right()const;

    static const DirectX::XMVECTOR FORWARD;
    static const DirectX::XMVECTOR UP;
    static const DirectX::XMVECTOR RIGHT;

    
    /// <summary>
    /// RectTransformかどうかの確認
    /// RectTransformはTransformを継承しているので、安全に型チェックを行うために作成
    /// </summary>
    /// <returns></returns>
    virtual bool get_IsRectTransform()const { return false; }


    void Release(); // 解放
};

#endif
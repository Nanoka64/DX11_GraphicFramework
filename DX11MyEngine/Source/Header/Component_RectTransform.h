#pragma once
#include "Component_Transform.h"

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:RectTransform Class --- */
//
//  ★継承：IComponent ★
//
// 【?】2D描画用のトランスフォーム
//	参考サイト：https://qiita.com/maple_22/items/4e81acec92ab44b31910
//				https://taidanahibi.com/unity/rect-transform/#index_id0
//		
// ***************************************************************************************
class RectTransform : public MyTransform
{
private:
	VECTOR2::VEC2 m_AnchoredPosition;	// アンカーからの相対座標
	VECTOR2::VEC2 m_SizeDelta;			// m_AnchorMin / m_AnchorMaxと自身の幅・高さの差

	// 親に対しての子の基準点 0.0～1.0の座標
	// 左下が（0.0,0.0）
	// 右上が（1.0,1.0）
	VECTOR2::VEC2 m_AnchorMin;	
	VECTOR2::VEC2 m_AnchorMax;	 

	VECTOR2::VEC2 m_Pivot;		// ピボット（0.5,0.5で中心）

	VECTOR3::VEC3 m_Rotation;
public:
	RectTransform(std::weak_ptr<GameObject> pOwner, int updateRank = 100);
	~RectTransform();

	//*----------------------------------------------------------------------------------------
	//*【?】セッタ
	//*----------------------------------------------------------------------------------------
	void set_AnchorMin(const VECTOR2::VEC2 &_v) { m_AnchorMin = _v; }
	void set_AnchorMax(const VECTOR2::VEC2 &_v) { m_AnchorMax = _v; }
	void set_Pivot(const VECTOR2::VEC2 &_v) { m_Pivot = _v; }
	void set_Rotation(const VECTOR3::VEC3 &_v) { m_Rotation = _v; }

	//*----------------------------------------------------------------------------------------
	//*【?】ゲッタ
	//*----------------------------------------------------------------------------------------
	VECTOR2::VEC2 get_AnchorMin()const { return m_AnchorMin; }
	VECTOR2::VEC2 get_AnchorMax()const { return m_AnchorMax; }
	VECTOR2::VEC2 get_Pivot()const { return m_Pivot; }
	VECTOR3::VEC3 get_Rotation()const { return m_Rotation; }

	/// <summary>
	/// ワールド変換行列の取得
	/// </summary>
	/// <returns>ワールド変換行列</returns>
	DirectX::XMMATRIX get_WorldMtx()const override;


	/// <summary>
	/// RectTransformかどうか
	/// </summary>
	/// <returns></returns>
	bool get_IsRectTransform()const override { return true; };
};


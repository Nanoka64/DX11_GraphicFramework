#pragma once
#include "IComponent.h"

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:RectTransform Class --- */
//
//  ★継承：IComponent ★
//
// 【?】2D描画用のトランスフォーム
//	参考サイト：https://qiita.com/maple_22/items/4e81acec92ab44b31910
//		
// ***************************************************************************************
class RectTransform : public IComponent
{
private:
	VECTOR2::VEC2 m_AnchoredPosition;	// アンカーからの相対距離
	VECTOR2::VEC2 m_SizeDelta;			// 幅と高さ

	VECTOR2::VEC2 m_AnchorMin;
	VECTOR2::VEC2 m_AnchorMax;
	VECTOR2::VEC2 m_Pivot;		// ピボット（0.5,0.5で中心）

	VECTOR3::VEC3 m_Rotation;
public:
	RectTransform(std::weak_ptr<GameObject> pOwner, int updateRank = 100);
	~RectTransform();
};


#pragma once
#include "IComponent.h"

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:Render Class --- */
//
//  ★継承：IComponent ★
//
// 【?】描画を行うコンポーネントの基底クラス
//		
// ***************************************************************************************
class Render : public IComponent
{
private:
	DirectX::BoundingBox m_FrustumBoundingBox;	// フラスタム用のバウンディングボックス

public:
	Render(std::weak_ptr<GameObject> pOwner, int updateRank);
	virtual ~Render() = default;

	/// <summary>
	/// フラスタム用のBBを設定
	/// </summary>
	/// <param name="_aabb">AABBデータ</param>
	void set_FrustumBoundingBox(const DirectX::BoundingBox& _boundingBox) { m_FrustumBoundingBox = _boundingBox; }

	/// <summary>
	/// フラスタム用のBBを取得
	/// </summary>
	/// <returns></returns>
	const DirectX::BoundingBox& get_FrustumBoundingBox() const { return m_FrustumBoundingBox; }
};


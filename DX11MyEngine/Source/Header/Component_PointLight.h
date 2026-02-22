#pragma once
#include "Component_Light.h"


// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:PointLight Class --- */
//
//  ★継承：Light ← Component ★
//
// 【?】ポイントライト
//
// ***************************************************************************************
class PointLight : public Light
{
private:
	float m_Range;	// 影響範囲

public:
	PointLight(std::weak_ptr<GameObject> pOwner, int updateRank);
	~PointLight();


	void Start(RendererEngine &renderer) override;	// 初期化
	void Update(RendererEngine &renderer) override;// 更新
	void Draw(RendererEngine &renderer) override;	// 描画

	/// <summary>
	/// 影響範囲セット
	/// </summary>
	/// <param name="r"></param>
	void set_Range(float r) { m_Range = r; }

    float get_Range() const { return m_Range; }
};


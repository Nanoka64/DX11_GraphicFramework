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
	float m_FlashElapsed;		// フラッシュ経過時間
	float m_FlashDuration;		// フラッシュ時間
	float m_FlashMaxRange;		// フラッシュ最大光度
	float m_FlashMaxIntensity;	// フラッシュ最大範囲
	bool m_IsFlash;

public:
	PointLight(std::weak_ptr<GameObject> pOwner, int updateRank);
	~PointLight();


	void Start(RendererEngine &renderer) override;	// 初期化
	void Update(RendererEngine &renderer) override;// 更新

	/// <summary>
	/// 影響範囲セット
	/// </summary>
	/// <param name="r"></param>
	void set_Range(float r) { m_Range = r; }

	/// <summary>
	/// フラッシュ
	/// </summary>
	/// <param name="_duration">フラッシュ時間</param>
	/// <param name="_maxIntensity">最大光度</param>
	/// <param name="_maxRange">最大範囲</param>
	void Flash(float _duration, float _maxIntensity, float _maxRange);

    float get_Range() const { return m_Range; }
};


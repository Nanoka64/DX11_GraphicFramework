#pragma once
#include "Component_Light.h"

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:SpotLight Class --- */
//
//  ★継承：Light ← Component ★
//
// 【?】スポットライト
//
// ***************************************************************************************
class SpotLight :  public Light
{
private:
	float m_Range;				// 影響範囲
	float m_Angle;				// 放射角度
	VECTOR3::VEC3 m_Direction;	// 放射方向

public:
	SpotLight(std::weak_ptr<GameObject> pOwner, int updateRank);
	~SpotLight();

	void Start(RendererEngine& renderer) override;	// 初期化
	void Update(RendererEngine& renderer) override; // 更新

	void set_SpotLightData(float _range, float _angleDeg);
	float get_Range()const { return m_Range; }
	float get_Angle()const { return m_Angle; }
	float get_AngleDeg()const { return Tool::RadToDeg(m_Angle); }
};


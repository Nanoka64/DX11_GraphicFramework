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
	CB_POINT_LIGHT_SET *m_pCBLightSet;
	float m_Range;	// 影響範囲

public:
	PointLight(std::weak_ptr<GameObject> pOwner, int updateRank);
	~PointLight();


	bool CreateCBuffer(ID3D11Device *pDevice)override;

	void Init(RendererManager &renderer) override;	// 初期化
	void Update(RendererManager &renderer) override;// 更新
	void Draw(RendererManager &renderer) override;	// 描画

	/// <summary>
	/// 影響範囲セット
	/// </summary>
	/// <param name="r"></param>
	void set_Range(float r) { m_Range = r; }
};


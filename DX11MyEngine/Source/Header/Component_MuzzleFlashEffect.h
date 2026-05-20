#pragma once
#include "IComponent.h"


// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:MuzzleFlash Class --- */
//
//  ★継承：IComponent ★
//
// 【?】マズルフラッシュエフェクト
//		
// ***************************************************************************************
class MuzzleFlashEffect : public IComponent
{
private:
	std::weak_ptr<class PointLight> m_pFlashLight;	// フラッシュ用ポイントライト

public:
	MuzzleFlashEffect(std::weak_ptr<GameObject> pOwner, int updateRank);
	~MuzzleFlashEffect();
	void Start(RendererEngine& renderer) override;	// 初期化
	void Update(RendererEngine& renderer) override;	// 更新
	void Draw(RendererEngine& renderer) override;	// 描画
};

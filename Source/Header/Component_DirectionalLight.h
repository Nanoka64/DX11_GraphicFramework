#pragma once
#include "Component_Light.h"

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:DirectionalLight Class --- */
//
//  ★継承：Light ← Component ★
//
// 【?】ディレクションライト
//
// ***************************************************************************************
class DirectionalLight : public Light
{
private:

public:
	DirectionalLight(std::weak_ptr<GameObject> pOwner, int updateRank);
	~DirectionalLight();

	void Init(RendererEngine &renderer) override;	// 初期化
	void Update(RendererEngine &renderer) override;// 更新
	void Draw(RendererEngine &renderer) override;	// 描画

};


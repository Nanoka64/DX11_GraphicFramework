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
	CB_DIRECTION_LIGHT_SET *m_pCBLightSet;

public:
	DirectionalLight(std::weak_ptr<GameObject> pOwner, int updateRank);
	~DirectionalLight();

	bool CreateCBuffer(ID3D11Device *pDevice)override;

	void Init(RendererManager &renderer) override;	// 初期化
	void Update(RendererManager &renderer) override;// 更新
	void Draw(RendererManager &renderer) override;	// 描画

};


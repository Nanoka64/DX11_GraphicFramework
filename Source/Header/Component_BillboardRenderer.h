#pragma once
#include "IComponent.h"


// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:BillboardRenderer Class --- */
//
//  ★継承：IComponent ★
//
// 【?】ビルボードの描画を行うコンポーネント
//
// ***************************************************************************************
class BillboardRenderer : public IComponent
{
private:
	std::weak_ptr<class BillboardResource>m_pResource;	// ビルボードリソースの参照	

public:
	BillboardRenderer(std::weak_ptr<class GameObject> pOwner, int updateRank = 100);
	~BillboardRenderer();

	void Init(RendererEngine& renderer)override;
	void Update(RendererEngine& renderer)override;
	void Draw(RendererEngine& renderer)override;

	void set_BillboardResource(std::weak_ptr<class BillboardResource> billboardResource);
};


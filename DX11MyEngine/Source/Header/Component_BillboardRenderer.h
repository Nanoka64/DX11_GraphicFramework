#pragma once
#include "IComponent.h"
#include "Component_Render.h"


// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:BillboardRenderer Class --- */
//
//  ★継承：IComponent ★
//
// 【?】ビルボードの描画を行うコンポーネント
//
// ***************************************************************************************
class BillboardRenderer : public Render
{
private:
	std::weak_ptr<class BillboardResource>m_pResource;	// ビルボードリソースの参照	
	VECTOR4::VEC4 m_ColorFactor;
public:
	BillboardRenderer(std::weak_ptr<class GameObject> pOwner, int updateRank = 100);
	~BillboardRenderer();

	void Start(RendererEngine& renderer)override;
	void Update(RendererEngine& renderer)override;
	void Draw(RendererEngine& renderer)override;
	bool IsVisible(const DirectX::BoundingFrustum& _frustum) const override;

	void set_ColorFactor(const VECTOR4::VEC4& _col) { m_ColorFactor = _col; }
	void set_BillboardResource(std::weak_ptr<class BillboardResource> billboardResource);
};


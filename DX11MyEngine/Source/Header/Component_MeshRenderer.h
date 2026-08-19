#pragma once
#include "IComponent.h"
#include "Component_Render.h"

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:MeshRenderer Class --- */
//
//  ★継承：IComponent ★
//
// 【?】IMeshResourceから受け取った情報から、
//		メッシュの描画をする
//		
// ***************************************************************************************
class MeshRenderer :public Render
{
private:
	std::weak_ptr<class IMeshResource> m_pMeshResource;	// メッシュ情報の参照

public:
	MeshRenderer(std::weak_ptr<GameObject> pOwner, int updateRank = 100);
	~MeshRenderer();

	void Start(RendererEngine& renderer) override;		// 初期化
	void Update(RendererEngine& renderer) override;	// 更新処理
	void Draw(RendererEngine& renderer) override;		// 描画処理
	bool IsVisible(const DirectX::BoundingFrustum& _frustum) const override;
	
	void set_MeshResource(std::weak_ptr<class IMeshResource> meshResource);
};


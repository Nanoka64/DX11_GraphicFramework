#pragma once
#include "IComponent.h"

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
class MeshRenderer :public IComponent
{
private:
	std::weak_ptr<class IMeshResource> m_pMeshResource;	// メッシュ情報の参照


public:
	MeshRenderer(std::weak_ptr<GameObject> pOwner, int updateRank = 100);
	~MeshRenderer();

	void Init(RendererManager& renderer) override;		// 初期化
	void Update(RendererManager& renderer) override;	// 更新処理
	void Draw(RendererManager& renderer) override;		// 描画処理
	void set_MeshResource(std::weak_ptr<class IMeshResource> meshResource);
};


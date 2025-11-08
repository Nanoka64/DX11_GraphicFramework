#pragma once
#include "IComponent.h"


class ModelMeshRenderer : public IComponent
{
private:
	std::weak_ptr<class ModelMeshResource> m_pMeshResource;	// メッシュ情報の参照
	bool m_IsDrawLine;

public:
	ModelMeshRenderer(std::weak_ptr<GameObject> pOwner, int updateRank = 100);
	~ModelMeshRenderer();

	void Init(RendererManager &renderer) override;		// 初期化
	void Update(RendererManager &renderer) override;	// 更新処理
	void Draw(RendererManager &renderer) override;		// 描画処理
	void set_MeshResource(std::weak_ptr<class ModelMeshResource> meshResource);
};


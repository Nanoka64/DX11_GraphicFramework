#pragma once
#include "IComponent.h"


class ModelMeshRenderer : public IComponent
{
private:
	std::weak_ptr<class ModelMeshResource> m_pMeshResource;	// メッシュ情報の参照
	bool m_IsDrawWireframe;

	int m_DebugDrawBoneNum;

public:
	ModelMeshRenderer(std::weak_ptr<GameObject> pOwner, int updateRank = 100);
	~ModelMeshRenderer();

	void Start(RendererEngine &renderer) override;		// 初期化
	void Update(RendererEngine &renderer) override;	// 更新処理
	void Draw(RendererEngine &renderer) override;		// 描画処理
	void set_MeshResource(std::shared_ptr<class ModelMeshResource> meshResource);

	/* ワイヤーフレームで表示するかどうか */
	bool get_IsDrawWireframe()const { return m_IsDrawWireframe; };
	void set_IsDrawWireframe(bool _flag) { m_IsDrawWireframe = _flag; }



};


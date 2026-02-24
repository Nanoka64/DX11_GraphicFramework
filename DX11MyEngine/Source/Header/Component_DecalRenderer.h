#pragma once
#include "IComponent.h"

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:DecalRenderer Class --- */
//
//  ★継承：IComponent ★
//
// 【?】デカールの描画を行う
//		
// ***************************************************************************************
class DecalRenderer : public IComponent
{
private:
	std::weak_ptr<class IMeshResource> m_pMeshResource;	// メッシュ情報の参照
	CB_DECAL_SET *m_pCBDecalSet;						// デカール用定数バッファ
	bool m_IsDynamic;									// 動的に変更されるか
public:
	DecalRenderer(std::weak_ptr<GameObject> pOwner, int updateRank = 100);
	~DecalRenderer();

	void Start(RendererEngine &renderer) override;		// 初期化
	void Update(RendererEngine &renderer) override;		// 更新処理
	void Draw(RendererEngine &renderer)override;		// 描画処理

	bool CreateDecalConstantBuffer(RendererEngine &renderer);	// デカール用定数バッファの作成

	void set_MeshResource(std::weak_ptr<class IMeshResource> meshResource);
	void set_IsDynamic(bool _flag) { m_IsDynamic = _flag; }		// 動的変更を行うか
};


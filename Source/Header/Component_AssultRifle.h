#pragma once
#include "IComponent.h"
class AssultRifle : public IComponent
{
private:
	std::shared_ptr<GameObject> m_pBulletObject;	// 弾となるオブジェクト

public:
	AssultRifle(std::weak_ptr<GameObject> pOwner, int updateRank = 100);
	~AssultRifle();

	void Start(RendererEngine &renderer) override;		// 初期化
	void Update(RendererEngine &renderer) override;		// 更新処理
	void Draw(RendererEngine &renderer)override;		// 描画処理

	void set_BulletObject(std::shared_ptr<GameObject> pObj);
};


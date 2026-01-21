#pragma once
#include "IComponent.h"
class Bullet : public IComponent
{
private:
	std::shared_ptr<GameObject> m_pBulletObject;	// 弾となるオブジェクト
	VECTOR3::VEC3 m_StartPos;
	VECTOR3::VEC3 m_MoveVelocity;

public:
	Bullet(std::weak_ptr<GameObject> pOwner, int updateRank = 100);
	~Bullet();

	void Start(RendererEngine &renderer) override;		// 初期化
	void Update(RendererEngine &renderer) override;		// 更新処理
	void Draw(RendererEngine &renderer)override;		// 描画処理

	void set_BulletObject(std::shared_ptr<GameObject> pObj);
};


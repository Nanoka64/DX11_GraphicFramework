#pragma once
#include "IComponent.h"
class PlayerController : public IComponent
{
private:
	
	std::shared_ptr<class Camera3D> m_pCameraComp;
	bool m_IsAnim;
public:
	PlayerController(std::weak_ptr<GameObject> pOwner, int updateRank = 100);
	~PlayerController();

	void Init(RendererEngine& renderer) override;		// ‰Šú‰»
	void Update(RendererEngine& renderer) override;	// XVˆ—
	void Draw(RendererEngine& renderer)override;		// •`‰æˆ—
};


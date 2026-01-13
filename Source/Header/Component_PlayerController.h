#pragma once
#include "IComponent.h"
#include "RootSceneState.h"
#include "StateMachine.h"

class PlayerController : public IComponent
{
private:
    StateMachine<PlayerController> m_StateMachine;
	std::shared_ptr<class Camera3D> m_pCameraComp;
    bool m_IsAnim;		// アニメーション中かどうか
    float m_MoveSpeed;	// 移動速度



public:
	PlayerController(std::weak_ptr<GameObject> pOwner, int updateRank = 100);
	~PlayerController();

	void Init(RendererEngine& renderer) override;		// 初期化
	void Update(RendererEngine& renderer) override;	// 更新処理
	void Draw(RendererEngine& renderer)override;		// 描画処理


    float get_MoveSpeed() const { return m_MoveSpeed; }
    void set_MoveSpeed(float speed) { m_MoveSpeed = speed; }
};


#pragma once
#include "IComponent.h"

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:EnemyController Class --- */
//
//  ★継承：IComponent ★
//
// 【?】敵の挙動を管理
//		実際の挙動はステートマシン内で行っている
//		
// ***************************************************************************************
class EnemyController :  public IComponent
{
private:
	std::shared_ptr<class Health>m_pHealthComp;	// 体力管理コンポーネント
	std::shared_ptr<class SkinnedMeshAnimator> m_pAnimatorComp;	// アニメータコンポーネント
	bool m_IsDead;

	StateMachine<EnemyController> m_StateMachine;

public:
	EnemyController(std::weak_ptr<GameObject> pOwner, int updateRank);
	~EnemyController();

	void Start(RendererEngine& renderer) override;	// 初期化
	void Update(RendererEngine& renderer) override;// 更新

	void OnCollisionEnter(const class CollisionInfo& _other)override;
};


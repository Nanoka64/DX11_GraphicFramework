#include "pch.h"
#include "Component_EnemyController.h"
#include "GameObject.h"
#include "Component_SkinnedMeshAnimator.h"
#include "Component_Health.h"
#include "GameObject.h"
#include "RendererEngine.h"
#include "CollisionInfo.h"

using namespace GIGA_Engine;
using namespace VECTOR3;
using namespace VECTOR2;

//*---------------------------------------------------------------------------------------
//*【?】コンストラクタ
//* [引数]
//* pOwner : オーナーオブジェクト
//* updateRank : 更新レイヤー
//*----------------------------------------------------------------------------------------
EnemyController::EnemyController(std::weak_ptr<GameObject> pOwner, int updateRank)
    :IComponent(pOwner, updateRank),
    m_IsDead(false)
{
    this->set_Tag("EnemyController");
}

//*---------------------------------------------------------------------------------------
//*【?】デストラクタ
//*----------------------------------------------------------------------------------------
EnemyController::~EnemyController()
{

}

//*---------------------------------------------------------------------------------------
//*【?】初期化
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//* [返値]なし
//*----------------------------------------------------------------------------------------
void EnemyController::Start(RendererEngine& renderer)
{
    // アニメーションコンポーネントの取得
    m_pAnimatorComp = m_pOwner.lock()->get_Component<SkinnedMeshAnimator>();
	
	// HP管理コンポーネントの取得
	m_pHealthComp = m_pOwner.lock()->get_Component<Health>();
	m_pHealthComp->set_MaxHP(200.0f);
	m_pHealthComp->set_CrntHP(200.0f);
	// 被弾時のコールバック
	m_pHealthComp->RegisterOnDamage(
		[this](float _damage)
		{
		}
	);
	// 死亡時のコールバック
	m_pHealthComp->RegisterOnDead(
		[this]
		{
			m_IsDead = true;
		}
	);
}


//*---------------------------------------------------------------------------------------
//*【?】更新
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//* [返値]なし
//*----------------------------------------------------------------------------------------
void EnemyController::Update(RendererEngine& renderer)
{

}

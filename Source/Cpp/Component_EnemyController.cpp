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
    m_IsDead(false),
	m_StateMachine(this)
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

	// ステートの作成（TODO:外から種類を変えられるようにする）
	EnemyStateFactory::Create(m_StateMachine, ENEMY_TYPE::ENEMY_TYPE_ANT_Normal, renderer);
	m_StateMachine.SetCrntState(ANT_STATE::ANT_STATE_IDLE);
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
	if (m_IsDead)
	{
		m_pOwner.lock()->clear_StatusFlag(OBJECT_STATUS_BITFLAG::IS_ACTIVE);
	}
}

//*---------------------------------------------------------------------------------------
//*【?】衝突処理
//*
//* [引数]
//* & _other : 衝突相手の情報
//* [返値]なし
//*----------------------------------------------------------------------------------------
void EnemyController::OnCollisionEnter(const class CollisionInfo& _other)
{
	if (_other.get_HitObject().lock()->get_Tag() == "Bullet")
	{
		m_pHealthComp->TakeDamage(5.0f);
	}
}
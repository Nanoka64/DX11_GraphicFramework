#include "pch.h"
#include "Component_EnemyController.h"
#include "GameObject.h"
#include "Component_SkinnedMeshAnimator.h"
#include "Component_Collider.h"
#include "Component_Health.h"
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
	m_StateMachine(this),
	m_IsAnim(false),
	m_CrntAnimID(-1),
	m_MoveSpeed(0.0f),
	m_MoveVelocity(VECTOR3::VEC3()),
	m_StateTimer(0)
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

	// コライダーの取得
	m_pColliderComp = m_pOwner.lock()->get_Component<Collider>();

	// HP管理コンポーネントの取得
	m_pHealthComp = m_pOwner.lock()->get_Component<Health>();
	m_pHealthComp->set_MaxHP(200.0f);
	m_pHealthComp->set_CrntHP(200.0f);
	// 被弾時のコールバック
	m_pHealthComp->RegisterOnDamage(
		[this](float _damage)
		{
			auto transform = m_pOwner.lock()->get_Transform().lock();
			VEC3 pos = transform->get_VEC3ToPos();
			VEC3 rot;
			rot.x = Tool::RandRange(-3.14f, 3.14f);
			rot.y = Tool::RandRange(-3.14f, 3.14f);
			rot.z = Tool::RandRange(-3.14f, 3.14f);

			ChangeState(ANT_STATE::ANT_STATE_TRACKING);

			int handle = Master::m_pEffectManager->PlayEffect("Hit");
			Master::m_pEffectManager->SetScaleEffect(handle, 10.0f, 10.0f, 10.0f);
			Master::m_pEffectManager->SetPositionEffect(handle, pos.x, pos.y, pos.z);
			Master::m_pEffectManager->SetRotationEffect(handle, rot.x, rot.y, rot.z);
		}
	);
	// 死亡時のコールバック
	m_pHealthComp->RegisterOnDead(
		[this]
		{
			auto transform = m_pOwner.lock()->get_Transform().lock();
			VEC3 pos = transform->get_VEC3ToPos();
			int handle = Master::m_pEffectManager->PlayEffect("DeadExplosion");
			Master::m_pEffectManager->SetScaleEffect(handle,15.0f, 15.0f, 15.0f);
			Master::m_pEffectManager->SetPositionEffect(handle, pos.x, pos.y, pos.z);
			m_IsDead = true;
			m_IsAnim = false;
		}
	);

	m_MoveSpeed = 2.0f;

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
		m_pColliderComp->set_IsEnable(false);	// コライダーの判定をオフに
		m_pOwner.lock()->set_StatusFlag(OBJECT_STATUS_BITFLAG::IS_DELETE);
	}

	auto target = Master::m_pGameObjectManager->get_ObjectByTag("Player");
	m_pTarget = target;

	// ステートの実行
	m_StateMachine.Update();

	m_pAnimatorComp->set_IsAnim(m_IsAnim);

	m_StateTimer++;


	// 移動処理（今後移動コンポーネント等にまとめる）
	auto myTransform = m_pOwner.lock()->get_Transform().lock();
	VEC3 crntPos = myTransform->get_VEC3ToPos();
	VEC3 crntRot = myTransform->get_VEC3ToRotateToRad();

	VEC3 newPos = crntPos + m_MoveVelocity;

	newPos.y = crntPos.y;	// Yは変えない

	if (newPos.y <= 5.0f)
	{
		newPos.y = 5.0f;
	}

	myTransform->set_Pos(newPos);

	if (m_MoveVelocity.LengthSq() > 0.01)
	{
		//目標の方向ベクトルから角度値を算出c
		float targetAngle = atan2(m_MoveVelocity.x, m_MoveVelocity.z);

		// 線形補間
		//targetAngle = Tool::Lerp(crntRot.y, targetAngle, 0.1f);
		myTransform->set_RotateToRad(0.0f, targetAngle, 0.0f);
	}
}


//*---------------------------------------------------------------------------------------
//*【?】描画
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//* [返値]なし
//*----------------------------------------------------------------------------------------
void EnemyController::Draw(RendererEngine &renderer)
{
	// ステートの描画
	m_StateMachine.Draw();
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
		m_pHealthComp->TakeDamage(40.0f);
	}

}

//*---------------------------------------------------------------------------------------
//*【?】ステートの変更
//*
//* [引数]
//*  _state : ステート番号
//* [返値]なし
//*----------------------------------------------------------------------------------------
void EnemyController::ChangeState(const int _state)
{
	m_StateTimer = 0;	// ステート時間をリセット
	m_StateMachine.ChangeState(_state);
}

//*---------------------------------------------------------------------------------------
//*【?】アニメーションの変更
//*
//* [引数]
//*  _id : アニメーション番号
//* [返値]なし
//*----------------------------------------------------------------------------------------
void EnemyController::ChangeAnimation(const int _newId)
{
	// 同じまたは停止中なら返す
	if (_newId == m_CrntAnimID || m_IsAnim == false)
	{
		return;
	}

	// ひとつ前のアニメーションIDセット
	m_pAnimatorComp->set_PrevAnimIndex(static_cast<int>(m_CrntAnimID));

	m_CrntAnimID = _newId;

	// 現在のアニメーションIDセット
	m_pAnimatorComp->set_AnimIndex(static_cast<int>(_newId));
	m_pAnimatorComp->set_AnimTime(0.0f);
}

//*---------------------------------------------------------------------------------------
//*【?】ターゲットのトランスフォームを取得
//*
//* [引数]
//*  _id : アニメーション番号
//* [返値]なし
//*----------------------------------------------------------------------------------------
std::shared_ptr<Transform> EnemyController::get_TargetTransform() const
{
	if (m_pTarget)
	{
		return m_pTarget->get_Transform().lock();
	}
}
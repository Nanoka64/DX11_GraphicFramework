#include "pch.h"
#include "Component_EnemyController.h"
#include "GameObject.h"
#include "Component_SkinnedMeshAnimator.h"
#include "Component_DecalRenderer.h"
#include "Component_TimerDestruction.h"
#include "Component_Collider.h"
#include "Component_Health.h"
#include "RendererEngine.h"
#include "CollisionInfo.h"
#include "MeshFactory.h"
#include "ResourceManager.h"
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
		[this, &renderer](float _damage)
		{
			auto transform = m_pOwner.lock()->get_Transform().lock();
			VEC3 pos = transform->get_VEC3ToPos();
			VEC3 rot;
			rot.x = Tool::RandRange(-3.14f, 3.14f);
			rot.y = Tool::RandRange(-3.14f, 3.14f);
			rot.z = Tool::RandRange(-3.14f, 3.14f);

			ChangeState(ANT_STATE::ANT_STATE_TRACKING);

			// ****************************************************
			//				 被弾音再生
			// ****************************************************
			//Master::m_pSoundManager->Play_RandPitch(SOUND_TYPE::SE, SOUND_ID_TO_INT(SOUND_ID::ENEMY_ANT_HIT01), 300);
			Master::m_pSoundManager->Play_3D(SOUND_TYPE::SE, SOUND_ID_TO_INT(SOUND_ID::ENEMY_ANT_HIT01), pos);

			int handle = Master::m_pEffectManager->PlayEffect("Hit");
			Master::m_pEffectManager->SetScaleEffect(handle, 10.0f, 10.0f, 10.0f);
			Master::m_pEffectManager->SetPositionEffect(handle, pos.x, pos.y, pos.z);
			Master::m_pEffectManager->SetRotationEffect(handle, rot.x, rot.y, rot.z);



			auto matPtr = Master::m_pResourceManager->FindMaterial("Decal_Ant_Splash");

			SetupMaterialInfo matInfo[1];
			matInfo[0].Index = 0;
			matInfo[0].pMaterialData = matPtr;

			CreateDecalInfo decal;
			decal.pRenderer = &renderer;
			decal.Type = UTILITY_MESH_TYPE::CUBU;
			decal.MatNum = 1;
			decal.MaterialData = matInfo;
			decal.IsActive = false;
			decal.ShaderType = SHADER_TYPE::DEFERRED_STD_DECAL;
			decal.IsNormalMap = false;
			decal.IsDynamic = true;


			VEC3 scale;
			scale.x = 100.0f;
			scale.y = 100.0f;
			scale.z = 100.0f;

			auto obj = MeshFactory::CreateDecal(decal);
			obj->get_Component<DecalRenderer>()->Start(renderer);
			obj->get_Transform().lock()->set_Pos(pos);
			obj->get_Transform().lock()->set_Scale(scale);
			obj->get_Transform().lock()->set_RotateToRad(1.57, Tool::RandRange(0.0f, 6.14f), 0.0f);
			obj->set_Tag("Ant_Splash");
			auto timer = obj->add_Component<TimerDestruction>();
			timer->set_LifeTime(8.0f);  // 生存時間
		}
	);
	// 死亡時のコールバック
	m_pHealthComp->RegisterOnDead(
		[this, &renderer]
		{
			auto transform = m_pOwner.lock()->get_Transform().lock();
			VEC3 pos = transform->get_VEC3ToPos();
			
			// ****************************************************
			//				 死亡音再生
			// ****************************************************
			//Master::m_pSoundManager->Play_RandPitch(SOUND_TYPE::SE, SOUND_ID_TO_INT(SOUND_ID::ENEMY_ANT_DEAD), 300);
			Master::m_pSoundManager->Play_3D(SOUND_TYPE::SE, SOUND_ID_TO_INT(SOUND_ID::ENEMY_ANT_DEAD), pos);

            // 死亡エフェクト
			int handle = Master::m_pEffectManager->PlayEffect("DeadExplosion");
			Master::m_pEffectManager->SetScaleEffect(handle,15.0f, 15.0f, 15.0f);
			Master::m_pEffectManager->SetPositionEffect(handle, pos.x, pos.y, pos.z);
			m_IsDead = true;
			m_IsAnim = false;

			Master::m_pSoundManager->Play_3D(SOUND_TYPE::SE, SOUND_ID_TO_INT(SOUND_ID::ENEMY_ANT_DEAD), pos);


			auto matPtr = Master::m_pResourceManager->FindMaterial("Decal_Ant_Splash");

			SetupMaterialInfo matInfo[1];
			matInfo[0].Index = 0;
			matInfo[0].pMaterialData = matPtr;

			CreateDecalInfo decal;
			decal.pRenderer = &renderer;
			decal.Type = UTILITY_MESH_TYPE::CUBU;
			decal.MatNum = 1;
			decal.MaterialData = matInfo;
			decal.IsActive = false;
			decal.ShaderType = SHADER_TYPE::DEFERRED_STD_DECAL;
			decal.IsNormalMap = false;
			decal.IsDynamic = true;


			VEC3 scale;
			scale.x = 200.0f;
			scale.y = 200.0f;
			scale.z = 200.0f;

			auto obj = MeshFactory::CreateDecal(decal);
			obj->get_Component<DecalRenderer>()->Start(renderer);
			obj->get_Transform().lock()->set_Pos(pos);
			obj->get_Transform().lock()->set_Scale(scale);
			obj->get_Transform().lock()->set_RotateToRad(1.57, 0.0f, 0.0f);
			obj->set_Tag("Ant_Splash");
			auto timer = obj->add_Component<TimerDestruction>();
			timer->set_LifeTime(15.0f);  // 生存時間
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

	//目標の方向ベクトルから角度値を算出c
	float targetAngleY = atan2(m_MoveVelocity.x, m_MoveVelocity.z);

	// 目標とするクォータニオン
	XMVECTOR targetRotQ = XMQuaternionRotationRollPitchYaw(0.0f, targetAngleY, 0.0f);

	XMVECTOR crntRotQ = myTransform->get_RotationQuaternion();

	// クォータニオンの球面線形補間
	// 普通の線形補間だと、値が飛んでしまうためクォータニオンの場合は球面線形補間を使う
	XMVECTOR newRotQ = XMQuaternionSlerp(crntRotQ, targetRotQ, 0.1f);

	myTransform->set_RotationQuaternion(newRotQ);


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
	if (_other.get_HitObject().lock()->get_Tag() == "Bullet_Normal")
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
std::shared_ptr<MyTransform> EnemyController::get_TargetTransform() const
{
	if (m_pTarget)
	{
		return m_pTarget->get_Transform().lock();
	}
}
#include "pch.h"
#include "Component_EnemyController.h"
#include "GameObject.h"
#include "Component_SkinnedMeshAnimator.h"
#include "Component_DecalRenderer.h"
#include "Component_MoveLogic.h"
#include "Component_TimerDestruction.h"
#include "Component_Collider.h"
#include "Component_BoxCollider.h"
#include "Component_Health.h"
#include "Component_Physics.h"
#include "RendererEngine.h"
#include "CollisionInfo.h"
#include "MeshFactory.h"
#include "ResourceManager.h"


using namespace GIGA_Engine;
using namespace VECTOR3;
using namespace VECTOR2;
using namespace EnemyData;
using namespace UtilityData;

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
	m_IsGrounded(false),
	m_CrntAnimID(-1),
	m_MoveSpeed(0.0f),
	m_MoveVelocity(VECTOR3::VEC3()),
	m_StateTimer(0),
	m_GravityVelocity(0.0f),
	m_pEnemyData(nullptr),
	m_pTarget(nullptr),
	m_Gravity(18.0f),
	m_AnimSpeed(1.25f)
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
	auto ownerObj = m_pOwner.lock();;

    // アニメーションコンポーネントの取得
    m_pAnimatorComp = ownerObj->get_Component<SkinnedMeshAnimator>();

	// コライダーの取得
	m_pColliderComp = ownerObj->get_Component<BoxCollider>();

	// 移動コンポーネントの取得
	m_pMoveLogicComp = ownerObj->get_Component<MoveLogic>();

	// 物理コンポーネントの取得
	m_pPhysicsComp = ownerObj->get_Component<Physics>();

	// HP管理コンポーネントの取得
	m_pHealthComp = m_pOwner.lock()->get_Component<Health>();

	// TODO:処理関数を外から入れるようにする
	// 被弾時の処理登録
	m_pHealthComp.lock()->RegisterOnDamage(
		[this, &renderer](float _damage)
		{
			auto transform = m_pOwner.lock()->get_Transform().lock();
			VEC3 pos = transform->get_VEC3ToPos();
			VEC3 rot;
			rot.x = Tool::RandRange(-3.14f, 3.14f);
			rot.y = Tool::RandRange(-3.14f, 3.14f);
			rot.z = Tool::RandRange(-3.14f, 3.14f);

			ChangeState(ANT_STATE::ANT_STATE_ACTIVE_TRACKING);

			// ****************************************************
			//				 被弾音再生
			// ****************************************************
			Master::m_pSoundManager->Play_3D(SOUND_TYPE::SE, SOUND_ID_TO_INT(SOUND_ID::ENEMY_ANT_HIT01), pos, SOUND_HIT_RADIUS);

			int hit_handle = Master::m_pEffectManager->PlayEffect("EnemyHit_01");
			Master::m_pEffectManager->SetScaleEffect(hit_handle, 1.0f, 1.0f, 1.0f);
			Master::m_pEffectManager->SetPositionEffect(hit_handle, pos.x, pos.y + 2.0f, pos.z);	// 位置が足元になってしまってるので、少し上に補正
			Master::m_pEffectManager->SetRotationEffect(hit_handle, rot.x, rot.y, rot.z);


			auto matPtr = Master::m_pResourceManager->FindMaterial("Decal_Ant_Splash");
			SetupMaterialInfo matInfo[1];
			matInfo[0].Index = 0;
			matInfo[0].pMaterialData = matPtr;

			CreateDecalInfo decal;
			decal.pRenderer = &renderer;
			decal.Type = UTILITY_MESH_TYPE::CUBE;
			decal.MatNum = 1;
			decal.MaterialData = matInfo;
			decal.IsActive = false;
			decal.ShaderType = SHADER_TYPE::DEFERRED_STD_DECAL;
			decal.IsNormalMap = false;
			decal.IsDynamic = true;

			VEC3 scale;
			scale.x = 6.0f;
			scale.y = 6.0f;
			scale.z = 1.0f;
			auto obj = MeshFactory::CreateDecal(decal);
			obj->get_Component<DecalRenderer>()->Start(renderer);
			obj->get_Transform().lock()->set_Pos(pos);
			obj->get_Transform().lock()->set_Scale(scale);
			obj->get_Transform().lock()->set_RotateToRad(1.57f, Tool::RandRange(0.0f, 6.14f), 0.0f);
			obj->set_Tag("Ant_Splash");
			auto timer = obj->add_Component<TimerDestruction>();
			timer->set_LifeTime(8.0f);  // 生存時間
		}
	);
	// 死亡時の処理登録
	m_pHealthComp.lock()->RegisterOnDead(
		[this, &renderer]
		{
			auto transform = m_pOwner.lock()->get_Transform().lock();
			VEC3 pos = transform->get_VEC3ToPos();

			// ****************************************************
			//				 死亡音再生
			// ****************************************************
			Master::m_pSoundManager->Play_3D(SOUND_TYPE::SE, SOUND_ID_TO_INT(SOUND_ID::ENEMY_ANT_DEAD), pos, SOUND_DEAD_RADIUS);

			// ****************************************************
			//				アイテム出現
			// ****************************************************
			Master::m_pItemManager->SpawnItemRand(DROP_ITEM_MIN, DROP_ITEM_MAX, pos, 0.0f);


            // 死亡エフェクト
			int handle = Master::m_pEffectManager->PlayEffect("EnemyDead_01");
			Master::m_pEffectManager->SetScaleEffect(handle,1.0f,1.0f, 1.0f);
			Master::m_pEffectManager->SetPositionEffect(handle, pos.x, pos.y + 2.0f, pos.z);
			m_IsDead = true;
			m_IsAnim = false;

			auto matPtr = Master::m_pResourceManager->FindMaterial("Decal_Ant_Splash");
			SetupMaterialInfo matInfo[1];
			matInfo[0].Index = 0;
			matInfo[0].pMaterialData = matPtr;

			CreateDecalInfo decal;
			decal.pRenderer = &renderer;
			decal.Type = UTILITY_MESH_TYPE::CUBE;
			decal.MatNum = 1;
			decal.MaterialData = matInfo;
			decal.IsActive = false;
			decal.ShaderType = SHADER_TYPE::DEFERRED_STD_DECAL;
			decal.IsNormalMap = false;
			decal.IsDynamic = true;

			VEC3 scale;
			scale.x = 20.0f;
			scale.y = 20.0f;
			scale.z = 1.0f;

			auto obj = MeshFactory::CreateDecal(decal);
			obj->get_Component<DecalRenderer>()->Start(renderer);
			obj->get_Transform().lock()->set_Pos(pos);
			obj->get_Transform().lock()->set_Scale(scale);
			obj->get_Transform().lock()->set_RotateToRad(1.57f, Tool::RandRange(0.0f, 6.14f), 0.0f);
			obj->set_Tag("Ant_Splash");
			auto timer = obj->add_Component<TimerDestruction>();
			timer->set_LifeTime(15.0f);  // 生存時間
		}
	);

	// 開始時の座標を入れる
	m_StartPos = m_pOwner.lock()->get_Transform().lock()->get_VEC3ToPos();

	// ステートの作成（TODO:外から種類を変えられるようにする）
	EnemyStateFactory::Create(m_StateMachine, ENEMY_TYPE::ENEMY_TYPE_ANT_Normal, renderer);
	m_StateMachine.SetCrntState(ANT_STATE::ANT_STATE_PATROL_IDLE);

	m_pMoveLogicComp.lock()->Register(MOVE_BEHAVIOUR_TYPE::HOMING);
	m_pMoveLogicComp.lock()->Register(MOVE_BEHAVIOUR_TYPE::LINEAR);
	m_pMoveLogicComp.lock()->ChangeBehaviour(MOVE_BEHAVIOUR_TYPE::LINEAR);
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
	m_IsGrounded = false;
}


//*---------------------------------------------------------------------------------------
//*【?】遅延更新
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//* [返値]なし
//*----------------------------------------------------------------------------------------
void EnemyController::LateUpdate(RendererEngine& renderer)
{
	float deltaTime = Master::m_pTimeManager->get_DeltaTime();

	auto target = Master::m_pGameObjectManager->get_ObjectByTagConst("Player");
	m_pTarget = target;

	m_pAnimatorComp.lock()->set_IsAnim(m_IsAnim);
	m_StateTimer += deltaTime;	// タイマー進める

	// ステートの実行
	m_StateMachine.Update();

	m_pAnimatorComp.lock()->PlayAnim(Master::m_pTimeManager->get_DeltaTime() * m_AnimSpeed);

	auto transform = m_pOwner.lock()->get_Transform().lock();
	VEC3 newPos = transform->get_VEC3ToPos();

	// 世界の裏側に落下した場合
	if (newPos.y < -100.0f)
	{
		m_GravityVelocity = 0.0f;
		transform->set_Pos(VEC3(0.0f, 100.0f, 0.0f));
		m_pPhysicsComp.lock()->SetZeroVelocity();
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
	VEC3 normal = _other.get_HitNormal();

	// 法線のY成分が一定以上なら床とみなす
	if (normal.y < -0.7f)
	{
		// めり込み防止のため、下方向への速度をリセット
		if (m_GravityVelocity < 0.0f) {
			m_GravityVelocity = 0.0f;
		}
		m_IsGrounded = true;
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

	auto animComp = m_pAnimatorComp.lock();

	// ひとつ前のアニメーションIDセット
	animComp->set_PrevAnimIndex(static_cast<int>(m_CrntAnimID));

	m_CrntAnimID = _newId;

	// 現在のアニメーションIDセット
	animComp->set_AnimIndex(static_cast<int>(_newId));
	animComp->set_AnimTime(0.0f);
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

	return nullptr;
}

//*---------------------------------------------------------------------------------------
//*【?】移動ロジックの切り替え
//*
//* [引数]
//*  _moveType : 移動タイプ
//* [返値]なし
//*----------------------------------------------------------------------------------------
void EnemyController::set_MoveLogicState(UtilityData::MOVE_BEHAVIOUR_TYPE _moveType)
{
	m_pMoveLogicComp.lock()->ChangeBehaviour(_moveType);
}

//*---------------------------------------------------------------------------------------
//*【?】パラメータ取得
//*
//* [引数] なし
//* [返値] 書き換え不可のエネミーパラメータ
//*----------------------------------------------------------------------------------------
const EnemyData::BaseEnemyData* EnemyController::get_EnemyData()const
{
	return static_cast<const EnemyData::BaseEnemyData*>(m_pEnemyData);
}

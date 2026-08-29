#include "pch.h"
#include "Building_State.h"
#include "RendererEngine.h"
#include "Component_3DCamera.h"
#include "Component_BuildingController.h"
#include "Component_BoxCollider.h"

using namespace VECTOR2;
using namespace VECTOR3;
using namespace BuildingData;
using namespace Tool;

/* 倒壊パラメータ */
constexpr float BUILDING_COLLAPSE_TIME_MIN = 2.0f;	// 倒壊にかかる時間の最小値
constexpr float BUILDING_COLLAPSE_TIME_MAX = 5.0f;	// 倒壊にかかる時間の最大値
constexpr float BUILDING_COLLAPSE_SPEED = 25.0f;	// 建物の倒壊スピード
constexpr float BUILDING_COLLAPSE_END_TIME = 3.0f;	// 倒壊終了から落下までの時間
constexpr float BUILDING_FALL_SPEED = 20.0f;		// 建物の落下スピード

/* サウンド */
constexpr float BUILDING_DESTRUCTION_SOUND_RADIUS = 500.0f;	// 破壊時の音の聞こえる範囲
constexpr float BUILDING_FALL_SOUND_RADIUS = 500.0f;		// 落下時の音の聞こえる範囲

/* カメラシェイク */
constexpr float SHAKE_DURATION = 3.0f;		// 持続時間
constexpr float SHAKE_LENGTH = 0.3f;		// 強さ
constexpr float SHAKE_MAX_RANGE = 300.0f;	// 揺れが影響する最大距離


//////////////////////////////////////////////////////////////////////////////////////////

//								待機ステート
 
//////////////////////////////////////////////////////////////////////////////////////////
void Building_IdleState::OnEnter(BuildingController* pOwner)
{

}

void Building_IdleState::OnExit(BuildingController* pOwner)
{

}

int Building_IdleState::Update(BuildingController* pOwner)
{
	if (pOwner->get_IsDestruction())
	{
		return BUILDING_STATE::BUILDING_STATE_CLLAPSE_IN;	// 倒壊始めステートへ
	}

	return BUILDING_STATE::BUILDING_STATE_IDLE;
}


//////////////////////////////////////////////////////////////////////////////////////////

//								倒壊始めステート

//////////////////////////////////////////////////////////////////////////////////////////
void Building_CllapseInState::OnEnter(BuildingController* pOwner)
{
	auto ownerObj =pOwner->get_OwnerObj().lock();

	VEC3 pos = ownerObj->get_Transform().lock()->get_VEC3ToPos();
	Master::m_pSoundManager->Play_3D(SOUND_TYPE::SE, INT_CAST(SOUND_ID::BUILDING_DESTRUCTION), pos, BUILDING_DESTRUCTION_SOUND_RADIUS);


	// 倒壊が始まったら、コライダーをオフにする
	auto collider = ownerObj->get_Component<BoxCollider>();
	if (collider){
		collider->set_IsEnable(false);
	}


	//*****************************************************************************************
	//						エフェクト再生
	//					煙がブワッと出てくる感じ
	//*****************************************************************************************
	float scale = 10.0f;

	VEC3 rot;
	for (int i = 0; i < 5; i++)
	{
		rot.x = Master::m_pRandomManager->GetFloatRandom(-G_PI_F, G_PI_F);
		rot.y = Master::m_pRandomManager->GetFloatRandom(-G_PI_F, G_PI_F);
		rot.z = Master::m_pRandomManager->GetFloatRandom(-G_PI_F, G_PI_F);

		int handle = Master::m_pEffectManager->PlayEffect("Explosion_Smoke_02");
		Master::m_pEffectManager->SetPositionEffect(handle, pos.x, pos.y, pos.z);
		Master::m_pEffectManager->SetRotationEffect(handle, rot.x, rot.y, rot.z);
		Master::m_pEffectManager->SetScaleEffect(handle, scale, scale, scale);
	}
	ownerObj->set_IsStatic(false);	// 動的オブジェクトに変更

}

void Building_CllapseInState::OnExit(BuildingController* pOwner)
{

}

int Building_CllapseInState::Update(BuildingController* pOwner)
{
	return BUILDING_STATE::BUILDING_STATE_CLLAPSE_NOW;	// 倒壊中ステートへ
}


//////////////////////////////////////////////////////////////////////////////////////////

//								倒壊中ステート

//////////////////////////////////////////////////////////////////////////////////////////
void Building_CllapseNowState::OnEnter(BuildingController* pOwner)
{
	m_CollapseTime = Master::m_pRandomManager->GetFloatRandom(BUILDING_COLLAPSE_TIME_MIN, BUILDING_COLLAPSE_TIME_MAX);	// 倒壊にかかる時間をランダムで決める

	m_CrntCollapseTime = 0.0f;	// 現在の倒壊時間を初期化
	m_SunkRateY = 0.0f;
	m_FrameCounter = 0;

	m_CollapseTargetAngle = Master::m_pRandomManager->GetFloatRandom(-0.3f, 0.3f);	// 目標角度を決める
}

void Building_CllapseNowState::OnExit(BuildingController* pOwner)
{
	auto transform = pOwner->get_OwnerObj().lock()->get_Transform().lock();

	VEC3 pos = transform->get_VEC3ToPos();

	// ****************************************************
	//				 カメラシェイク
	// ****************************************************
	std::shared_ptr<Camera3D> camera;
	if (camera = Master::m_pDataManager->get_CameraComponent().lock())
	{
		camera->DistanceDecay(SHAKE_DURATION, VEC3(SHAKE_LENGTH), pos, SHAKE_MAX_RANGE);
	}




	//*****************************************************************************************
	//						エフェクト再生
	//				煙と破片と、ドデカい爆発が起きる感じ
	//*****************************************************************************************
	float scale = 10.0f;
	int handle = Master::m_pEffectManager->PlayEffect("Smoke_02");
	Master::m_pEffectManager->SetPositionEffect(handle, pos.x, pos.y + m_SunkRateY, pos.z);
	Master::m_pEffectManager->SetRotationEffect(handle, 0.0f, 0.0f, 0.0f);
	Master::m_pEffectManager->SetScaleEffect(handle, scale, scale, scale);

	handle = Master::m_pEffectManager->PlayEffect("DestructionFragments");
	Master::m_pEffectManager->SetPositionEffect(handle, pos.x, pos.y + m_SunkRateY, pos.z);
	Master::m_pEffectManager->SetRotationEffect(handle, 0.0f, 0.0f, 0.0f);
	Master::m_pEffectManager->SetScaleEffect(handle, scale, scale, scale);

	VEC3 explosionRot = 0.0f;
	VEC3 explosionScale = 5.0f;
	handle = Master::m_pEffectManager->PlayEffect("Explosion_03");
	Master::m_pEffectManager->SetPositionEffect(handle, pos.x, pos.y + m_SunkRateY, pos.z);
	Master::m_pEffectManager->SetRotationEffect(handle, explosionRot.x, explosionRot.y, explosionRot.z);
	Master::m_pEffectManager->SetScaleEffect(handle, explosionScale.x, explosionScale.y, explosionScale.z);
}

int Building_CllapseNowState::Update(BuildingController* pOwner)
{
	float deltaTime = Master::m_pTimeManager->get_DeltaTime();
	m_CrntCollapseTime += deltaTime;	// 倒壊時間を更新

	if (pOwner->get_IsActiveOwnerObj())
	{
		auto transform = pOwner->get_OwnerObj().lock()->get_Transform().lock();

		VEC3 pos = transform->get_VEC3ToPos();
		VEC3 rot = transform->get_VEC3ToRotateToRad();

		float t = m_CrntCollapseTime / m_CollapseTime;
		float easeIn = Tool::Easing::EaseInSine(t);

		float speed = BUILDING_COLLAPSE_SPEED * easeIn;;
		
		/* 沈んでいくような感じに */
		pos.y -= speed * deltaTime;

		// どのくらい沈んだかを保持
		m_SunkRateY += speed * deltaTime;


		/* 倒れるような感じに */
		float crntAngle = m_CollapseTargetAngle * easeIn;
		rot.x = crntAngle;
		rot.z = crntAngle;

		transform->set_Pos(pos);
		transform->set_RotateToRad(rot);


		m_FrameCounter++;

		//*****************************************************************************************
		//						エフェクト再生
		//				崩れていく際に、炎が出ている感じにする
		//*****************************************************************************************
		if (m_FrameCounter % 30 == 0)
		{
			VEC3 explosionRot;
			VEC3 explosionScale = 4.0f;
			explosionRot.x = Master::m_pRandomManager->GetFloatRandom(-G_PI_F, G_PI_F);
			explosionRot.y = Master::m_pRandomManager->GetFloatRandom(-G_PI_F, G_PI_F);
			explosionRot.z = Master::m_pRandomManager->GetFloatRandom(-G_PI_F, G_PI_F);
			float explosionPosY_Offset;
			explosionPosY_Offset = Master::m_pRandomManager->GetFloatRandom(1.0f, 50.0f);
			int handle = Master::m_pEffectManager->PlayEffect("Explosion_02");
			Master::m_pEffectManager->SetPositionEffect(handle, pos.x, pos.y + explosionPosY_Offset, pos.z);
			Master::m_pEffectManager->SetRotationEffect(handle, explosionRot.x, explosionRot.y, explosionRot.z);
			Master::m_pEffectManager->SetScaleEffect(handle, explosionScale.x, explosionScale.y, explosionScale.z);
		}
	}

	if(m_CollapseTime <= m_CrntCollapseTime )
	{
		return BUILDING_STATE::BUILDING_STATE_CLLAPSE_END;	// 倒壊終了ステートへ
	}

	return BUILDING_STATE::BUILDING_STATE_CLLAPSE_NOW;
}


//////////////////////////////////////////////////////////////////////////////////////////

//								倒壊終了ステート

//////////////////////////////////////////////////////////////////////////////////////////
void Building_CllapseEndState::OnEnter(BuildingController* pOwner)
{
	VEC3 pos = pOwner->get_OwnerObj().lock()->get_Transform().lock()->get_VEC3ToPos();
	Master::m_pSoundManager->Play_3D(SOUND_TYPE::SE, INT_CAST(SOUND_ID::BUILDING_FALL), pos, BUILDING_FALL_SOUND_RADIUS);


	m_CrntCollapseEndTime = 0.0f;

	m_Rot = pOwner->get_OwnerObj().lock()->get_Transform().lock()->get_VEC3ToRotateToRad();
}

void Building_CllapseEndState::OnExit(BuildingController* pOwner)
{

}

int Building_CllapseEndState::Update(BuildingController* pOwner)
{
	float deltaTime = Master::m_pTimeManager->get_DeltaTime();

	m_CrntCollapseEndTime += deltaTime;	// 倒壊終了時間を更新

	if (pOwner->get_IsActiveOwnerObj())
	{
		auto transform = pOwner->get_OwnerObj().lock()->get_Transform().lock();

		VEC3 rot = transform->get_VEC3ToRotateToRad();

		float t = m_CrntCollapseEndTime / BUILDING_COLLAPSE_END_TIME;
		float easeBack = Tool::Easing::EaseOutBack(t);

		rot.x = Tool::Lerp(m_Rot.x, m_Rot.x + 0.1f, easeBack);
		rot.z = Tool::Lerp(m_Rot.z, m_Rot.z + 0.1f, easeBack);

		transform->set_RotateToRad(rot);
	}

	// 倒壊終了時間が一定以上経過したら落下ステートへ
	if (BUILDING_COLLAPSE_END_TIME <= m_CrntCollapseEndTime)
	{
		return BUILDING_STATE::BUILDING_STATE_FALL;	// 落下ステートへ
	}

	return BUILDING_STATE::BUILDING_STATE_CLLAPSE_END;
}


//////////////////////////////////////////////////////////////////////////////////////////

//					倒壊が終わって、裏世界へ落ちていくステート

//////////////////////////////////////////////////////////////////////////////////////////
void Building_FallState::OnEnter(BuildingController* pOwner)
{

}

void Building_FallState::OnExit(BuildingController* pOwner)
{

}

int Building_FallState::Update(BuildingController* pOwner)
{
	float deltaTime = Master::m_pTimeManager->get_DeltaTime();

	if (pOwner->get_IsActiveOwnerObj())
	{
		auto transform = pOwner->get_OwnerObj().lock()->get_Transform().lock();

		VEC3 pos = transform->get_VEC3ToPos();

		pos.y -= BUILDING_FALL_SPEED * deltaTime;

		transform->set_Pos(pos);

		if (pos.y < -100.0f)
		{
			return BUILDING_STATE::BUILDING_STATE_END;	// 終了ステートへ
		}
	}
	return BUILDING_STATE::BUILDING_STATE_FALL;
}


//////////////////////////////////////////////////////////////////////////////////////////

//								終了ステート

//////////////////////////////////////////////////////////////////////////////////////////
void Building_EndState::OnEnter(BuildingController* pOwner)
{

}

void Building_EndState::OnExit(BuildingController* pOwner)
{

}

int Building_EndState::Update(BuildingController* pOwner)
{
	pOwner->get_OwnerObj().lock()->set_StatusFlag(OBJECT_STATUS_BITFLAG::IS_DELETE);

	return BUILDING_STATE::BUILDING_STATE_END;
}
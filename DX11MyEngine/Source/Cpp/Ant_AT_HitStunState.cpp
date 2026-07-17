#include "pch.h"
#include "Component_EnemyController.h"
#include "Component_Physics.h"
#include "Ant_StateHeader.h"
#include "GameObject.h"
#include "Component_BoxCollider.h"
#include "Component_DecalRenderer.h"
#include "Component_TimerDestruction.h"
#include "RendererEngine.h"
#include "MeshFactory.h"

using namespace DirectX;
using namespace VECTOR3;
using namespace UtilityData;
using namespace EnemyData;

//*---------------------------------------------------------------------------------------
//* @:Ant_AT_HitStunState Class 
//*【?】開始
//* 引数：1.EnemyController
//* 返値：void
//*----------------------------------------------------------------------------------------
void Ant_AT_HitStunState::OnEnter(class EnemyController* pOwner)
{
	auto myTransform = pOwner->get_TransformComponent();
	VEC3 pos = myTransform->get_VEC3ToPos();
	VEC3 rot;
	rot.x = Tool::RandRange(-3.14f, 3.14f);
	rot.y = Tool::RandRange(-3.14f, 3.14f);
	rot.z = Tool::RandRange(-3.14f, 3.14f);

	pOwner->ChangeState(ANT_STATE::ANT_STATE_ACTIVE_TRACKING);

	// ****************************************************
	//				 被弾音再生
	// ****************************************************
	Master::m_pSoundManager->Play_3D(SOUND_TYPE::SE, SOUND_ID_TO_INT(SOUND_ID::ENEMY_ANT_HIT01), pos, SOUND_HIT_RADIUS);

	int hit_handle = Master::m_pEffectManager->PlayEffect("AntHit_01");
	Master::m_pEffectManager->SetScaleEffect(hit_handle, 1.0f, 1.0f, 1.0f);
	Master::m_pEffectManager->SetPositionEffect(hit_handle, pos.x, pos.y + 2.0f, pos.z);	// 位置が足元になってしまってるので、少し上に補正
	Master::m_pEffectManager->SetRotationEffect(hit_handle, rot.x, rot.y, rot.z);


	auto matPtr = Master::m_pResourceManager->FindMaterial("Decal_Ant_Splash");
	SetupMaterialInfo matInfo[1];
	matInfo[0].Index = 0;
	matInfo[0].pMaterialData = matPtr;

	CreateDecalInfo decal;
	decal.pRenderer = m_pRenderer;
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
	obj->get_Component<DecalRenderer>()->Start(*m_pRenderer);
	auto decalTransform = obj->get_Transform().lock();
	decalTransform->set_Pos(pos);
	decalTransform->set_Scale(scale);
	decalTransform->set_RotateToRad(1.57f, Tool::RandRange(0.0f, 6.14f), 0.0f);
	obj->set_Tag("Ant_Splash");
	auto timer = obj->add_Component<TimerDestruction>();
	timer->set_LifeTime(8.0f);  // 生存時間
}

//*---------------------------------------------------------------------------------------
//* @:Ant_AT_HitStunState Class 
//*【?】終了
//* 引数：1.EnemyController
//* 返値：void
//*----------------------------------------------------------------------------------------
void Ant_AT_HitStunState::OnExit(class EnemyController* pOwner)
{

}

//*---------------------------------------------------------------------------------------
//* @:Ant_AT_HitStunState Class 
//*【?】更新
//* 引数：1.EnemyController
//* 返値：void
//*----------------------------------------------------------------------------------------
int Ant_AT_HitStunState::Update(class EnemyController* pOwner)
{
	// 共通処理
	int commonRes = Ant_CommonStateProcess::CommonProcess(pOwner);
	if (commonRes != -1)
	{
		return commonRes;
	}

	return ANT_STATE::ANT_STATE_ACTIVE_MOVE;
}


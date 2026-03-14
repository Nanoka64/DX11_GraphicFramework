#include "pch.h"
#include "Component_ExplosionLightController.h"
#include "Component_PointLight.h"
#include "RendererEngine.h"

using namespace VECTOR3;

//*---------------------------------------------------------------------------------------
//*【?】コンストラクタ
//* [引数]
//* pOwner : オーナーオブジェクト
//* updateRank : 更新レイヤー
//*----------------------------------------------------------------------------------------
ExplosionLightController::ExplosionLightController(std::weak_ptr<GameObject> pOwner, int updateRank)
	: IComponent(pOwner, updateRank),
	m_Parameter(),
	m_Timer(0.0f)
{
	this->set_Tag("ExplosionLightController");
}


//*---------------------------------------------------------------------------------------
//*【?】デストラクタ
//*----------------------------------------------------------------------------------------
ExplosionLightController::~ExplosionLightController()
{

}


//*---------------------------------------------------------------------------------------
//*【?】初期化
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//* [返値]なし
//*----------------------------------------------------------------------------------------
void ExplosionLightController::Start(RendererEngine &renderer)
{
	// プールでオブジェクトが管理されているため、コンポーネントが消されることはないので、
	// ここで参照ポインタを取ってしまう
	// 念のため、Setup関数の方に移してもいいかも
	m_pPointLight = m_pOwner.lock()->get_Component<PointLight>();
}


//*---------------------------------------------------------------------------------------
//*【?】更新
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//* [返値]なし
//*----------------------------------------------------------------------------------------
void ExplosionLightController::Update(RendererEngine &renderer)
{
	if (m_pPointLight.expired())return;

	float deltaTime = Master::m_pTimeManager->get_DeltaTime();
	m_Timer += deltaTime;

	float t = m_Timer / m_Parameter._explosionDuration;	// 0.0 ～ 1.0にする

	// 時間を過ぎたらアクティブ降ろす
	if (t >= 1.0f)
	{
		m_pOwner.lock()->clear_StatusFlag(OBJECT_STATUS_BITFLAG::IS_ACTIVE);
	}

	float easeOut = Tool::Easing::EaseOutQuint(t);

	// 発光範囲
	float range = (m_Parameter._explosionLightRadius * 12.0f) * (1.0f - easeOut);

	auto pPointLight = m_pPointLight.lock();
	pPointLight->set_Intensity(50.0f * (1.0f - easeOut));
	pPointLight->set_Range(range);
	pPointLight->set_LightColor(VEC3(0.8f, 0.4f, 0.1f));

}


//*---------------------------------------------------------------------------------------
//*【?】描画
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//* [返値]なし
//*----------------------------------------------------------------------------------------
void ExplosionLightController::Draw(RendererEngine &renderer)
{

}


//*---------------------------------------------------------------------------------------
//*【?】プールへの返却時に実行させる
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//* [返値]なし
//*----------------------------------------------------------------------------------------
void ExplosionLightController::Reset()
{
	m_Parameter.Reset();
	m_Timer = 0.0f;
}


//*---------------------------------------------------------------------------------------
//*【?】プールからの取り出し時に実行させる
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//* [返値]なし
//*----------------------------------------------------------------------------------------
void ExplosionLightController::Setup()
{
	m_pPointLight = m_pOwner.lock()->get_Component<PointLight>();
}
#include "pch.h"
#include "Component_BuildingController.h"
#include "Component_Health.h"
#include "ConstantBuildingData.h"
#include "BuildingStateFactory.h"

using namespace BuildingData;

constexpr float BUILDING_MAX_HP = 600.0f;	// 建物の最大HP

//*---------------------------------------------------------------------------------------
//*【?】コンストラクタ
//*----------------------------------------------------------------------------------------
BuildingController::BuildingController(std::weak_ptr<GameObject> pOwner, int updateRank):
	IComponent(pOwner,updateRank),
	m_StateMachine(this),
	m_IsDestruction(false)
{
}
//*---------------------------------------------------------------------------------------
//*【?】デストラクタ
//*----------------------------------------------------------------------------------------
BuildingController::~BuildingController()
{
}

//*---------------------------------------------------------------------------------------
//*【?】初期化
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//* [返値]なし
//*----------------------------------------------------------------------------------------
void BuildingController::Start(RendererEngine& renderer)
{
	m_pHealthComp = get_OwnerObj().lock()->get_Component<Health>();	// 体力管理コンポーネントの取得
	m_pHealthComp.lock()->set_MaxHP(BUILDING_MAX_HP);
	m_pHealthComp.lock()->set_CrntHP(BUILDING_MAX_HP);

	// 死亡時の処理を登録
	m_pHealthComp.lock()->RegisterOnDead([this]() {
		m_IsDestruction = true;	// 破壊されたフラグをオンにする
		});

	m_IsDestruction = false;

	// ステート作成
	BuildingStateFactory::CreateBuilding(m_StateMachine, 0, renderer);
	m_StateMachine.SetStartState(BUILDING_STATE::BUILDING_STATE_IDLE);
}

//*---------------------------------------------------------------------------------------
//*【?】更新
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//* [返値]なし
//*----------------------------------------------------------------------------------------
void BuildingController::Update(RendererEngine& renderer)
{
	m_StateMachine.Update();	// ステートマシンの更新
}

//*---------------------------------------------------------------------------------------
//*【?】衝突処理
//*
//* [引数]
//* &_other : 衝突相手の情報
//* [返値]なし
//*----------------------------------------------------------------------------------------
void BuildingController::OnCollisionEnter(const class CollisionInfo& _other)
{

}

#include "pch.h"
#include "Component_TimerDestruction.h"
#include "RendererEngine.h"
#include "GameObject.h"


using namespace VECTOR3;

//*---------------------------------------------------------------------------------------
//*【?】コンストラクタ
//* [引数]
//* pOwner : オーナーオブジェクト
//* updateRank : 更新レイヤー
//*----------------------------------------------------------------------------------------
TimerDestruction::TimerDestruction( std::weak_ptr<GameObject> pOwner, int updateRank)
	: IComponent(pOwner, updateRank),
	m_LifeTime(10.0f),	
	m_Counter(0.0f)
{
	this->set_Tag("TimerDestruction");
}


//*---------------------------------------------------------------------------------------
//*【?】デストラクタ
//*----------------------------------------------------------------------------------------
TimerDestruction::~TimerDestruction()
{

}

//*---------------------------------------------------------------------------------------
//*【?】更新
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//* [返値]なし
//*----------------------------------------------------------------------------------------
void TimerDestruction::Update(RendererEngine& renderer)
{
	m_Counter += Master::m_pTimeManager->get_DeltaTime();

	if(m_Counter >= m_LifeTime)
	{
		// 親オブジェクトの削除フラグを立てる
		m_pOwner.lock()->set_StatusFlag(OBJECT_STATUS_BITFLAG::IS_DELETE);
	}
}

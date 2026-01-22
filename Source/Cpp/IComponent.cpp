#include "pch.h"
#include "IComponent.h"
#include "GameObject.h"


//*---------------------------------------------------------------------------------------
//* @:IComponent Class 
//*【?】コンストラクタ
//* 引数：1.オーナーオブジェクト
//* 引数：2.更新レイヤー
//*----------------------------------------------------------------------------------------
IComponent::IComponent(std::weak_ptr<GameObject> pOwner, int updateRank ):
    m_pOwner(pOwner),
    m_UpdateRank(updateRank)
{
    this->set_StatusFlag(OBJECT_STATUS_BITFLAG::IS_ACTIVE);
}


//*---------------------------------------------------------------------------------------
//* @:IComponent Class 
//*【?】デストラクタ
//*----------------------------------------------------------------------------------------
IComponent::~IComponent()
{

}








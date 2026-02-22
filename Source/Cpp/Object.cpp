#include "pch.h"
#include "Object.h"
#include "IComponent.h"

//*---------------------------------------------------------------------------------------
//* @:Object Class 
//*【?】コンストラクタ
//* 引数：なし
//*----------------------------------------------------------------------------------------
Object::Object() :
	m_Tag("None"),
	m_LayerRank(0),
	m_ObjectStatusBitFlag()
{
	// Transformは全てのオブジェクトに共通するコンポーネントとするため、生成時に追加する
	// (Unity風に)
	//this->add_Component<MyTransform>();
	//m_pTransform = this->get_Component<MyTransform>();
}



//*---------------------------------------------------------------------------------------
//* @:Object Class 
//*【?】フラグを立てる
//* 引数：立てるフラグの指定
//* 戻値：void
//*----------------------------------------------------------------------------------------
void Object::set_StatusFlag(OBJECT_STATUS_BITFLAG flag)
{
	m_ObjectStatusBitFlag |= (unsigned int)flag;
}


//*---------------------------------------------------------------------------------------
//* @:Object Class 
//*【?】フラグを降ろす
//* 引数：降ろすフラグの指定
//* 戻値：void
//*----------------------------------------------------------------------------------------
void Object::clear_StatusFlag(OBJECT_STATUS_BITFLAG flag)
{
	m_ObjectStatusBitFlag &= ~((unsigned int)flag);
}


//*---------------------------------------------------------------------------------------
//* @:Object Class 
//*【?】フラグが立っているかどうか
//* 引数：立っているか調べるフラグの指定
//* 戻値：bool
//*----------------------------------------------------------------------------------------
bool Object::get_IsStatusFlag(OBJECT_STATUS_BITFLAG flag)
{
	return (m_ObjectStatusBitFlag & (unsigned int)flag) != 0;
}


//*---------------------------------------------------------------------------------------
//* @:Object Class 
//*【?】フラグを反転させる
//* 引数：反転させるフラグの指定
//* 戻値：void
//*----------------------------------------------------------------------------------------
void Object::toggle_StatusFlag(OBJECT_STATUS_BITFLAG flag)
{
	m_ObjectStatusBitFlag ^= (unsigned int)flag;
}




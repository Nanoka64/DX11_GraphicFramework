#include "pch.h"
#include "Component_BillBoard.h"
#include "RendererEngine.h"

using namespace GIGA_Engine;

//*---------------------------------------------------------------------------------------
//*【?】コンストラクタ
//* [引数]
//* std::weak_ptr<class GameObject> : オーナーオブジェクト
//* updateRank : 更新レイヤー
//*----------------------------------------------------------------------------------------
BillBoard::BillBoard(std::weak_ptr<class GameObject> pOwner, int updateRank) : IComponent(pOwner, updateRank),
m_FixedAxisBitFlag()
{
	// TODO:ビットフラグ、関数内で演算が効かない問題を直す
	BitFlag::SetFlag((int)FIXED_AXIS_BITFLAG_X, (int&)m_FixedAxisBitFlag);
	BitFlag::SetFlag((int)FIXED_AXIS_BITFLAG_Y, (int&)m_FixedAxisBitFlag);
	BitFlag::SetFlag((int)FIXED_AXIS_BITFLAG_Z, (int&)m_FixedAxisBitFlag);
}

//*---------------------------------------------------------------------------------------
//*【?】デストラクタ
//*----------------------------------------------------------------------------------------
BillBoard::~BillBoard()
{

}

void BillBoard::Init(RendererEngine& renderer)
{

}

void BillBoard::Update(RendererEngine& renderer)
{

}

void BillBoard::Draw(RendererEngine& renderer)
{

}
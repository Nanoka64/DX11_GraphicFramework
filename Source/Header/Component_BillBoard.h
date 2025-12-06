#pragma once
#include "IComponent.h"

/// <summary>
/// 固定軸指定ビットフラグ
/// </summary>
enum FIXED_AXIS_BITFLAG : unsigned
{
	FIXED_AXIS_BITFLAG_X = 1 << 0,
	FIXED_AXIS_BITFLAG_Y = 1 << 1,
	FIXED_AXIS_BITFLAG_Z = 1 << 2,
};


class BillBoard : public IComponent
{
private:
	FIXED_AXIS_BITFLAG m_FixedAxisBitFlag;	// 固定軸を決めるためのフラグ（Y軸固定など）


public:
	BillBoard(std::weak_ptr<class GameObject> pOwner, int updateRank = 100);
	~BillBoard();

	void Init(RendererEngine& renderer)override;
	void Update(RendererEngine& renderer)override;
	void Draw(RendererEngine& renderer)override;
};


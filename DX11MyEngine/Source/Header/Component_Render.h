#pragma once
#include "IComponent.h"

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:Render Class --- */
//
//  ★継承：IComponent ★
//
// 【?】描画を行うコンポーネントの基底クラス
//		
// ***************************************************************************************
class Render : public IComponent
{
private:

public:
	Render(std::weak_ptr<GameObject> pOwner, int updateRank);
	virtual ~Render() = default;
};


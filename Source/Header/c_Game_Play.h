#pragma once
#include "IState.h"

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:c_Game_Play Class --- */
//
// 【?】ゲームシーンの子ステート
//		ゲームプレイ
//
// ***************************************************************************************
class c_Game_Play : public IState<SceneManager>
{
private:


public:
	void OnEnter(SceneManager * pOwner) override;
	void OnExit(SceneManager* pOwner)override;
	int Update(SceneManager* pOwner)override;
	void Draw(SceneManager* pOwner)override;
};



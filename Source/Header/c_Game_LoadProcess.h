#pragma once
#include "IState.h"

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:c_Game_LoadProcess Class --- */
//
// 【?】ゲームシーンの子ステート
//		ゲームに必要なもののロード処理
//
// ***************************************************************************************
class c_Game_LoadProcess : public IState<SceneManager>
{
private:


public:
	void OnEnter(SceneManager * pOwner) override;
	void OnExit(SceneManager *pOwner)override;
	int Update(SceneManager *pOwner)override;
	void Draw(SceneManager *pOwner)override;
};


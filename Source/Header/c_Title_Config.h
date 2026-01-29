#pragma once
#include "IState.h"

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:c_Title_Config Class --- */
//
// 【?】タイトルシーンの子ステート
//		設定をする
//
// ***************************************************************************************
class c_Title_Config : public IState<SceneManager>
{
private:


public:
	void OnEnter(SceneManager *pOwner) override;
	void OnExit(SceneManager *pOwner)override;
	int Update(SceneManager *pOwner)override;
	void Draw(SceneManager *pOwner)override;
};

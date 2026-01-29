#pragma once
#include "IState.h"

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:c_Title_MissionSelect Class --- */
//
// 【?】タイトルシーンの子ステート
//		ミッション選択をする
//
// ***************************************************************************************
class c_Title_MissionSelect : public IState<SceneManager>
{
private:


public:
	void OnEnter(SceneManager *pOwner) override;
	void OnExit(SceneManager *pOwner)override;
	int Update(SceneManager *pOwner)override;
	void Draw(SceneManager *pOwner)override;
};


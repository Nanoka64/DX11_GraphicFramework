#pragma once
#include "CompositeState.h"



// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:Root_ResultSceneState Class --- */
//
// 【?】リザルトシーン親ステート
//
// ***************************************************************************************
class Root_ResultSceneState : public CompositeState<SceneManager>
{
private:

public:
	void OnEnter(SceneManager *pOwner) override;
	void OnExit(SceneManager *pOwner)override;
	int Update(SceneManager *pOwner)override;
	void Draw(SceneManager *pOwner)override;
};
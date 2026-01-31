#pragma once
#include "IState.h"

/// <summary>
/// ミッション選択中のどの状態か
/// </summary>
enum class MISSION_SELECT_PHASE
{
	SELECT_MISSION,			// ミッション選択
	SELECT_DIFFCULTY,		// 難易度選択
};

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


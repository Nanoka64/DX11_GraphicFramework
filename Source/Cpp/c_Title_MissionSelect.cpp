#include "pch.h"
#include "c_Title_MissionSelect.h"
#include "Root_TitleSceneState.h"
#include "GameObjectManager.h"
#include "ResourceManager.h"
#include "InputFactory.h"
#include "SceneStateEnums.h"

using namespace SceneStateEnums;
//*---------------------------------------------------------------------------------------
//* @:c_Title_MissionSelect Class 
//*【?】開始
//* 引数：1.SceneManager
//* 返値：void
//*----------------------------------------------------------------------------------------
void c_Title_MissionSelect::OnEnter(SceneManager *pOwner)
{

}


//*---------------------------------------------------------------------------------------
//* @:c_Title_MissionSelect Class 
//*【?】終了
//* 引数：1.SceneManager
//* 返値：void
//*----------------------------------------------------------------------------------------
void c_Title_MissionSelect::OnExit(SceneManager *pOwner)
{

}


//*---------------------------------------------------------------------------------------
//* @:c_Title_MissionSelect Class 
//*【?】更新
//* 引数：1.SceneManager
//* 返値：void
//*----------------------------------------------------------------------------------------
int c_Title_MissionSelect::Update(SceneManager *pOwner)
{
	// ゲームシーンへ
	if (GetInputDown(GAME_CONFIG::MOVE_JUMP))
	{
		return c_TITLE::c_GO_GAME_SCENE;
	}

	return c_TITLE::c_TITLE_MISSION_SELECT;
}


//*---------------------------------------------------------------------------------------
//* @:c_Title_MissionSelect Class 
//*【?】描画
//* 引数：1.SceneManager
//* 返値：void
//*----------------------------------------------------------------------------------------
void c_Title_MissionSelect::Draw(SceneManager *pOwner)
{

}

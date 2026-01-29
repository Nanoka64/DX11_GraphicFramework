#include "pch.h"
#include "c_Game_LoadProcess.h"
#include "ResourceManager.h"
#include "SceneStateEnums.h"
#include "InputFactory.h"

using namespace SceneStateEnums;
//*---------------------------------------------------------------------------------------
//* @:c_Game_LoadProcess Class 
//*【?】開始
//* 引数：1.SceneManager
//* 返値：void
//*----------------------------------------------------------------------------------------
void c_Game_LoadProcess::OnEnter(SceneManager *pOwner)
{

}


//*---------------------------------------------------------------------------------------
//* @:c_Game_LoadProcess Class 
//*【?】終了
//* 引数：1.SceneManager
//* 返値：void
//*----------------------------------------------------------------------------------------
void c_Game_LoadProcess::OnExit(SceneManager *pOwner)
{

}


//*---------------------------------------------------------------------------------------
//* @:c_Game_LoadProcess Class 
//*【?】更新
//* 引数：1.SceneManager
//* 返値：void
//*----------------------------------------------------------------------------------------
int c_Game_LoadProcess::Update(SceneManager *pOwner)
{
	// リザルトシーンへ
	if (GetInputDown(GAME_CONFIG::MOVE_JUMP))
	{
		return c_GAME::c_GO_RESULT_SCENE;
	}

	return c_GAME::c_GAME_LOAD;
}


//*---------------------------------------------------------------------------------------
//* @:c_Game_LoadProcess Class 
//*【?】描画
//* 引数：1.SceneManager
//* 返値：void
//*----------------------------------------------------------------------------------------
void c_Game_LoadProcess::Draw(SceneManager *pOwner)
{

}

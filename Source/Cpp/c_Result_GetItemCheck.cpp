#include "pch.h"
#include "c_Result_GetItemCheck.h"
#include "ResourceManager.h"
#include "SceneStateEnums.h"
#include "InputFactory.h"

using namespace SceneStateEnums;
//*---------------------------------------------------------------------------------------
//* @:c_Result_GetItemCheck Class 
//*【?】開始
//* 引数：1.SceneManager
//* 返値：void
//*----------------------------------------------------------------------------------------
void c_Result_GetItemCheck::OnEnter(SceneManager *pOwner)
{

}


//*---------------------------------------------------------------------------------------
//* @:c_Result_GetItemCheck Class 
//*【?】終了
//* 引数：1.SceneManager
//* 返値：void
//*----------------------------------------------------------------------------------------
void c_Result_GetItemCheck::OnExit(SceneManager *pOwner)
{

}


//*---------------------------------------------------------------------------------------
//* @:c_Result_GetItemCheck Class 
//*【?】更新
//* 引数：1.SceneManager
//* 返値：void
//*----------------------------------------------------------------------------------------
int c_Result_GetItemCheck::Update(SceneManager *pOwner)
{
	// リザルトシーンへ
	if (GetInputDown(GAME_CONFIG::MOVE_JUMP))
	{
		return c_RESULT::c_GO_TITLE_SCENE;
	}

	return c_RESULT::c_GETITEM_CHECK;
}


//*---------------------------------------------------------------------------------------
//* @:c_Result_GetItemCheck Class 
//*【?】描画
//* 引数：1.SceneManager
//* 返値：void
//*----------------------------------------------------------------------------------------
void c_Result_GetItemCheck::Draw(SceneManager *pOwner)
{

}

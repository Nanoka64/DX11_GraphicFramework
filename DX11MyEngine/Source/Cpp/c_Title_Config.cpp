#include "pch.h"
#include "c_Title_Config.h"
#include "Root_TitleSceneState.h"
#include "GameObjectManager.h"
#include "ResourceManager.h"
#include "SceneStateEnums.h"
#include "InputFactory.h"

using namespace SceneStateEnums;
//*---------------------------------------------------------------------------------------
//* @:c_Title_Config Class 
//*【?】開始
//* 引数：1.SceneManager
//* 返値：void
//*----------------------------------------------------------------------------------------
void c_Title_Config::OnEnter(SceneManager *pOwner)
{

}


//*---------------------------------------------------------------------------------------
//* @:c_Title_Config Class 
//*【?】終了
//* 引数：1.SceneManager
//* 返値：void
//*----------------------------------------------------------------------------------------
void c_Title_Config::OnExit(SceneManager *pOwner)
{

}


//*---------------------------------------------------------------------------------------
//* @:c_Title_Config Class 
//*【?】更新
//* 引数：1.SceneManager
//* 返値：void
//*----------------------------------------------------------------------------------------
int c_Title_Config::Update(SceneManager *pOwner)
{
	// 右クリックでメインメニューへ戻る
	if (GetMouseClickDown(MOUSE_BUTTON_STATE::RIGHT))
	{
		return c_TITLE::c_TITLE_MAIN_MENU;
	}

	return c_TITLE::c_TITLE_CONFIG;
}


//*---------------------------------------------------------------------------------------
//* @:c_Title_Config Class 
//*【?】描画
//* 引数：1.SceneManager
//* 返値：void
//*----------------------------------------------------------------------------------------
void c_Title_Config::Draw(SceneManager *pOwner)
{
	Master::m_pDirectWriteManager->DrawString("☆設定", VECTOR2::VEC2(40.0f, 500.0f), "White_40_STD");
	Master::m_pDirectWriteManager->DrawString("まだ何もありません", VECTOR2::VEC2(600.0f, 500.0f), "White_40_STD");
}

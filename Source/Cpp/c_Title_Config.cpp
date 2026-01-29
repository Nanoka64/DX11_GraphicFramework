#include "pch.h"
#include "c_Title_Config.h"
#include "Root_TitleSceneState.h"
#include "GameObjectManager.h"
#include "ResourceManager.h"
#include "SceneStateEnums.h"

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

}

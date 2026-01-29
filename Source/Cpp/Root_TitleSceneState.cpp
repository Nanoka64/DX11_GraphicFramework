#include "pch.h"
#include "Root_TitleSceneState.h"
#include "GameObjectManager.h"
#include "ResourceManager.h"
#include "SceneStateEnums.h"

using namespace SceneStateEnums;
using namespace Tool::UV;


//*---------------------------------------------------------------------------------------
//* @:TitleSceneState Class 
//*【?】開始
//* 引数：1.SceneManager
//* 返値：void
//*----------------------------------------------------------------------------------------
void Root_TitleSceneState::OnEnter(SceneManager* pOwner)
{
	this->SetInitChildState(pOwner, c_TITLE::c_TITLE_LOAD_PROCESS);
}


//*---------------------------------------------------------------------------------------
//* @:TitleSceneState Class 
//*【?】終了
//* 引数：1.SceneManager
//* 返値：void
//*----------------------------------------------------------------------------------------
void Root_TitleSceneState::OnExit(SceneManager* pOwner)
{
    //Master::m_pGameObjectManager->clear_AllObject();
}


//*---------------------------------------------------------------------------------------
//* @:Root_TitleSceneState Class 
//*【?】更新
//* 引数：1.SceneManager
//* 返値：void
//*----------------------------------------------------------------------------------------
int Root_TitleSceneState::Update(SceneManager* pOwner)
{
	if (m_CrntChildStateID == -1)return -1;

	int newState = m_pChildStateMap[m_CrntChildStateID]->Update(pOwner);

	// ゲームシーンへ
	if (newState == c_TITLE::c_GO_GAME_SCENE)
	{
		return SCENE_STATE::SCENE_STATE_GAME;
	}

	// ステートが変更されていたら切り替え処理を行う
	// ※ c_GO_GAME_SCENE以外
	if (newState != m_CrntChildStateID)
	{
		ChangeChildState(pOwner, newState);
	}

	return SCENE_STATE::SCENE_STATE_TITLE;
}


//*---------------------------------------------------------------------------------------
//* @:Root_TitleSceneState Class 
//*【?】描画
//* 引数：1.SceneManager
//* 返値：void
//*----------------------------------------------------------------------------------------
void Root_TitleSceneState::Draw(SceneManager* pOwner)
{
	if (m_CrntChildStateID == -1)return;

	Master::m_pDirectWriteManager->DrawString("☆タイトルです",VECTOR2::VEC2(940, 540));

	m_pChildStateMap[m_CrntChildStateID]->Draw(pOwner);
}

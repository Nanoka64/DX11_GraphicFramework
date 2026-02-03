#include "pch.h"
#include "Root_GameSceneState.h"
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
void Root_GameSceneState::OnEnter(SceneManager *pOwner)
{
	this->SetInitChildState(pOwner,c_GAME::c_GAME_LOAD);
}


//*---------------------------------------------------------------------------------------
//* @:TitleSceneState Class 
//*【?】終了
//* 引数：1.SceneManager
//* 返値：void
//*----------------------------------------------------------------------------------------
void Root_GameSceneState::OnExit(SceneManager *pOwner)
{
	//Master::m_pGameObjectManager->clear_AllObject();
}


//*---------------------------------------------------------------------------------------
//* @:Root_GameSceneState Class 
//*【?】更新
//* 引数：1.SceneManager
//* 返値：void
//*----------------------------------------------------------------------------------------
int Root_GameSceneState::Update(SceneManager *pOwner)
{
	if (m_CrntChildStateID == -1)return -1;

	// 子ステートの更新
	int newState = m_pChildStateMap[m_CrntChildStateID]->Update(pOwner);

	// リザルトシーンへ
	if (newState == c_GAME::c_GO_RESULT_SCENE)
	{
		m_pChildStateMap[m_CrntChildStateID]->OnExit(pOwner);
		return  SCENE_STATE::SCENE_STATE_RESULT;
	}

	// ステートが変更されていたら切り替え処理を行う
	if (newState != m_CrntChildStateID)
	{
		ChangeChildState(pOwner, newState);
	}

	return SCENE_STATE::SCENE_STATE_GAME;
}


//*---------------------------------------------------------------------------------------
//* @:Root_GameSceneState Class 
//*【?】描画
//* 引数：1.SceneManager
//* 返値：void
//*----------------------------------------------------------------------------------------
void Root_GameSceneState::Draw(SceneManager *pOwner)
{
	if (m_CrntChildStateID == -1)return;

	//Master::m_pDirectWriteManager->DrawString("☆ゲームです", VECTOR2::VEC2(940, 540));

	m_pChildStateMap[m_CrntChildStateID]->Draw(pOwner);
}

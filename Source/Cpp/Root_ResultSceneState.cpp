#include "pch.h"
#include "Root_ResultSceneState.h"
#include "GameObjectManager.h"
#include "ResourceManager.h"
#include "SceneStateEnums.h"
#include "InputFactory.h"

using namespace SceneStateEnums;
using namespace Tool::UV;


//*---------------------------------------------------------------------------------------
//* @:TitleSceneState Class 
//*【?】開始
//* 引数：1.SceneManager
//* 返値：void
//*----------------------------------------------------------------------------------------
void Root_ResultSceneState::OnEnter(SceneManager *pOwner)
{
	this->SetInitChildState(pOwner, c_RESULT::c_GETITEM_CHECK);
}


//*---------------------------------------------------------------------------------------
//* @:TitleSceneState Class 
//*【?】終了
//* 引数：1.SceneManager
//* 返値：void
//*----------------------------------------------------------------------------------------
void Root_ResultSceneState::OnExit(SceneManager *pOwner)
{
	//Master::m_pGameObjectManager->clear_AllObject();
}


//*---------------------------------------------------------------------------------------
//* @:Root_ResultSceneState Class 
//*【?】更新
//* 引数：1.SceneManager
//* 返値：void
//*----------------------------------------------------------------------------------------
int Root_ResultSceneState::Update(SceneManager *pOwner)
{
	if (m_CrntChildStateID == -1)return -1;

	int newState = m_pChildStateMap[m_CrntChildStateID]->Update(pOwner);

	// タイトルシーンへ
	if (newState == c_RESULT::c_GO_TITLE_SCENE)
	{
		m_pChildStateMap[m_CrntChildStateID]->OnExit(pOwner);
		return SCENE_STATE::SCENE_STATE_TITLE;
	}

	// ステートが変更されていたら切り替え処理を行う
	// ※ c_GO_TITLE_SCENE以外
	if (newState != m_CrntChildStateID)
	{
		ChangeChildState(pOwner, newState);
	}

	return SCENE_STATE::SCENE_STATE_RESULT;
}


//*---------------------------------------------------------------------------------------
//* @:Root_ResultSceneState Class 
//*【?】描画
//* 引数：1.SceneManager
//* 返値：void
//*----------------------------------------------------------------------------------------
void Root_ResultSceneState::Draw(SceneManager *pOwner)
{
	if (m_CrntChildStateID == -1)return;
	Master::m_pDirectWriteManager->DrawString("☆リザルトです", VECTOR2::VEC2(940, 540));


	m_pChildStateMap[m_CrntChildStateID]->Draw(pOwner);
}

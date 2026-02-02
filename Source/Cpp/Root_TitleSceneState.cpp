#include "pch.h"
#include "Root_TitleSceneState.h"
#include "GameObjectManager.h"
#include "ResourceManager.h"
#include "SceneStateEnums.h"
#include "GameObject.h"
#include "Component_SpriteRenderer.h"
#include "InputFactory.h"

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
	//char filename01[] = "Resource/Sound/BGM/battle-dark.mp3";
	//Master::m_pSoundManager->LoadXA2Sound(filename01);

	// 最初はロード
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

	// タイトル用のスプライトをオフに
	auto obj = Master::m_pGameObjectManager->get_ObjectByTag("TitleBack_Sp");
	if (obj)obj->clear_StatusFlag(OBJECT_STATUS_BITFLAG::IS_ACTIVE);
	obj = Master::m_pGameObjectManager->get_ObjectByTag("TitleLogo_Sp");
	if (obj)obj->clear_StatusFlag(OBJECT_STATUS_BITFLAG::IS_ACTIVE);
	obj = Master::m_pGameObjectManager->get_ObjectByTag("MenuItemBack_Sp1");
	if (obj)obj->clear_StatusFlag(OBJECT_STATUS_BITFLAG::IS_ACTIVE);
	obj = Master::m_pGameObjectManager->get_ObjectByTag("MenuItemBack_Sp2");
	if (obj)obj->clear_StatusFlag(OBJECT_STATUS_BITFLAG::IS_ACTIVE);
	obj = Master::m_pGameObjectManager->get_ObjectByTag("MenuItemBack_Sp3");
	if (obj)obj->clear_StatusFlag(OBJECT_STATUS_BITFLAG::IS_ACTIVE);
	obj = Master::m_pGameObjectManager->get_ObjectByTag("MenuItemBack_Sp4");
	if (obj)obj->clear_StatusFlag(OBJECT_STATUS_BITFLAG::IS_ACTIVE);
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
		m_pChildStateMap[m_CrntChildStateID]->OnExit(pOwner);
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

	//Master::m_pDirectWriteManager->DrawString("☆タイトルです",VECTOR2::VEC2(940, 540));

	// スプライト取得
	if (!m_pBackSprite)
	{
		auto obj = Master::m_pGameObjectManager->get_ObjectByTag("TitleBack_Sp");
		if (obj)
		{
			m_pBackSprite = obj->get_Component<SpriteRenderer>();
		}
	}
	else
	{
		// 背景のUVスクロール
		m_UVScroll.x += 0.001f;;
		m_pBackSprite->set_UVOffset(m_UVScroll);
	}

	m_pChildStateMap[m_CrntChildStateID]->Draw(pOwner);
}

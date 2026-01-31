#include "pch.h"
#include "SceneFactory.h"
#include "SceneStateEnums.h"
#include "RendererEngine.h"

/* タイトル */
#include "Root_TitleSceneState.h"
#include "c_Title_LoadProcess.h"
#include "c_Title_MainMenu.h"
#include "c_Title_MissionSelect.h"
#include "c_Title_SoldierSelect.h"
#include "c_Title_Config.h"

/* ゲーム */
#include "Root_GameSceneState.h"
#include "c_Game_LoadProcess.h"
#include "c_Game_Play.h"


/* リザルト */
#include "Root_ResultSceneState.h"
#include "c_Result_GetItemCheck.h"



using namespace SceneStateEnums;



//*---------------------------------------------------------------------------------------
//* @:SceneFactory Class 
//*【?】シーンステートの作成
//* 引数：1.StateMachine
//* 返値：void
//*----------------------------------------------------------------------------------------
void SceneFactory::Create(StateMachine<SceneManager> &_out, int _createScene, RendererEngine &_renderer)
{
	switch (_createScene)
	{
	case SCENE_STATE_TITLE:
		CreateTitleScene(_out, _renderer);
		break;
	case SCENE_STATE_GAME:
		CreateGameScene(_out, _renderer);
		break;
	case SCENE_STATE_RESULT:
		CreateResultScene(_out, _renderer);
		break;
	default:
		break;
	};

}


//*---------------------------------------------------------------------------------------
//*【?】タイトルシーンの作成
//*
//* [引数]
//* &_out : 出力先
//* &renderer : 描画エンジンの参照
//*
//* [返値]
//* true : 成功
//* false : 失敗
//*----------------------------------------------------------------------------------------
void SceneFactory::CreateTitleScene(StateMachine<SceneManager> &_out, RendererEngine &_renderer)
{
	// ルートシーン作成
	std::shared_ptr<Root_TitleSceneState> pTitleScene = std::make_shared<Root_TitleSceneState>();

	//描画インターフェイス設定
	// add_childの中で子シーンにもセットしてるのでaddする前に呼んで！
	pTitleScene->set_Renderer(&_renderer);

	// ********************************************************************************
	// シーンの子ステート作成
	// ********************************************************************************
	std::shared_ptr<c_Title_LoadProcess> c_LoadProcess = std::make_shared<c_Title_LoadProcess>();		// ロード処理
	std::shared_ptr<c_Title_MainMenu> c_MainMenu = std::make_shared<c_Title_MainMenu>();		// ロード処理
	std::shared_ptr<c_Title_MissionSelect> c_MissionSelect = std::make_shared<c_Title_MissionSelect>();	// ミッション選択
	std::shared_ptr<c_Title_SoldierSelect> c_SoldierSelect = std::make_shared<c_Title_SoldierSelect>();	// 兵科選択
	std::shared_ptr<c_Title_Config> c_Config = std::make_shared<c_Title_Config>();						// 設定

	// 親を設定
	c_LoadProcess->set_Parent(pTitleScene);
	c_MainMenu->set_Parent(pTitleScene);
	c_MissionSelect->set_Parent(pTitleScene);
	c_SoldierSelect->set_Parent(pTitleScene);
	c_Config->set_Parent(pTitleScene);

	// 子を登録
	pTitleScene->add_Child(c_TITLE::c_TITLE_LOAD_PROCESS, (c_LoadProcess));
	pTitleScene->add_Child(c_TITLE::c_TITLE_MAIN_MENU, (c_MainMenu));
	pTitleScene->add_Child(c_TITLE::c_TITLE_MISSION_SELECT, (c_MissionSelect));
	pTitleScene->add_Child(c_TITLE::c_TITLE_SOLDIER_SELECT, (c_SoldierSelect));
	pTitleScene->add_Child(c_TITLE::c_TITLE_CONFIG, (c_Config));

	// タイトルをステートマシンに登録
	_out.RegisterState(SCENE_STATE::SCENE_STATE_TITLE, std::move(pTitleScene));
}


//*---------------------------------------------------------------------------------------
//*【?】ゲームシーンの作成
//*
//* [引数]
//* &_out : 出力先
//* &renderer : 描画エンジンの参照
//*
//* [返値]
//* true : 成功
//* false : 失敗
//*----------------------------------------------------------------------------------------
void SceneFactory::CreateGameScene(StateMachine<SceneManager> &_out, RendererEngine &_renderer)
{
	// ルートシーン作成
	std::shared_ptr<Root_GameSceneState> pGameScene = std::make_shared<Root_GameSceneState>();

	//描画インターフェイス設定
	// add_childの中で子シーンにもセットしてるのでaddする前に呼んで！
	pGameScene->set_Renderer(&_renderer);

	// ********************************************************************************
	// シーンの子ステート作成
	// ********************************************************************************
	std::shared_ptr<c_Game_LoadProcess> c_LoadProcess = std::make_shared<c_Game_LoadProcess>();	// ミッション選択
	std::shared_ptr<c_Game_Play> c_GamePlay = std::make_shared<c_Game_Play>();	// ミッション選択

	// 親を設定
	c_LoadProcess->set_Parent(pGameScene);
	c_GamePlay->set_Parent(pGameScene);

	// 子を登録
	pGameScene->add_Child(c_GAME::c_GAME_LOAD, (c_LoadProcess));
	pGameScene->add_Child(c_GAME::c_GAME_PLAY, (c_GamePlay));

	// タイトルをステートマシンに登録
	_out.RegisterState(SCENE_STATE::SCENE_STATE_GAME, std::move(pGameScene));
}


//*---------------------------------------------------------------------------------------
//*【?】リザルトシーンの作成
//*
//* [引数]
//* &_out : 出力先
//* &renderer : 描画エンジンの参照
//*
//* [返値]
//* true : 成功
//* false : 失敗
//*----------------------------------------------------------------------------------------
void SceneFactory::CreateResultScene(StateMachine<SceneManager> &_out, RendererEngine &_renderer)
{
	// ルートシーン作成
	std::shared_ptr<Root_ResultSceneState> pRsultScene = std::make_shared<Root_ResultSceneState>();

	//描画インターフェイス設定
	// add_childの中で子シーンにもセットしてるのでaddする前に呼んで！
	pRsultScene->set_Renderer(&_renderer);

	// ********************************************************************************
	// シーンの子ステート作成
	// ********************************************************************************
	std::shared_ptr<c_Result_GetItemCheck> c_GetItemCheck = std::make_shared<c_Result_GetItemCheck>();	// 入手したアイテムの確認

	// 親を設定
	c_GetItemCheck->set_Parent(pRsultScene);

	// 子を登録
	pRsultScene->add_Child(c_RESULT::c_GETITEM_CHECK, (c_GetItemCheck));

	// タイトルをステートマシンに登録
	_out.RegisterState(SCENE_STATE::SCENE_STATE_RESULT, std::move(pRsultScene));
}

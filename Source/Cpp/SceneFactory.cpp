#include "pch.h"
#include "SceneFactory.h"
#include "TitleSceneState.h"
#include "SceneStateEnums.h"
#include "RendererEngine.h"

using namespace SceneStateEnums;



//*---------------------------------------------------------------------------------------
//* @:SceneFactory Class 
//*【?】シーンステートの作成
//* 引数：1.StateMachine
//* 返値：void
//*----------------------------------------------------------------------------------------
void SceneFactory::Create(StateMachine<SceneManager>& out, RendererEngine& renderer)
{
	// タイトル作成
	std::unique_ptr<TitleSceneState> pTitleScene = std::make_unique<TitleSceneState>();
	
	//描画インターフェイス設定
	// add_childの中で子シーンにもセットしてるのでaddする前に呼んで！
	pTitleScene->set_Renderer(&renderer);

	// 子を登録
	pTitleScene->add_Child(TITLE_CHILD_TEST, std::make_unique<title_TestState>());

	// ステートマシンに登録
	out.RegisterState(TITLE, std::move(pTitleScene));
}
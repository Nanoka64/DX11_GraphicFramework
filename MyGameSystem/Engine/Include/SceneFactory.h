#pragma once
#include "StateMachine.h"


// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:SceneFactory Class --- */
//
// 【?】シーンステートの作成を行う
//
// ***************************************************************************************
class SceneFactory
{
private:

public:
	/// <summary>
	/// シーンステートを作成して挿入する
	/// </summary>
	/// <param name="_out">出力先</param>
	/// <param name="_createScene">作成するシーン</param>
	/// <param name="_renderer">描画エンジンの参照</param>
	static void Create(StateMachine<SceneManager>& _out,int _createScene,RendererEngine& _renderer);

private:
	/// <summary>
	/// タイトルの作成
	/// </summary>
	/// <param name="_out">出力先</param>
	/// <param name="_renderer">描画エンジンの参照</param>
	static void CreateTitleScene(StateMachine<SceneManager> &_out, RendererEngine &_renderer);

	/// <summary>
	/// ゲームシーンの作成
	/// </summary>
	/// <param name="_out">出力先</param>
	/// <param name="_renderer">描画エンジンの参照</param>
	static void CreateGameScene(StateMachine<SceneManager> &_out, RendererEngine &_renderer);

	/// <summary>
	/// リザルトシーンの作成
	/// </summary>
	/// <param name="_out">出力先</param>
	/// <param name="_renderer">描画エンジンの参照</param>
	static void CreateResultScene(StateMachine<SceneManager> &_out, RendererEngine &_renderer);

};


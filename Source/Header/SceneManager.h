#pragma once
//--------------------------------------------------------------------------------------
//      * Includes *
//--------------------------------------------------------------------------------------
#include "TRIANGLE.h"
#include "Model.h"
#include "RootSceneState.h"
#include "StateMachine.h"

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:SceneManager Class --- */
//
// 【?】シーンの管理
//
// ***************************************************************************************
class SceneManager
{
private:
	VERTEX3 m_testTriangle;
	std::weak_ptr<GameObject>  m_pCamera;
	//Model m_TestModel;
	//Model m_TestModel2;
	//Model m_TestModel3;
	//class Cubu* m_pCubu;
	class Cubu* m_pCubu;
	class Quad* m_pQuad;


	int m_CrntSceneState;	//現在実行しているSceneStateのID
	//std::map<int, std::shared_ptr<RootSceneState>> m_SceneStateMap;//IDとSceneStateの関連付け

	// ステートマシン
	StateMachine<SceneManager> m_StateMachine;

public:
	SceneManager();
	~SceneManager();

	bool Init(RendererManager& renderer);
	void Update(RendererManager& renderer);
	void Draw(RendererManager& renderer);
	void Term(RendererManager &renderer);
};

// シーンマネージャはタイトルシーンの中身を知る必要はない
// 逆もしかり
// タイトルの中にモード選択、設定、難易度...それらはシーンマネージャにとってどうでもいい
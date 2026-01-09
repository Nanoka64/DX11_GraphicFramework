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
	std::shared_ptr<GameObject>  m_pPlayer;
	std::shared_ptr<GameObject>  m_pCamera;

	int m_CrntSceneState;	

	// ステートマシン
	StateMachine<SceneManager> m_StateMachine;

	float m_PointLightRange;
	VECTOR3::VEC3 m_LightDir;
	VECTOR3::VEC3 m_LightPos = { 0,1,0 };

public:
	SceneManager();
	~SceneManager();

	bool Init(RendererEngine& renderer);
	void Update(RendererEngine& renderer);
	void Draw(RendererEngine& renderer);
	void Term(RendererEngine &renderer);
};

// シーンマネージャはタイトルシーンの中身を知る必要はない
// 逆もしかり
// タイトルの中にモード選択、設定、難易度...それらはシーンマネージャにとってどうでもいい
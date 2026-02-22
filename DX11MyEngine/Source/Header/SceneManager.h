#pragma once
//--------------------------------------------------------------------------------------
//      * Includes *
//--------------------------------------------------------------------------------------
#include "TRIANGLE.h"
#include "Model.h"
#include "RootSceneState.h"


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
	std::shared_ptr<GameObject>  m_pCameraObj;
	std::shared_ptr<class Camera3D>  m_pCameraComp;

	int m_CrntSceneState;	

	VECTOR3::VEC3 m_V1;
	VECTOR3::VEC3 m_V2;
	VECTOR3::VEC3 m_V3;

	// ステートマシン
	StateMachine<SceneManager> m_StateMachine;

	std::shared_ptr<GameObject> m_pTempObj;
	std::shared_ptr<GameObject> m_pAnt[2];
	std::shared_ptr<GameObject> m_pBomber[3];

	bool m_IsClose;

public:
	SceneManager();
	~SceneManager();

	bool Init(RendererEngine& renderer);
	void Update(RendererEngine& renderer);
	void Draw(RendererEngine& renderer);
	void Term(RendererEngine &renderer);

	/* シーンの終了フラグ */
	void OnSceneClose() { m_IsClose = true; }
	bool get_IsSceneClose()const { return m_IsClose; }
};

// シーンマネージャはタイトルシーンの中身を知る必要はない
// 逆もしかり
// タイトルの中にモード選択、設定、難易度...それらはシーンマネージャにとってどうでもいい
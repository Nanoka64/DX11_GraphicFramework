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
	std::weak_ptr<GameObject>  m_pCamera;

	int m_CrntSceneState;	

	// ステートマシン
	StateMachine<SceneManager> m_StateMachine;

	float m_PointLightRange;
	VECTOR3::VEC3 m_LightDir;

	class DX_RenderTarget *m_pAlbedo_RT;
	class DX_RenderTarget *m_pNormal_RT;
	class DX_RenderTarget *m_pDepth_RT;
	class DX_RenderTarget *m_pSpecular_RT;

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
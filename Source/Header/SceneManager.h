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
	class DX_RenderTarget *m_pHorizontalBlur;   // 水平ブラー
	class DX_RenderTarget *m_pVerticalBlur;		// 垂直ブラー
	class DX_RenderTarget *m_pSceneFinal_RT;	// シーン最終合成用

	static const int NUM_WEIGHTS = 8;
	float m_weights[NUM_WEIGHTS];

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
#pragma once
#include "ResourceManager.h"
#include "BlendManager.h"
#include "ShaderManager.h"
#include "LightManager.h"
#include "DirectWriteManager.h"
#include "GameObjectManager.h"
#include "Debugger.h"

class Master
{
public:
	static Debugger				* m_pDebugger;			// ImGui機能ラップ
	static ShaderManager		* m_pShaderManager;		// シェーダ管理
	static LightManager			* m_pLightManager;		// ライト管理
	static DirectWriteManager	* m_pDirectWriteManager;// 文字管理
	static BlendManager			* m_pBlendManager;		// ブレンド管理
	static GameObjectManager	* m_pGameObjectManager;	// オブジェクト管理
	static ResourceManager		* m_pResourceManager;	// リソース管理
};
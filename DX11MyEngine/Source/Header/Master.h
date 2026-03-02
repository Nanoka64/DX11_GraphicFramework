#pragma once
#include "ResourceManager.h"
#include "BlendManager.h"
#include "ShaderManager.h"
#include "LightManager.h"
#include "DirectWriteManager.h"
#include "InputManager.h"
#include "GameObjectManager.h"
#include "Debugger.h"
#include "EditorManager.h"
#include "CollisionManager.h"
#include "EffectManager.h"
#include "SoundManager.h"
#include "TimeManager.h"
#include "BulletManager.h"

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
	static EditorManager		* m_pEditorManager;		// エディタ管理
	static InputManager			* m_pInputManager;		// 入力管理
	static CollisionManager		* m_pCollisionManager;	// 衝突管理
	static EffectManager		* m_pEffectManager;		// エフェクト管理
	static SoundManager			* m_pSoundManager;		// サウンド管理
	static TimeManager			* m_pTimeManager;		// 時間管理
};
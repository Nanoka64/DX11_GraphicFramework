#pragma once
//--------------------------------------------------------------------------------------
//      * Includes *
//--------------------------------------------------------------------------------------
#include "BulletManager.h"

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:GameManager Class --- */
//
// y?zƒQ[ƒ€‘S‘Ì‚ÌŠÇ—
//		DXApp‚ªÀ‘Ì‚ğ‚Â
//
// ***************************************************************************************
class GameManager
{
private:
	SceneManager* m_pSceneManager;
	static BulletManager *m_pBulletManager;		// ’eŠÇ—
	bool m_IsClose;	// I—¹ƒtƒ‰ƒO

public:
	GameManager();
	~GameManager();

	bool Init(RendererEngine& renderer);
	void Update(RendererEngine& renderer);
	void Draw(RendererEngine& renderer);
	void Term(RendererEngine &renderer);

	/* ƒQ[ƒ€‚ÌI—¹ƒtƒ‰ƒO */
	void OnGameClose() { m_IsClose = true; };
	bool get_IsGameClose()const { return m_IsClose; }

	static BulletManager *get_BulletManager() { return m_pBulletManager; }
};


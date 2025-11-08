#pragma once
//--------------------------------------------------------------------------------------
//      * Includes *
//--------------------------------------------------------------------------------------

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

public:
	GameManager();
	~GameManager();

	bool Init(RendererManager& renderer);
	void Update(RendererManager& renderer);
	void Draw(RendererManager& renderer);
	void Term(RendererManager &renderer);

};


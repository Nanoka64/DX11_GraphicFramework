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

	bool Init(RendererEngine& renderer);
	void Update(RendererEngine& renderer);
	void Draw(RendererEngine& renderer);
	void Term(RendererEngine &renderer);

};


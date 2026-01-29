#pragma once
#include "IState.h"

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:c_Title_LoadProcess Class --- */
//
// 【?】タイトルシーンの子ステート
//		一番最初に呼ばれるロード処理ステート
//
// ***************************************************************************************
class c_Title_LoadProcess : public IState<SceneManager>
{
private:
	std::shared_ptr<class SpriteRenderer>m_pLoadBackSprite;

public:
	void OnEnter(SceneManager *pOwner) override;
	void OnExit(SceneManager *pOwner)override;
	int Update(SceneManager *pOwner)override;
	void Draw(SceneManager *pOwner)override;
};

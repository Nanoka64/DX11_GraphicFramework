#pragma once
#include "CompositeState.h"


// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:Root_TitleSceneState Class --- */
//
// 【?】タイトルシーンの親ステート
//
// ***************************************************************************************
class Root_TitleSceneState : public CompositeState<SceneManager>
{
private:
	bool m_IsLoad;	// ロード完了しているか
	
	std::shared_ptr<class SpriteRenderer> m_pBackSprite;
	VECTOR2::VEC2 m_UVScroll;
public:
	void OnEnter(SceneManager* pOwner) override;
	void OnExit(SceneManager* pOwner)override;
	int Update(SceneManager* pOwner)override;
	void Draw(SceneManager* pOwner)override;
};



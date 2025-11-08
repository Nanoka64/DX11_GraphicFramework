#pragma once
#include "CompositeState.h"

// 親シーン
class TitleSceneState : public CompositeState<SceneManager>
{
private:


public:
	void OnEnter(SceneManager* pOwner) override;
	void OnExit(SceneManager* pOwner)override;
	int Update(SceneManager* pOwner)override;
	void Draw(SceneManager* pOwner)override;
};

// 子シーン
class title_TestState : public IState<SceneManager>
{
private:


public:
	void OnEnter(SceneManager* pOwner) override;
	void OnExit(SceneManager* pOwner)override;
	int Update(SceneManager* pOwner)override;
	void Draw(SceneManager* pOwner)override;
};

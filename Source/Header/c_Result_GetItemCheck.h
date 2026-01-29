#pragma once
#include "IState.h"
class c_Result_GetItemCheck : public IState<SceneManager>
{
private:


public:
	void OnEnter(SceneManager *pOwner) override;
	void OnExit(SceneManager *pOwner)override;
	int Update(SceneManager *pOwner)override;
	void Draw(SceneManager *pOwner)override;
};


#pragma once
#include "IChildSceneState.h"
class Test_ChildState_Start : public IChildSceneState
{
public:
	Test_ChildState_Start();
	~Test_ChildState_Start();

	void OnEnter(RendererManager &renderer) ;
	void OnExit(RendererManager &renderer) ;
	int Update(RendererManager &renderer) ;
	void Draw(RendererManager &renderer) ;
};


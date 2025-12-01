#pragma once
#include "IChildSceneState.h"
class Test_ChildState_Start : public IChildSceneState
{
public:
	Test_ChildState_Start();
	~Test_ChildState_Start();

	void OnEnter(RendererEngine &renderer) ;
	void OnExit(RendererEngine &renderer) ;
	int Update(RendererEngine &renderer) ;
	void Draw(RendererEngine &renderer) ;
};


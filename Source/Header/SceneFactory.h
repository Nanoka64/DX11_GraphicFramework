#pragma once
#include "StateMachine.h"


// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:SceneFactory Class --- */
//
//  ★★★シングルトン★★★
//
// 【?】シーンステートの作成を行う
//
// ***************************************************************************************
class SceneFactory
{
private:

public:
	/// <summary>
	/// シーンステートを作成して挿入する
	/// </summary>
	/// <param name="out"></param>
	static void Create(StateMachine<SceneManager>& out, RendererEngine& renderer);
};


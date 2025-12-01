#pragma once
#include "IChildSceneState.h"

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:RootSceneState Class --- */
//
// 【?】このクラスは「タイトル」や「リザルト」など、シーン構成の親として振る舞う。
//		各シーンの要素、タイトルであれば「ステージ選択」や「難易度選択」「設定」などを
//		子シーンとして登録し、ステートパターンとして機能させる。
//
// ***************************************************************************************
class RootSceneState
{
protected:
	int m_CrntChildSceneState;	//現在実行している子シーンのID
	u_int m_ChildNum;	// 子シーンの数
	std::map<int, std::shared_ptr<IChildSceneState>> m_ChildSceneStateMap;//IDと子シーンの関連付け


public:
	RootSceneState() = default;
	virtual ~RootSceneState() = default;

	int Update(RendererEngine &renderer);
	void Draw(RendererEngine &renderer);

	void RegisterChildScene(const int id, std::shared_ptr<IChildSceneState> pScene) ;
	void SetCurrentChildScene(const int id, RendererEngine &renderer) ;
};



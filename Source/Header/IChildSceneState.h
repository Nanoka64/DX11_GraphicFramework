#pragma once

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:IChildSceneState Class --- */
//
//  ★★★抽象クラス★★★
// 
// 【?】RootSceneStateが最上位の親として設計する
//		この子シーンは例えばRootSceneが「タイトル」であった場合、
//		「ステージ選択」や「装備選択」などタイトルの要素の一部として機能する。
//		「ステージ選択」の中に別の機能がある場合はそれを子シーンとして登録し、
//		入れ子構造にする。
// 
//		※ compositeパターンを使うといい感じにできるらしいので設計見直す！
//			まるぺけに載ってる
//
// ***************************************************************************************
class IChildSceneState
{
protected:
	std::shared_ptr<IChildSceneState> m_pChild;

public:
	IChildSceneState() = default;
	virtual ~IChildSceneState() = default;

	virtual void OnEnter(RendererManager &renderer) = 0;
	virtual void OnExit(RendererManager &renderer) = 0;
	virtual int Update(RendererManager &renderer) = 0;
	virtual void Draw(RendererManager &renderer) = 0;
};


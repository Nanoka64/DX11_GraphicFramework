#pragma once
#include "IComponent.h"

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:Button Class --- */
//
//  ★継承：IComponent ★
//
// 【?】Unity風のボタン
//		
// ***************************************************************************************
class Button : public IComponent
{
private:
	enum class STATE { NORMAL, HIGH_LIGHTED, PRESSED, DISABLED, NUM };	// 通常/ハイライト/クリック中/無効中
	enum class TRANSITION { COLOR, SPRITE };							// 遷移オプション 

	VECTOR4::VEC4 m_StateColor[UINT_CAST(STATE::NUM)];	// それぞれのステートごとのカラー
	STATE m_CrntState;					// 現在の状態
	float m_FadeDuration;				// ステート間の色の遷移時間
	std::function<void()> m_OnClick;	// クリックされた際の処理
	bool m_IsInteractable;				// 入力を受け付けるかどうか
	std::string m_Text;					// テキスト
	std::weak_ptr<class MyTransform> m_pMyTransform;	// 自分のトランスフォーム

public:
	Button(std::weak_ptr<GameObject> pOwner, int updateRank = 100);
	~Button();

	void Start(RendererEngine &renderer) override;		// 初期化
	void Update(RendererEngine &renderer) override;		// 更新処理
	void Draw(RendererEngine &renderer)override;		// 描画処理

	/// <summary>
	/// クリックされた際の処理を登録
	/// </summary>
	/// <param name="_onClick">クリックされた際の処理が記述してある関数</param>
	void OnClickFunc(const std::function<void()> &_onClick) { m_OnClick = _onClick; };

	/// <summary>
	/// 有効状態の設定
	/// </summary>
	/// <param name="_isIntertactable">フラグ</param>
	void set_IsInteractable(bool _isIntertactable) { m_IsInteractable = _isIntertactable; }

	/// <summary>
	/// 有効状態の取得
	/// </summary>
	/// <returns>有効フラグ</returns>
	bool get_IsInteractable()const { return m_IsInteractable; }

	/// <summary>
	/// ステート間の色の遷移時間設定
	/// </summary>
	/// <param name="_duration">遷移時間</param>
	void set_FadeDuration(float _duration) { m_FadeDuration = _duration; }

	/// <summary>
	/// ステート間の色の遷移時間取得
	/// </summary>
	/// <returns>遷移時間</returns>
	float get_FadeDuration()const { return m_FadeDuration; }

	/// <summary>
	/// ボタンに表示する文字を設定
	/// </summary>
	/// <param name="_text">表示する文字</param>
	void set_Text(const std::string &_text) { m_Text = _text; }

	/// <summary>
	/// ボタンに表示する文字の取得
	/// </summary>
	/// <returns>表示する文字</returns>
	std::string get_Text()const { return m_Text; }
};


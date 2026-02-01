#pragma once
#include "IState.h"
#include "SceneStateEnums.h"

using namespace SceneStateEnums;

/// <summary>
/// タイトル項目
/// </summary>
enum class TITLEMENU_ITEM
{
	MISSION_SELECT,	// ミッション選択
	SOLDER_SELECT,	// 兵科選択
	CONFIG,			// 設定
	EXIT,			// 終了

	NUM,
};

/// <summary>
/// メニュー項目情報
/// </summary>
struct MenuItemInfo
{
	VECTOR2::VEC2 _pos;	
	const char* _name;
	TITLEMENU_ITEM _type;
	c_TITLE _nextState;	// 選択した際の遷移先ステート
	bool _isHovered;	// マウスが上に乗っているか

	MenuItemInfo() :
		_pos(VECTOR2::VEC2()),
		_name(),
		_type(TITLEMENU_ITEM::NUM),
		_nextState(c_TITLE::c_TITLE_MAIN_MENU),
		_isHovered(false)
	{};
};

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:c_Title_MainMenu Class --- */
//
// 【?】タイトルシーンの子ステート
//		タイトル項目の選択をする
//
// ***************************************************************************************
class c_Title_MainMenu : public IState<SceneManager>
{
private:
	c_TITLE m_NextState = c_TITLE::c_TITLE_MAIN_MENU;

	MenuItemInfo m_MenuItemInfoArray[static_cast<int>(TITLEMENU_ITEM::NUM)];

	bool m_IsInit;	// 既に初期化済みか

	//static const VECTOR2::VEC2 m_MenuItemPosArray[static_cast<int>(TITLEMENU_ITEM::NUM)];
	std::shared_ptr<class Transform> m_pMenuItemBackSpriteTransform[static_cast<int>(TITLEMENU_ITEM::NUM)];



public:
	void OnEnter(SceneManager* pOwner) override;
	void OnExit(SceneManager* pOwner)override;
	int Update(SceneManager* pOwner)override;
	void Draw(SceneManager* pOwner)override;
};


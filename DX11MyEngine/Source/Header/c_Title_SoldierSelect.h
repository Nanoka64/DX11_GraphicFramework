#pragma once
#include "IState.h"
#include "SceneStateEnums.h"

/// <summary>
/// 兵科の種類
/// </summary>
enum class SOLDIER_TYPE
{
	RANGER,	// 歩兵
	WING,	// 歩兵
	AIR,	// 歩兵
	FEN,	// 歩兵

	NUM,
};

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:c_Title_SoldierSelect Class --- */
//
// 【?】タイトルシーンの子ステート
//		兵科選択をする
//
// ***************************************************************************************
class c_Title_SoldierSelect : public IState<SceneManager>
{
private:
	SOLDIER_TYPE m_CrntSelectedSoldierType;	// 現在選択している兵科
	int m_PrevHoveredMIssionItem;	// 前にマウスが乗っていた項目（SE用）
	SceneStateEnums::c_TITLE m_NextState = SceneStateEnums::c_TITLE::c_TITLE_MAIN_MENU;
	bool m_IsInit;	// 既に初期化済みか
	std::shared_ptr<class MyTransform> m_pMenuItemBackSpriteTransform[UINT_CAST(SOLDIER_TYPE::NUM)];


	/// <summary>
	/// 項目情報
	/// </summary>
	struct ItemInfo
	{
		VECTOR2::VEC2 _pos;
		const char* _name;
		bool _isHovered;	// マウスが上に乗っているか

		ItemInfo() :
			_pos(VECTOR2::VEC2()),
			_name(),
			_isHovered(false)
		{
		};
	}m_Items[UINT_CAST(SOLDIER_TYPE::NUM)];

public:
	void OnEnter(SceneManager *pOwner) override;
	void OnExit(SceneManager *pOwner)override;
	int Update(SceneManager *pOwner)override;
	void Draw(SceneManager *pOwner)override;
};


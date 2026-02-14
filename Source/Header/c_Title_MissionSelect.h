#pragma once
#include "IState.h"
#include "SceneStateEnums.h"

/// <summary>
/// ミッション選択中のどの状態か
/// </summary>
enum class MISSION_SELECT_PHASE
{
	SELECT_MISSION,			// ミッション選択
	SELECT_DIFFCULTY,		// 難易度選択
};

constexpr UINT MISSION_NUM = 1;	// ミッション数

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:c_Title_MissionSelect Class --- */
//
// 【?】タイトルシーンの子ステート
//		ミッション選択をする
//
// ***************************************************************************************
class c_Title_MissionSelect : public IState<SceneManager>
{
private:
	/// <summary>
	/// ミッション項目情報
	/// 配列番号＋１をそのままミッションナンバーにする
	/// </summary>
	struct MissionItemInfo
	{
		VECTOR2::VEC2 _pos;
		const char* _name;
		bool _isHovered;	// マウスが上に乗っているか

		MissionItemInfo() :
			_pos(VECTOR2::VEC2()),
			_name(),
			_isHovered(false)
		{
		};
	}m_MissionItems[1];


	SceneStateEnums:: c_TITLE m_NextState = SceneStateEnums::c_TITLE::c_TITLE_MAIN_MENU;
	int m_PrevHoveredMIssionItem;	// 前にマウスが乗っていた項目（SE用）
	bool m_IsInit;	// 既に初期化済みか
	
	std::shared_ptr<class Transform> m_pMenuItemBackSpriteTransform[MISSION_NUM];

public:
	void OnEnter(SceneManager *pOwner) override;
	void OnExit(SceneManager *pOwner)override;
	int Update(SceneManager *pOwner)override;
	void Draw(SceneManager *pOwner)override;
};


#pragma once
#include "CompositeState.h"
#include "SceneStateEnums.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//						タイトルシーンのステートのヘッダクラス定義をまとめたヘッダ
// 
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:Root_TitleSceneState Class --- */
//
// 【?】タイトルシーンの親ステート
//
// ***************************************************************************************
class Root_TitleSceneState : public CompositeState<SceneManager>
{
private:
	bool m_IsLoad;	// ロード完了しているか

	// 緑背景
	std::weak_ptr<class SpriteRenderer> m_pBackSprite;

	// 赤いグリッド背景
	std::weak_ptr<class SpriteRenderer> m_pBackSprite2;
	std::weak_ptr<class RectTransform> m_pBackSprite2_Rect;

	// 青いグリッド背景
	std::weak_ptr<class SpriteRenderer> m_pBackSprite3;
	std::weak_ptr<class RectTransform> m_pBackSprite3_Rect;

	class GameObject* m_pTitleLogoObj = nullptr;			// タイトルロゴオブジェクト
	class GameObject* m_pTitleBackObj = nullptr;			// タイトル背景の緑のスプライトオブジェクト
	class GameObject* m_pTitleBackGridObj_Red = nullptr;	// タイトル背景のグリッドスプライトオブジェクト - 赤
	class GameObject* m_pTitleBackGridObj_Blue = nullptr;	// タイトル背景のグリッドスプライトオブジェクト - 青

	VECTOR2::VEC2 m_UVScroll;
	VECTOR2::VEC2 m_UVScroll2;
public:
	void OnEnter(SceneManager* pOwner) override;
	void OnExit(SceneManager* pOwner)override;
	int Update(SceneManager* pOwner)override;
	void Draw(SceneManager* pOwner)override;
};


// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:c_Title_LoadProcess Class --- */
//
// 【?】タイトルシーンの子ステート
//		一番最初に呼ばれるロード処理ステート
//
// ***************************************************************************************
class c_Title_LoadProcess : public IState<SceneManager>
{
private:
	static constexpr int LAYER_RANK_CAMERA = 91;	    // カメラのレイヤーランク
	static constexpr int LAYER_RANK_DIRLIGHT = 90;		// 平行ライトのレイヤーランク
	static constexpr int LAYER_RANK_PLAYER = 90;		// プレイヤーのレイヤーランク

	//std::shared_ptr<class SpriteRenderer>m_pLoadBackSprite;
	class GameObject* m_pLoadBackObj;
	bool m_IsLoad = false;	// すでにロード済みか
	bool m_IsMatLoad = false;	// すでにロード済みか
	std::shared_ptr<class Camera3D> m_pCameraComp;

public:
	void OnEnter(SceneManager* pOwner) override;
	void OnExit(SceneManager* pOwner)override;
	int Update(SceneManager* pOwner)override;
	void Draw(SceneManager* pOwner)override;
};



// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:c_Title_MainMenu Class --- */
//
// 【?】タイトルシーンの子ステート
//		メインメニュー
//
// ***************************************************************************************
class c_Title_MainMenu : public IState<SceneManager>
{
private:
	// メニュー項目の状態
	enum class MENU_STATE
	{
		NONE,
		FADE_IN,
		FADE_OUT,
	};

	/* 定数 */
	const float MOUSE_HOVERTED_ITEM_SLIDEOFFSET = 50.0f;							// マウスカーソルが項目の上に乗った際に、項目をどれくらいずらすか
	const VECTOR2::VEC2 MENU_ITEM_SIZE			= VECTOR2::VEC2(450.0f, 100.0f);	// メニュー項目のサイズ
	const VECTOR2::VEC2 MENU_ITEM_START_POS		= VECTOR2::VEC2(300.0f, 1200.0f);	// 項目の開始位置
	const float ITEM_POS_Y_BETWEEN_DIST = 120.0f;									// 項目同士のY距離
	const float FADE_IN_DURATION = 0.5f;											// フェードインにかかる時間
	const float FADE_OUT_DURATION = 0.5f;											// フェードアウトにかかる時間

	SceneStateEnums::c_TITLE m_NextState = SceneStateEnums::c_TITLE::c_TITLE_MAIN_MENU;	// 次のステート

	UtilityData::MainMenuItemInfo m_MenuItemInfoArray[static_cast<int>(UtilityData::TITLEMENU_ITEM::NUM)];				// メニュー項目情報配列
	class GameObject* m_pButtonsObjArray[static_cast<int>(UtilityData::TITLEMENU_ITEM::NUM)];							// メニュー項目オブジェクト配列
	std::weak_ptr<class ButtonUI> m_pButtonArray[static_cast<int>(UtilityData::TITLEMENU_ITEM::NUM)];					// メニュー項目のButtonUI配列  
	std::weak_ptr<class RectTransform> m_pMenuItemRectTransformArray[static_cast<int>(UtilityData::TITLEMENU_ITEM::NUM)]; // メニュー項目のRectTransform配列
	UtilityData::TITLEMENU_ITEM m_CrntSelectItem;	// 現在選択中の項目
	
	MENU_STATE m_MenuState;			// メニュー項目の状態
	float m_FadeTimer;					// フェード用のタイマー
	
	bool m_IsInit;						// 既に初期化済みか


public:
	void OnEnter(SceneManager* pOwner) override;
	void OnExit(SceneManager* pOwner)override;
	int Update(SceneManager* pOwner)override;
	void Draw(SceneManager* pOwner)override;
	void FadeInMenuItems(float _deltaTime);
	void FadeOutMenuItems(float _deltaTime);
	void Button_OnClicFunction(SceneManager* pOwner, UtilityData::TITLEMENU_ITEM _type);	// ボタンが押された際の処理
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


	/* 定数 */
	const float DECISON_TEXT_DRAW_DURATION = 2.0f;								// 装備決定時のテキストの表示時間
	const float MOUSE_HOVERTED_ITEM_SLIDEOFFSET = 50.0f;						// マウスカーソルが項目の上に乗った際に、項目をどれくらいずらすか
	const float ITEM_POS_Y_BETWEEN_DIST = 140.0f;								// 項目同士のY距離
	const VECTOR2::VEC2 SOLDIER_ITEM_SIZE = VECTOR2::VEC2(500.0f, 100.0f);		// 兵装項目のサイズ
	//const VECTOR2::VEC2 SOLDIER_ITEM_START_POS = VECTOR2::VEC2(400.0f, 500.0f);	// 兵装項目の開始位置
	const VECTOR2::VEC2 SOLDIER_ITEM_START_POS = VECTOR2::VEC2(300.0f, 0.0f);	// 兵装項目の開始位置
	const float FADE_IN_DURATION = 0.5f;										// フェードインにかかる時間
	const float FADE_OUT_DURATION = 0.5f;										// フェードアウトにかかる時間


	SceneStateEnums::c_TITLE m_NextState = SceneStateEnums::c_TITLE::c_TITLE_MAIN_MENU;	// 次のステート
	UtilityData::SOLDIER_TYPE m_CrntSelectedSoldierType;	// 現在選択している兵科
	int m_CrntSelectItem;					// 選択されている項目
	bool m_IsInit;							// 既に初期化済みか

	std::weak_ptr<class RectTransform> m_pMenuItemRectTransformArray[UINT_CAST(UtilityData::SOLDIER_TYPE::NUM)];
	std::weak_ptr<class ButtonUI> m_pButtonArray[UINT_CAST(UtilityData::SOLDIER_TYPE::NUM)];	// ボタン用
	class GameObject* m_pButtonsObjArray[UINT_CAST(UtilityData::SOLDIER_TYPE::NUM)];			// ボタンオブジェクトの配列
	class GameObject* m_pWeaponDescriptionBackSpriteObj;	//  武器説明用のスプライトオブジェクト

	float m_DecisionTextDrawTimer; // 装備決定時のテキストを表示するためのカウンター
	int m_DecisionSoldierTypeIndex; // 決定した兵科のインデックス

	UtilityData::MenuItemInfo m_ItemInfoArray[UINT_CAST(UtilityData::SOLDIER_TYPE::NUM)];	// メニュー項目の情報の配列

public:
	void OnEnter(SceneManager* pOwner) override;
	void OnExit(SceneManager* pOwner)override;
	int Update(SceneManager* pOwner)override;
	void Draw(SceneManager* pOwner)override;

private:

	/// <summary>
	/// パラメータの描画
	/// </summary>
	/// <param name="_label">ラベル名（ダメージ:とか連射速度:にあたるもの）</param>
	/// <param name="_value">値</param>
	/// <param name="_pos">位置</param>
	void DrawParam(const std::wstring& _label, const std::wstring& _value, const VECTOR2::VEC2& _pos);


	void DrawWeaponInfo(const WeaponData::GunWeaponData* weaponData, int weaponIndex, float startX, float startY);
};



static constexpr UINT MISSION_NUM = 2;	// ミッション数

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:c_Title_MissionSelect Class --- */
//
// 【?】タイトルシーンの子ステート
//		ミッション選択をする
// 
//		難易度選択の子ステートを持つ
//
// ***************************************************************************************
class c_Title_MissionSelect : public CompositeState<SceneManager>
{
private:
	/* 定数 */
	const VECTOR2::VEC2 MISSION_ITEM_SIZE = VECTOR2::VEC2(400.0f, 70.0f);		// ミッション項目のサイズ
	const VECTOR2::VEC2 MISSION_ITEM_START_POS = VECTOR2::VEC2(400.0f, 600.0f);	// ミッション項目の開始位置
	const float MOUSE_HOVERTED_ITEM_SLIDEOFFSET = 50.0f;						// マウスカーソルが項目の上に乗った際に、項目をどれくらいずらすか
	const float ITEM_POS_Y_BETWEEN_DIST = 100.0f;								// ミッション項目同士のY距離

	/// <summary>
	/// ミッション選択中のどの状態か
	/// </summary>
	enum class MISSION_SELECT_PHASE
	{
		SELECT_MISSION,			// ミッション選択
		SELECT_DIFFCULTY,		// 難易度選択
	};

	UtilityData::MenuItemInfo m_ItemInfoArray[MISSION_NUM];	// メニュー項目の情報配列
	SceneStateEnums::c_TITLE m_NextState = SceneStateEnums::c_TITLE::c_TITLE_MAIN_MENU;
	bool m_IsInit;	// 既に初期化済みか
	int m_CrntSelectItem;					// 選択されている項目

	class GameObject* m_pButtonsObjArray[MISSION_NUM];							// ミッション項目オブジェクト配列
	std::weak_ptr<class ButtonUI> m_pButtonArray[MISSION_NUM];						// ミッション項目のButtonUI配列  
	std::weak_ptr<class RectTransform> m_pMenuItemRectTransformArray[MISSION_NUM];  // ミッション項目のRectTransform配列


public:
	void OnEnter(SceneManager* pOwner) override;
	void OnExit(SceneManager* pOwner)override;
	int Update(SceneManager* pOwner)override;
	void Draw(SceneManager* pOwner)override;

	void MissionSelectButton_OnClicFunction(SceneManager* pOwner);		// ボタンが押された際の処理
};



// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:c_MissionSelect_DifficultSelect Class --- */
//
// 【?】ミッション選択の子シーン
//		難易度選択
//
// ***************************************************************************************
class c_MissionSelect_DifficultSelect : public IState<SceneManager>
{
private:

	/* 定数 */
	const VECTOR2::VEC2 ITEM_SIZE = VECTOR2::VEC2(440.0f, 80.0f);		// 項目のサイズ
	const VECTOR2::VEC2 ITEM_START_POS = VECTOR2::VEC2(900.0f, 550.0f);	// 項目の開始位置
	const float ITEM_POS_Y_BETWEEN_DIST = 90.0f;				// ミッション項目同士のY距離

	class GameObject* m_pBackSpriteObj;	// 背景スプライトオブジェクト
	std::array<class GameObject*, UINT_CAST(UtilityData::DIFFICULTY_LEVEL::NUM)> m_pButtonsObjArray;	// 難易度選択のボタン
	std::array<std::weak_ptr<class ButtonUI>, UINT_CAST(UtilityData::DIFFICULTY_LEVEL::NUM)>m_pButtonArray;	// 難易度選択のButtonUI配列  
	std::array<std::weak_ptr<class RectTransform>, UINT_CAST(UtilityData::DIFFICULTY_LEVEL::NUM)>m_pItemRectTransformArray;	// 難易度選択のButtonUI配列  
	SceneStateEnums::c_TITLE m_NextState = SceneStateEnums::c_TITLE::c_TITLE_DIFFICULT_SELECT;	// 次のステート
	UtilityData::DIFFICULTY_LEVEL m_CrntSelectItem;	// 現在選択中の項目


public:
	void OnEnter(SceneManager* pOwner) override;
	void OnExit(SceneManager* pOwner)override;
	int Update(SceneManager* pOwner)override;
	void Draw(SceneManager* pOwner)override;

	void DifficultSelectButton_OnClicFunction(SceneManager* pOwner, int _i);		// ボタンが押された際の処理
};



// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:c_Title_Config Class --- */
//
// 【?】タイトルシーンの子ステート
//		設定をする
//
// ***************************************************************************************
class c_Title_Config : public IState<SceneManager>
{
private:
	std::weak_ptr<class ConfigSetting> m_pConfigSetting;	// 設定画面用コンポーネントを持つ

public:
	void OnEnter(SceneManager* pOwner) override;
	void OnExit(SceneManager* pOwner)override;
	int Update(SceneManager* pOwner)override;
	void Draw(SceneManager* pOwner)override;

};

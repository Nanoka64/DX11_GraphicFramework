#pragma once
#include "ConstantEnemyData.h"

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:DataManager Class --- */
//
//  ★★★シングルトン★★★
//
// 【?】データ管理を行う
// 
//	※ なぜ作ったか
//		RendererEngineクラス がコンポーネントなどのエンジン、ゲーム問わず、様々な場所で参照をしデータの
//		取得などを行う中継器的な設計になってしまっている。
//		このクラスはRenderEngineの中継器的な干渉を切り離す為に作った。
//		例えば今は、RenderEngineからスクリーンの幅などを取得しているが、それをこのクラスを通して取得することで、
//		過多な干渉を減らす。
//
// ***************************************************************************************
class DataManager
{
private:
	UINT m_ScreenWidth;											// スクリーンの横幅
	UINT m_ScreenHeight;										// スクリーンの縦幅
	float m_DefaultFov;											// デフォルトのFOV
	bool m_IsDebugMode;											// デバッグ用エディタの有無のフラグ
	class RendererEngine* m_pRenderer;							// 描画エンジンのポインタ（読み取り）
	std::weak_ptr<class Camera3D> m_pCameraComponent;			// カメラコンポーネント
	int m_SelectWeaponID[2];									// 武器選択で選択した武器のID 一時的にここに置く
	UtilityData::UserConfigData m_UserConfigData;				// ユーザ設定データ
	UtilityData::DIFFICULTY_LEVEL m_SelectDifficultyLevel;		// 選択された難易度
	std::array<EnemyData::EnemyDifficultyFactor, UINT_CAST(UtilityData::DIFFICULTY_LEVEL::NUM)> m_EnemyDifficultyFactorArray;	// 難易度係数（敵ごとでは無く、一旦一括で管理）


	float m_PlayerHP;		// プレイヤーの体力
	bool m_IsCameraControl;	// カメラ制御の有無
	bool m_IsUseWeapon;		// 武器を使用するか
	bool m_IsPlayerDead;	// プレイヤーが死んだか
	bool m_IsMissionCleared;// ミッションクリアしたかどうか（リザルトで分岐）
	bool m_IsPause;			// ポーズ画面中か
	bool m_IsTitle;			// タイトルシーンか

public:
	DataManager();
	~DataManager();

	bool Init(class RendererEngine* pRenderer);
	void set_CameraComponent(std::shared_ptr<class Camera3D> _pCamera) { m_pCameraComponent = _pCamera; }	// カメラコンポーネント設定
	std::weak_ptr<class Camera3D> get_CameraComponent() { return m_pCameraComponent; }						// カメラコンポーネント取得
	VECTOR3::VEC3 get_CameraPos()const;			// カメラ座標の取得

	bool SettingsData_TitleLoadOnEnter(RendererEngine& renderer);			// タイトルロード時
	bool SettingsData_TitleLoadOnExit(RendererEngine& renderer);			// タイトルロード時
	bool SettingsData_MissionLoad(RendererEngine& renderer, UINT _missionNumber);			// ミッション開始時
	bool SettingsData_MissionTermination(RendererEngine& renderer, UINT _missionNumber);	// ミッション終了時
	void ClearGameSceneResource(RendererEngine& renderer);

	void set_ScreenSize(UINT _w, UINT _h);		// スクリーンサイズの設定（基本的にRendererEngine以外からは設定しない）
	void set_DefaultFov(float _f) { m_DefaultFov = _f; }			// デフォルトFOVの設定
	void set_IdDebugMode(bool flag) { m_IsDebugMode = flag; }		// デバッグ用エディタのフラグ設定
	void set_IsCameraControl(bool _isControl) { m_IsCameraControl = _isControl; }	// カメラ制御の有無の設定
	void set_IsUseWeapon(bool _isUse) { m_IsUseWeapon = _isUse; }		// 武器使用の有無の設定
	void set_IsPlayerDead(bool _isDead) { m_IsPlayerDead = _isDead; }	// プレイヤーが死亡したかの設定
	void set_IsMissionCleared(bool _isflag) { m_IsMissionCleared = _isflag; }	// ミッションクリアしたかの設定
	void set_PlayerHP(float _hp) { m_PlayerHP = _hp; }
	void set_SelectDifficultyLevel(UtilityData::DIFFICULTY_LEVEL _diffLevel) { m_SelectDifficultyLevel = _diffLevel; }	// 難易度の設定
	void set_IsPause(bool _flag) { m_IsPause = _flag; }				// ポーズフラグ設定
	void set_IsTitle(bool _flag) { m_IsTitle = _flag; }				// タイトルフラグ設定

	UINT get_ScreenWidth() const { return m_ScreenWidth; };			// スクリーンの横幅を取得
	UINT get_ScreenHeight() const { return m_ScreenHeight; }		// スクリーンの縦幅を取得
	float get_DefaultFov()const { return m_DefaultFov; }			// デフォルトFOVを取得
	bool get_IsDebugMode()const { return m_IsDebugMode; }			// デバッグ用エディタのフラグ取得
	const UtilityData::UserConfigData& get_UserConfigData() const { return m_UserConfigData; }	// ユーザ設定データの取得
	bool get_IsCameraControl()const { return m_IsCameraControl; }	// カメラ制御の有無の取得
	bool get_IsUseWeapon()const { return m_IsUseWeapon; }			// 武器使用の有無の取得
	bool get_IsPlayerDead()const { return m_IsPlayerDead; }			// プレイヤーが死亡したか
	bool get_IsMissionCleared()const { return m_IsMissionCleared; }	// ミッションクリアしたか
	float get_PlayerHP()const { return m_PlayerHP; }
	const UtilityData::DIFFICULTY_LEVEL get_DifficultyLevel()const { return m_SelectDifficultyLevel; }	// 設定された難易度を取得
	const EnemyData::EnemyDifficultyFactor& get_EnemyDifficultyFactor()const;
	bool get_IsPause()const { return m_IsPause; }					// ポーズフラグ取得
	bool get_IsTitle()const { return m_IsTitle; }					// タイトルフラグ取得

	// 設定項目
	void set_BGMVolume(int _vol);				// BGM音量の設定
	void set_SEVolume(int _vol);				// SE音量の設定
	void set_MouseSensitivity(float _sens);		// マウス感度の設定
	void set_IsInvertY(bool _isInvert);			// カメラのY反転の有無の設定
	void set_IsCameraShake(bool _isShake);		// カメラシェイクの有無の設定
	void set_IsShadowEnabled(bool _isEnable);	// シャドウの有無の設定
	void set_UserConfigData(const UtilityData::UserConfigData &_data) { m_UserConfigData = _data; }	// ユーザ設定データの設定

	void set_Fov(float _fov);	// FOVの設定
	float get_Fov();			// FOVの取得

	void set_SelectWeaponID(int _id, int _slot) { m_SelectWeaponID[_slot] = _id; }	// 武器選択で選択した武器のIDを設定
	int get_SelectWeaponID( int _slot)const { return m_SelectWeaponID[_slot]; }	// 武器選択で選択した武器のIDを取得

private:
	// コピー禁止
	DataManager(const DataManager&) = delete;
	DataManager& operator=(const DataManager&) = delete;
	// ------------------------------------------------------
};


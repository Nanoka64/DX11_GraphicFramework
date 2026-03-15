#pragma once


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
	float m_ScreenWidth;
	float m_ScreenHeight;

public:
	DataManager();
	~DataManager();

	bool Init();

	bool SettingsData_TitleLoadOnEnter(RendererEngine& renderer);			// タイトルロード時
	bool SettingsData_TitleLoadOnExit(RendererEngine& renderer);			// タイトルロード時
	bool SettingsData_MissionLoad(RendererEngine& renderer, UINT _missionNumber);			// ミッション開始時
	bool SettingsData_MissionTermination(RendererEngine& renderer, UINT _missionNumber);	// ミッション終了時

	void set_ScreenWidth(float _w) { m_ScreenWidth = _w; };			// スクリーンの横幅を設定（基本的にRendererEngine以外からは設定しない）
	void set_ScreenHeight(float _h) { m_ScreenHeight = _h; };		// スクリーンの縦幅を設定（基本的にRendererEngine以外からは設定しない）

	float get_ScreenWidth() const { return m_ScreenWidth; };		// スクリーンの横幅を取得
	float get_ScreenHeight() const { return m_ScreenHeight; }		// スクリーンの縦幅を取得

private:
	// コピー禁止
	DataManager(const DataManager&) = delete;
	DataManager& operator=(const DataManager&) = delete;
	// ------------------------------------------------------
};


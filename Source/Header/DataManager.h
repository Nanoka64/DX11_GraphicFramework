#pragma once


// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:DataManager Class --- */
//
//  ★★★シングルトン★★★
//
// 【?】データ管理を行う
//		
//
// ***************************************************************************************
class DataManager
{
private:
public:
	DataManager();
	~DataManager();

	bool Init();

	bool SettingsData_TitleLoadOnEnter(RendererEngine& renderer);			// タイトルロード時
	bool SettingsData_TitleLoadOnExit(RendererEngine& renderer);			// タイトルロード時
	bool SettingsData_MissionLoad(RendererEngine& renderer, UINT _missionNumber);			// ミッション開始時
	bool SettingsData_MissionTermination(RendererEngine& renderer, UINT _missionNumber);	// ミッション終了時

private:
	// コピー禁止
	DataManager(const DataManager&) = delete;
	DataManager& operator=(const DataManager&) = delete;
	// ------------------------------------------------------
};



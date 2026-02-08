#pragma once
#include <chrono>

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:TimeManager Class --- */
//
//  ★★★シングルトン★★★
//
// 【?】時間の管理を行う
//		今の所デルタタイムの取得用に使う
// 
// 参考サイト：https://qiita.com/tsukino_/items/74b4a86a3b89e9841f29
//
// ***************************************************************************************
class TimeManager
{
private:
	float  m_DeltaTime;
	std::chrono::steady_clock::time_point m_PrevTime;

public:
	TimeManager();
	~TimeManager();

	bool Init();
	void Update();

	float get_DeltaTime()const {return m_DeltaTime;}	// デルタタイムの取得

private:
	// コピー禁止
	TimeManager(const TimeManager&) = delete;
	TimeManager& operator=(const TimeManager&) = delete;
	// ------------------------------------------------------

};


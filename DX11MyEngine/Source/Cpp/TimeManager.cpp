#include "pch.h"
#include "TimeManager.h"
#include <iostream>


//*---------------------------------------------------------------------------------------
//*【?】コンストラクタ
//*----------------------------------------------------------------------------------------
TimeManager::TimeManager():
	m_DeltaTime(0.0)
{
}

//*---------------------------------------------------------------------------------------
//*【?】デストラクタ
//*----------------------------------------------------------------------------------------
TimeManager::~TimeManager()
{
}

bool TimeManager::Init()
{
	// 初期化
	m_PrevTime = std::chrono::high_resolution_clock::now();

	return true;
}

//*---------------------------------------------------------------------------------------
//*【?】更新
//*
//* [引数]なし
//* [返値]なし
//*----------------------------------------------------------------------------------------
void TimeManager::Update()
{
	// 現在の時間を取得
	auto currentTime = std::chrono::high_resolution_clock::now();
	
	// デルタタイムを計算 (ミリ秒単位に変換)
	auto deltaTime = currentTime - m_PrevTime;

	// 現在の時間を次のループの基準時間に更新
	m_PrevTime = currentTime;
	
	// デルタタイムをfloatにして保持
	m_DeltaTime = std::chrono::duration<float>(deltaTime).count();

	// --- 追加：デルタタイムの上限（Clamp）処理 ---
	// 例: 0.1秒（10fps相当の処理落ち）を上限とする
	const float MAX_DELTA_TIME = 0.1f;
	if (m_DeltaTime > MAX_DELTA_TIME)
	{
		m_DeltaTime = MAX_DELTA_TIME;
	}
}

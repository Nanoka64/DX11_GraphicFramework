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
}

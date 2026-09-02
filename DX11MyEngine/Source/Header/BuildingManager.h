#pragma once
#include "ConstantBuildingData.h"

// ---------------------------------------------------------------------------------------
/* --- @:BuildingManager Class --- */
//
//  ★★★シングルトン★★★
//
// 【?】建物の管理
//
// ***************************************************************************************
class BuildingManager
{
private:
	// 建物のデータを保持する連想配列
    std::unordered_map<int, std::unique_ptr<BuildingData::BuildingData>> m_BuildingDataMap;


public:
    BuildingManager();
    ~BuildingManager();

private:
    // コピー禁止
    BuildingManager(const BuildingManager&) = delete;
    BuildingManager& operator=(const BuildingManager&) = delete;

	void LoadBuildingData();  // 建物データの読み込み
};


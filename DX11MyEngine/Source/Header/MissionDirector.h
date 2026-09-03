#pragma once

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:BuildingManager Class --- */
//
//  ★★★シングルトン★★★
//
// 【?】ミッションの管理
//
// ***************************************************************************************
class MissionDirector
{
private:
    std::vector<UtilityData::MissionData> m_MissionDataList;  // ミッションデータの保持

public:
    MissionDirector();
    ~MissionDirector();

private:
    // コピー禁止
    MissionDirector(const MissionDirector&) = delete;
    MissionDirector& operator=(const MissionDirector&) = delete;

    bool LoadMissionData(const std::string& _filePath, UtilityData::MissionData& _outData);  // ミッションデータの読み込み
};


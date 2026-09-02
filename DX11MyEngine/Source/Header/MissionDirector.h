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


public:
    MissionDirector();
    ~MissionDirector();

private:
    // コピー禁止
    MissionDirector(const MissionDirector&) = delete;
    MissionDirector& operator=(const MissionDirector&) = delete;

};


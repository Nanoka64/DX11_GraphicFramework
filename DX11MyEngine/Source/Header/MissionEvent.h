#pragma once
#include "EventCondition.h"
#include "MissionAction.h"

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:MissionEvent Class --- */
//
// 【?】ミッションイベントの管理
//
// ***************************************************************************************
class MissionEvent
{
private:
    UtilityData::ConditionOperator m_Operator;

    std::vector<std::unique_ptr<EventCondition>> m_Conditions;
    std::vector<std::unique_ptr<MissionAction>> m_Actions;
public:

};


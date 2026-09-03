#include "pch.h"
#include "MissionDirector.h"

using namespace UtilityData;

//*---------------------------------------------------------------------------------------
//*【?】コンストラクタ
//*----------------------------------------------------------------------------------------
MissionDirector::MissionDirector()
{
}

//*---------------------------------------------------------------------------------------
//*【?】デストラクタ
//*----------------------------------------------------------------------------------------
MissionDirector::~MissionDirector()
{
}

//*---------------------------------------------------------------------------------------
//*【?】ミッションデータの読み込み（json）
//*
//* [引数] 
//* _filePath : jsonのファイルパス
//* &_outData : 出力先 
//* [返値]
//* true  : 読みとり成功
//* false : 読みとり失敗
//*----------------------------------------------------------------------------------------
bool MissionDirector::LoadMissionData(const std::string& _filePath, UtilityData::MissionData& _outData)
{
    using json = nlohmann::json;
    std::ifstream ifs(_filePath);
    if (!ifs.is_open()) return false;

    json j;
    ifs >> j;
}

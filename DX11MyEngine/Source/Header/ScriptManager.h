#pragma once
#include <angelscript.h>

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:ScriptManager Class --- */
//
//  ★★★シングルトン★★★
//
// 【?】AngelScriptの管理
//
// ***************************************************************************************
class ScriptManager
{
private:
    asIScriptEngine* m_pEngine = nullptr;
    asIScriptContext* m_pContext = nullptr;

public:
    ScriptManager();
    ~ScriptManager();

    bool Init();
    void Update(float _deltaTime);
    
	void Term();

    bool LoadScript(const std::string& _filePath);

private:
    // コピー禁止
    ScriptManager(const ScriptManager&) = delete;
    ScriptManager& operator=(const ScriptManager&) = delete;

};


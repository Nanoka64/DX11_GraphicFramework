#include "pch.h"
#include "ScriptManager.h"
#include <angelscript/add_on/scriptbuilder/scriptbuilder.h>     // ファイル読み込み用
#include <angelscript/add_on/scriptstdstring/scriptstdstring.h> // string型を使えるようにする
#include <angelscript/add_on/scriptarray/scriptarray.h>         // arrayを使えるようにする
#include <angelscript/add_on/scriptmath/scriptmath.h>           // 数学関数を使えるようにする

using namespace Tool;

// メッセージコールバック
void MessageCallback(const asSMessageInfo* msg, void* param)
{
    const char* type = "ERR ";
    if (msg->type == asMSGTYPE_WARNING) type = "WARN";
    else if (msg->type == asMSGTYPE_INFORMATION) type = "INFO";

    printf("%s (%d, %d) : %s : %s\n", msg->section, msg->row, msg->col, type, msg->message);
}

// C++ から呼ばれる関数
void print(const std::string& msg)
{
    MessageBoxA(NULL, msg.c_str(), "WORLD", MB_OK);
}


//*---------------------------------------------------------------------------------------
//*【?】コンストラクタ
//*----------------------------------------------------------------------------------------
ScriptManager::ScriptManager()
{
}

//*---------------------------------------------------------------------------------------
//*【?】デストラクタ
//*----------------------------------------------------------------------------------------
ScriptManager::~ScriptManager()
{
}

//*---------------------------------------------------------------------------------------
//*【?】初期化
//*
//* [引数] 
//* なし
//* [返値]
//* true : 成功
//* false : 失敗
//*----------------------------------------------------------------------------------------
bool ScriptManager::Init()
{
    // =====================================
    // AngelScriptエンジン作成
    // =====================================
    m_pEngine = asCreateScriptEngine();
    assert(m_pEngine != nullptr);

    // メッセージコールバック登録
    int r = m_pEngine->SetMessageCallback(
        asFUNCTION(MessageCallback), 
        0, 
        asCALL_CDECL
    );
    assert(r >= 0);

    // =====================================
    // アドオンの登録
    // =====================================
    {
        // arrayアドオンの登録
        RegisterScriptArray(m_pEngine, true);

        // string型を使えるようにする
        RegisterStdString(m_pEngine);

        // 数学
        RegisterScriptMath(m_pEngine);
    }


    // グローバル関数 print() を登録
    r = m_pEngine->RegisterGlobalFunction(
        "void print(const string &in)", 
        asFUNCTION(print), 
        asCALL_CDECL
    );
    assert(r >= 0);

    // =====================================
    // スクリプト読み込み
    // =====================================
    // スクリプトビルダーでモジュール作成
    CScriptBuilder builder;
    r = builder.StartNewModule(m_pEngine, "MyModule");
    if (r < 0)
    {
        ErrorMessage(L"モジュール作成失敗", L"ScriptManager");
        return false;
    }

    r = builder.AddSectionFromFile("Resource/MISSION_AS/test.as");
    if (r < 0)
    {
        ErrorMessage(L"ファイル読み込み失敗\n", L"ScriptManager");
        return false;
    }
    
    
    // =====================================
    // スクリプトをコンパイル
    // =====================================
    r = builder.BuildModule();
    if (r < 0)
    {
        ErrorMessage(L"コンパイル\n", L"ScriptManager");
        return false;
    }

    // モジュール取得
    asIScriptModule* mod = m_pEngine->GetModule("MyModule");
    if (!mod)
    {
        ErrorMessage(L"モジュールが見つかりません\n", L"ScriptManager");
        return false;
    }

    // 関数取得
	asIScriptFunction* func = mod->GetFunctionByDecl("void as_main(void)");
    if (!func)
    {
        ErrorMessage(L"関数が見つかりません\n", L"ScriptManager");
        return false;
    }

    // コンテキスト作成・準備・実行
    m_pContext = m_pEngine->CreateContext();
    m_pContext->Prepare(func);
    r = m_pContext->Execute();
    if (r != asEXECUTION_FINISHED)
    {
        if (r == asEXECUTION_EXCEPTION)
        {
            ErrorMessage(StringToWstring(m_pContext->GetExceptionString()), L"ScriptManager");
        }
    }
    return true;
}


//*---------------------------------------------------------------------------------------
//*【?】更新
//*
//* [引数] 
//* _deltaTime : デルタタイム
//* 
//* [返値] 
//* なし
//*----------------------------------------------------------------------------------------
void ScriptManager::Update(float _deltaTime)
{
    int r = 0;

}

//*---------------------------------------------------------------------------------------
//*【?】終了
//*
//* [引数] なし
//* [返値] なし 
//*----------------------------------------------------------------------------------------
void ScriptManager::Term()
{
    // クリーンアップ
    m_pContext->Release();
    m_pEngine->ShutDownAndRelease();
}


//*---------------------------------------------------------------------------------------
//*【?】AngelScriptのスクリプトをロードする
//*
//* [引数] 
//* _filePath : ファイルパス
//* 
//* [返値] 
//* なし
//*----------------------------------------------------------------------------------------
bool ScriptManager::LoadScript(const std::string& _filePath)
{
    // =====================================
    // スクリプト読み込み
    // =====================================
    // スクリプトビルダーでモジュール作成
    CScriptBuilder builder;
    int r = builder.StartNewModule(m_pEngine, "MyModule");
    if (r < 0)
    {
        MessageBoxA(NULL, "モジュール作成失敗", "ScriptManager", MB_OK);
        return false;
    }

    r = builder.AddSectionFromFile(_filePath.c_str());
    if (r < 0)
    {
        print("ファイル読み込み失敗\n");
        return false;
    }


    return true;
}

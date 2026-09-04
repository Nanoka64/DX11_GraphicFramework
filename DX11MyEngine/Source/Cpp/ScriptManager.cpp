#include "pch.h"
#include "ScriptManager.h"
#include <angelscript/add_on/scriptbuilder/scriptbuilder.h>     // スクリプトビルダー
#include <angelscript/add_on/scriptstdstring/scriptstdstring.h> // string型を使えるようにする

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

    // string型を使えるようにする
	RegisterStdString(m_pEngine);

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
        MessageBoxA(NULL, "モジュール作成失敗", "ScriptManager", MB_OK);
        return false;
    }

    //// スクリプトをセクションとして追加
    //const char* script = R"(
    //    void as_main()
    //    {
    //        print("HelloWorld");
    //    }
    //)";

    r = builder.AddSectionFromFile("Resource/MISSION_AS/test.as");
    if (r < 0)
    {
        printf("Failed to add script section\n");
        return false;
    }
    
    
    // =====================================
    // スクリプトをコンパイル
    // =====================================
    r = builder.BuildModule();
    if (r < 0)
    {
        printf("Failed to build module\n");
        return false;
    }

    // モジュール取得
    asIScriptModule* mod = m_pEngine->GetModule("MyModule");
    if (!mod)
    {
        printf("Module not found\n");
        return false;
    }

    // 関数取得
	asIScriptFunction* func = mod->GetFunctionByDecl("void as_main()");
    if (!func)
    {
        printf("Function 'void as_main()' not found\n");
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
            printf("Exception: %s\n", m_pContext->GetExceptionString());
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
    return true;
}

#include "pch.h"
#include "DXApp.h"
#include "Window.h"
#include "Path.h"
#include "MyStruct.h"
#include "InputManager.h"
#include "DirectWriteManager.h"
#include "FontConfig.h"
#include "InputFactory.h"
#include "GameManager.h"
#include "RendererEngine.h"
#include "ResourceManager.h"

#include <string>
#include <cassert>

using namespace Input;
using namespace VERTEX;


Debugger                *Master::m_pDebugger            = nullptr;  // ImGui機能ラップ
ShaderManager           *Master::m_pShaderManager       = nullptr;  // シェーダ管理
LightManager            *Master::m_pLightManager        = nullptr;  // ライト管理
BlendManager            *Master::m_pBlendManager        = nullptr;  // ブレンド管理
DirectWriteManager      *Master::m_pDirectWriteManager  = nullptr;  // 文字管理 
GameObjectManager       *Master::m_pGameObjectManager   = nullptr;  // オブジェクト管理


//*---------------------------------------------------------------------------------------
//* @:DXApp Class 
//*【?】コンストラクタ
//* 引数：1.ウインドウの横幅
//* 引数：2.ウインドウの縦幅
//*----------------------------------------------------------------------------------------
DXApp::DXApp(uint32_t width, uint32_t height) :
    m_hInst(nullptr),
    m_hWnd(nullptr),
    m_Width(width),
    m_Height(height),
    m_pRenderer(),
    m_pGameManager()
{

}


//*---------------------------------------------------------------------------------------
//* @:DXApp Class 
//*【?】デストラクタ
//* 引数：なし
//*----------------------------------------------------------------------------------------
DXApp::~DXApp()
{

}


//*---------------------------------------------------------------------------------------
//* @:DXApp Class 
//*【?】初期化
//* 引数：1.ウインドウのインスタンスハンドル
//* 引数：2.今は使用していない
//* 引数：3.ウインドウの表示状態
//* 返値：bool
//*----------------------------------------------------------------------------------------
bool DXApp::Init(HINSTANCE hInstance,LPSTR lpCmdLine, int nCmdShow)
{
    m_hInst = NULL;
    m_hWnd  = NULL;

    /** ゲーム・描画の管理クラス **/
    /**     インスタンス化       **/
    m_pGameManager = new GameManager();
    m_pRenderer = std::make_shared<RendererEngine>();

    Master::m_pDebugger             = new Debugger();              // ImGui機能ラップ 
    Master::m_pShaderManager        = new ShaderManager();         // シェーダ管理
    Master::m_pLightManager         = new LightManager();          // ライト管理
    Master::m_pBlendManager         = new BlendManager();          // ブレンド管理
    Master::m_pDirectWriteManager   = new DirectWriteManager();    // 文字管理 
    Master::m_pGameObjectManager    = new GameObjectManager();     // オブジェクト管理

    // *************************************************************************************************
    /**  ウインドウの初期化 **/
    // *************************************************************************************************
    if (FAILED(this->InitWindow(hInstance, nCmdShow))) 
    {
        assert(false);
        return false;
    }

    // *************************************************************************************************
    /**  描画(directX)初期化 **/
    // *************************************************************************************************
    if (!m_pRenderer->Init(m_hWnd))
    {
        assert(false);
        return false;
    }

    // *************************************************************************************************
    //** ブレンドステートの初期化 **/
    // *************************************************************************************************
    if (!Master::m_pBlendManager->Init(m_pRenderer))
    {
        assert(false);
        return false;
    }

    // *************************************************************************************************
    //** シェーダ管理の初期化 **/
    //** 描画クラスの弱参照を入れる   **/
    // *************************************************************************************************
    if (!Master::m_pShaderManager->Init(m_pRenderer))
    {
        assert(false);
        return false;
    }

    ///////////////////////////////////////////
    // シェーダ作成
    ///////////////////////////////////////////
    if (!Master::m_pShaderManager->CreateShader(SHADER_TYPE::DEFERRED_STD_STATIC,           SHADER_CREATE_TYPE::RUNTIME))return false;
    if (!Master::m_pShaderManager->CreateShader(SHADER_TYPE::DEFERRED_STD_STATIC_N,         SHADER_CREATE_TYPE::RUNTIME))return false;
    if (!Master::m_pShaderManager->CreateShader(SHADER_TYPE::DEFERRED_STD_SKINNED_N,        SHADER_CREATE_TYPE::RUNTIME))return false;
    if (!Master::m_pShaderManager->CreateShader(SHADER_TYPE::FORWARD_UNLIT_UI_SPRITE,       SHADER_CREATE_TYPE::RUNTIME))return false;
    if (!Master::m_pShaderManager->CreateShader(SHADER_TYPE::DEFERRED_STD_RT_SPRITE,        SHADER_CREATE_TYPE::RUNTIME))return false;
    if (!Master::m_pShaderManager->CreateShader(SHADER_TYPE::FORWARD_UNLIT_STATIC,          SHADER_CREATE_TYPE::RUNTIME))return false;
    if (!Master::m_pShaderManager->CreateShader(SHADER_TYPE::POST_GAUSSIAN_BLUR_HORIZONTAL, SHADER_CREATE_TYPE::RUNTIME))return false;
    if (!Master::m_pShaderManager->CreateShader(SHADER_TYPE::POST_GAUSSIAN_BLUR_VERTICAL,   SHADER_CREATE_TYPE::RUNTIME))return false;
    if (!Master::m_pShaderManager->CreateShader(SHADER_TYPE::POST_SKYBOX,                   SHADER_CREATE_TYPE::RUNTIME))return false;
    if (!Master::m_pShaderManager->CreateShader(SHADER_TYPE::POST_LUMINANCE_FILTER,         SHADER_CREATE_TYPE::RUNTIME))return false;
    if (!Master::m_pShaderManager->CreateShader(SHADER_TYPE::POST_KAWASE_FILTER,            SHADER_CREATE_TYPE::RUNTIME))return false;
    if (!Master::m_pShaderManager->CreateShader(SHADER_TYPE::POST_SHADOWMAP,                SHADER_CREATE_TYPE::RUNTIME))return false;
    if (!Master::m_pShaderManager->CreateShader(SHADER_TYPE::POST_SHADOW_RECIEVER,          SHADER_CREATE_TYPE::RUNTIME))return false;


    // *************************************************************************************************
    //** リソース管理の初期化 **/
    //** 描画クラスの弱参照を入れる   **/
    // *************************************************************************************************
    if (!ResourceManager::Instance().Init(m_pRenderer))
    {
        assert(false);
        return false;
    }


    // *************************************************************************************************
    /**  directWrire 初期化 **/
    // *************************************************************************************************
    //if (FAILED(Master::m_pDirectWriteManager->Init(*m_pRenderer)))
    //{
    //    assert(false);
    //    return false;
    //}

    // *************************************************************************************************
    /**  ゲームマネージャー初期化 **/
    // *************************************************************************************************
    if (!m_pGameManager->Init(*m_pRenderer))
    {
        assert(false);
        return false;
    }


    // *************************************************************************************************
    /**  ライトマネージャー初期化 **/
    // *************************************************************************************************
    if (!Master::m_pLightManager->Init(m_pRenderer))
    {
        assert(false);
        return false;
    }

    // *************************************************************************************************
    /** デバッガー初期化 **/
    // *************************************************************************************************
    if (!Master::m_pDebugger->Init(m_hWnd,m_pRenderer))
    {
        assert(false);
        return false;
    }

    // *************************************************************************************************
    /**  入力マネージャー初期化 **/
    // *************************************************************************************************
    if (!InputManager::GetInstance().Init(m_hWnd))
    {
        assert(false);
        return false;
    }

    ///** フォントデータ作成 **/
    //FONT_DATA *pFontData = new FONT_DATA();
    //pFontData->fontSize = 20.0f;
    //pFontData->fontWeight = DWRITE_FONT_WEIGHT_BOLD;
    //pFontData->color = D2D1::ColorF(D2D1::ColorF::White);

    //// フォントデータをDirectWriteManagerに設定
    //Master::m_pDirectWriteManager->SetFontData(pFontData);

    // 正常終了
    return true;
}


//*---------------------------------------------------------------------------------------
//* @:DXApp Class 
//*【?】終了処理
//* 引数：なし
//* 返値：void
//*----------------------------------------------------------------------------------------
void DXApp::Term()
{
    m_pGameManager->Term(*m_pRenderer);

    InputManager::GetInstance().Release();
}


//*---------------------------------------------------------------------------------------
//* @:DXApp Class 
//*【?】更新処理
//* 引数：なし
//* 返値：int
//*----------------------------------------------------------------------------------------
int DXApp::MainLoop()
{
    timeBeginPeriod(1); // タイマーの分解機能設定を１ms(１ミリ秒 = 1/1000秒)にする
    DWORD crntTime = timeGetTime(); // 現在時刻
    DWORD lastTime = crntTime;      // 前回更新時刻

    MSG msg = { 0 };
    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            crntTime = timeGetTime();   // 現在時刻を更新
            float difference = (crntTime - lastTime);

            // 現在時刻と前回更新時刻の差分が１６.６６msより大きければゲーム処理を実行する
            if (difference > 1000.0f / g_Fps)
            {
                if (GetInput(CONFIG_INPUT::PAUSE)) break;   // 終了ボタン

                // 前回時刻を更新
                lastTime = crntTime;

                Master::m_pDebugger->BeginFrame(m_pRenderer->get_ScreenWidth(), m_pRenderer->get_ScreenHeight());

                // アプリケーション情報
                Master::m_pDebugger->BeginDebugWindow("Application");
                Master::m_pDebugger->DG_TextValue("CrntTime : %d", crntTime);
                Master::m_pDebugger->DG_TextValue("LastTime : %d", lastTime);
                Master::m_pDebugger->DG_TextValue("Difference : %f.3", difference);
                Master::m_pDebugger->EndDebugWindow();


                // ゲーム更新
                m_pGameManager->Update(*m_pRenderer);

                // 入力更新
                InputManager::GetInstance().Update();

                // 描画の開始
                m_pRenderer->BeginRender();

                // ゲーム描画
                m_pGameManager->Draw(*m_pRenderer);

                // 描画の終了
                m_pRenderer->EndRender();

                Master::m_pDebugger->EndFrame();

                // 画面更新
                m_pRenderer->Swap();
            }
        }
    }

    m_pRenderer->Term();  // リソース解放

    timeBeginPeriod(1); // タイマーの分解機能設定をデフォルトに戻す

    return (int)msg.wParam;
}


//*---------------------------------------------------------------------------------------
//* @:DXApp Class 
//*【?】DXAppの実行（エントリーポイント的な）
//* 引数：1.ウインドウのインスタンスハンドル
//* 引数：2.コマンドライン（今は使用していない）
//* 引数：3.ウインドウを表示するためのもの
//* 返値：void
//*----------------------------------------------------------------------------------------
void DXApp::Run(HINSTANCE hInstance,  LPSTR lpCmdLine, int nCmdShow)
{
    // 初期化が成功したらメインループに入る
    if (Init(hInstance,lpCmdLine,nCmdShow))
    {
        MainLoop();
    }

    // 終了
    Term();
}



// ======================================================================================================
//
/*                      ウインドウ制御関連                 */
//
// ======================================================================================================

//*---------------------------------------------------------------------------------------
//* @:DXApp Class 
//*【?】ウインドウの初期化
//* 引数：1.ウインドウのインスタンスハンドル
//* 引数：2.ウインドウを表示するためのもの
//* 返値：HRESULT
//*----------------------------------------------------------------------------------------
HRESULT DXApp::InitWindow(HINSTANCE hInstance, int nCmdShow)
{
    // ウインドウにデータを渡す
    // Register class
    WNDCLASSEX wcex;
    ZeroMemory(&wcex, sizeof(wcex));
    wcex.cbSize        = sizeof(WNDCLASSEX);    // 構造体のサイズ
    wcex.style         = CS_CLASSDC;            // ウインドウスタイル
    wcex.lpfnWndProc   = WndProc;               // コールバック関数(アドレス)渡す
    wcex.cbClsExtra    = 0;                     // ウインドウクラス構造体の後ろに割り当てる補足バイト数
    wcex.cbWndExtra    = 0;                     // ウインドウインスタンスの後ろに割り当てる補足バイト数
    wcex.hInstance     = hInstance;             // インスタンスハンドル
    wcex.hIcon         = LoadIcon(NULL, IDI_QUESTION);  // アイコンのハンドル
    wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);   // マウスカーソルのハンドル
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);    // ウインドウ背景色
    wcex.lpszMenuName  = NULL;                  // デフォルトメニュー名
    wcex.lpszClassName = g_WindowClassNameA;    // オリジナルの名前付ける
    wcex.hIconSm       = NULL;                  // 左上やタスクバーの小さいアイコン
    
    if (!RegisterClassEx(&wcex))                // この設定でウインドウズにお願いする
        return E_FAIL;

    // windowの作成
    // Create window
    m_hInst = hInstance;
    RECT rc = { WND_RECT_LEFT, WND_RECT_TOP,  WND_RECT_RIGHT, WND_RECT_BOTTOM };      // 矩形を設定する
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);   // ウインドウの形を整える
    m_hWnd = CreateWindowW(
        g_WindowClassNameW,                              // 上で設定した名前と合わせる
        g_WindowTitle,                                   // ウインドウタイトル
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU |        // ウインドウスタイル
        WS_MAXIMIZEBOX | WS_MINIMIZEBOX,
        /*WS_OVERLAPPEDWINDOW | WS_POPUP,*/              // 基本ウインドウ、タイトルバー表示、Xボタンなど、サイズ変更、全画面
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        rc.right - rc.left,
        rc.bottom - rc.top,
        NULL,
        NULL,
        hInstance,
        NULL);
    
    if (!m_hWnd)
        return E_FAIL;

    // 消すと出ないよ
    ShowWindow(m_hWnd, nCmdShow);

    return S_OK;
}


//*---------------------------------------------------------------------------------------
//* @:DXApp Class 
//*【?】ウインドウの終了
//* 引数：なし
//* 返値：void
//*----------------------------------------------------------------------------------------
void DXApp::TermWindow()
{

}

// Imguiウインドウを動かすためのもの
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
//*---------------------------------------------------------------------------------------
//* @:DXApp Class 
//*【?】ウインドウプロシージャ
//      ※コールバック関数
//* 引数：1.メッセージを受け取ったウインドウのハンドル（どのウインドウに表示するかはこの受け取ったウインドウによって決まっている？）
//* 引数：2.メッセージの種別
//* 引数：3.補助パラメータ的な奴っぽい
//* 引数：4.・・・
//* 返値：LRESULT
//*----------------------------------------------------------------------------------------
LRESULT CALLBACK DXApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    // Imguiウインドウを動かすためのもの
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
        return true;

    switch (message)
    {
    case WM_PAINT:      // 再描画命令
        hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        break;

    case WM_DESTROY:    // 終了時に呼ばれる
        PostQuitMessage(0);
        break;


    default:            // デフォルトウインドウプロシージャ(いい感じにやってくれる)
        return DefWindowProc(hWnd, message, wParam, lParam);
        break;
    }

    return 0;
}


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
ResourceManager         *Master::m_pResourceManager     = nullptr;  // リソース管理
EditorManager           *Master::m_pEditorManager       = nullptr;  // エディタ管理
InputManager            *Master::m_pInputManager        = nullptr;   // 入力管理
CollisionManager        *Master::m_pCollisionManager    = nullptr;   // 衝突管理
EffectManager           *Master::m_pEffectManager       = nullptr;   // エフェクト管理
SoundManager            *Master::m_pSoundManager        = nullptr;   // サウンド管理


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
    m_pGameManager(),
    m_IsClose(false)
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

    Master::m_pDebugger             = new Debugger();               // ImGui機能ラップ 
    Master::m_pShaderManager        = new ShaderManager();          // シェーダ管理
    Master::m_pLightManager         = new LightManager();           // ライト管理
    Master::m_pBlendManager         = new BlendManager();           // ブレンド管理
    Master::m_pDirectWriteManager   = new DirectWriteManager();     // 文字管理 
    Master::m_pGameObjectManager    = new GameObjectManager();      // オブジェクト管理
    Master::m_pResourceManager      = new ResourceManager();        // リソース管理
    Master::m_pEditorManager        = new EditorManager();          // エディタ管理
    Master::m_pInputManager         = new InputManager();           // 入力管理
    Master::m_pCollisionManager     = new CollisionManager();       // 衝突管理
    Master::m_pEffectManager        = new EffectManager();          // エフェクト管理
    Master::m_pSoundManager         = new SoundManager();           // サウンド管理

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
    if (!Master::m_pShaderManager->CreateShader(SHADER_TYPE::DEFERRED_STD_STATIC,           SHADER_CREATE_TYPE::CSO))return false;
    if (!Master::m_pShaderManager->CreateShader(SHADER_TYPE::DEFERRED_STD_STATIC_N,         SHADER_CREATE_TYPE::CSO))return false;
    if (!Master::m_pShaderManager->CreateShader(SHADER_TYPE::DEFERRED_STD_SKINNED_N,        SHADER_CREATE_TYPE::CSO))return false;
    if (!Master::m_pShaderManager->CreateShader(SHADER_TYPE::FORWARD_UNLIT_UI_SPRITE,       SHADER_CREATE_TYPE::CSO))return false;
    if (!Master::m_pShaderManager->CreateShader(SHADER_TYPE::DEFERRED_STD_RT_SPRITE,        SHADER_CREATE_TYPE::CSO))return false;
    if (!Master::m_pShaderManager->CreateShader(SHADER_TYPE::FORWARD_UNLIT_STATIC,          SHADER_CREATE_TYPE::CSO))return false;
    if (!Master::m_pShaderManager->CreateShader(SHADER_TYPE::POST_GAUSSIAN_BLUR_HORIZONTAL, SHADER_CREATE_TYPE::CSO))return false;
    if (!Master::m_pShaderManager->CreateShader(SHADER_TYPE::POST_GAUSSIAN_BLUR_VERTICAL,   SHADER_CREATE_TYPE::CSO))return false;
    if (!Master::m_pShaderManager->CreateShader(SHADER_TYPE::POST_SKYBOX,                   SHADER_CREATE_TYPE::CSO))return false;
    if (!Master::m_pShaderManager->CreateShader(SHADER_TYPE::POST_LUMINANCE_FILTER,         SHADER_CREATE_TYPE::CSO))return false;
    if (!Master::m_pShaderManager->CreateShader(SHADER_TYPE::POST_KAWASE_FILTER,            SHADER_CREATE_TYPE::CSO))return false;
    if (!Master::m_pShaderManager->CreateShader(SHADER_TYPE::POST_SHADOWMAP,                SHADER_CREATE_TYPE::RUNTIME))return false;
    if (!Master::m_pShaderManager->CreateShader(SHADER_TYPE::POST_SHADOWMAP_SKINNED,        SHADER_CREATE_TYPE::RUNTIME))return false;
    if (!Master::m_pShaderManager->CreateShader(SHADER_TYPE::POST_DEPTH_OF_FILED,           SHADER_CREATE_TYPE::CSO))return false;
    if (!Master::m_pShaderManager->CreateShader(SHADER_TYPE::POST_TONEMAPPING,              SHADER_CREATE_TYPE::CSO))return false;
    if (!Master::m_pShaderManager->CreateShader(SHADER_TYPE::DEFERRED_STD_TRAIL,            SHADER_CREATE_TYPE::RUNTIME))return false;

    // *************************************************************************************************
    //** リソース管理の初期化 **/
    //** 描画クラスの弱参照を入れる   **/
    // *************************************************************************************************
    if (!Master::m_pResourceManager->Init(m_pRenderer))
    {
        assert(false);
        return false;
    }


    // *************************************************************************************************
    /**  directWrire 初期化 **/
    // *************************************************************************************************
    if (FAILED(Master::m_pDirectWriteManager->Init(*m_pRenderer)))
    {
        assert(false);
        return false;
    }

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
    if (!Master::m_pInputManager->Init(m_hWnd))
    {
        assert(false);
        return false;
    }
    
    // *************************************************************************************************
    /**  エディタマネージャー初期化 **/
    // *************************************************************************************************
    if (!Master::m_pEditorManager->Init(*m_pRenderer))
    {
        assert(false);
        return false;
    }   
    
    // *************************************************************************************************
    /**  エフェクトマネージャの初期化 **/
    // *************************************************************************************************
    if (!Master::m_pEffectManager->Setup(*m_pRenderer))
    {
        assert(false);
        return false;
    }
    
    // *************************************************************************************************
    /**  サウンドマネージャの初期化 **/
    // *************************************************************************************************
    if (!Master::m_pSoundManager->InitXA2Sound())
    {
        assert(false);
        return false;
    }

    /** フォントデータ作成 **/
    // 白 40サイズ 標準 ----------------------------------------------
    FONT_DATA *pFontData = new FONT_DATA("White_40_STD"); 
    pFontData->fontSize = 40.0f;
    pFontData->fontWeight = DWRITE_FONT_WEIGHT_BOLD;
    pFontData->color = D2D1::ColorF(D2D1::ColorF::White);

    // 登録
    Master::m_pDirectWriteManager->SetFontData(pFontData);
    SAFE_DELETE(pFontData);

    // 白 30サイズ 標準 ----------------------------------------------
    pFontData = new FONT_DATA("White_30_STD");  
    pFontData->fontSize = 30.0f;
    pFontData->fontWeight = DWRITE_FONT_WEIGHT_BOLD;
    pFontData->color = D2D1::ColorF(D2D1::ColorF::White);

    // 登録
    Master::m_pDirectWriteManager->SetFontData(pFontData);
    SAFE_DELETE(pFontData);

    // 白 20サイズ 標準 ----------------------------------------------
    pFontData = new FONT_DATA("White_20_STD");  
    pFontData->fontSize = 20.0f;
    pFontData->fontWeight = DWRITE_FONT_WEIGHT_BOLD;
    pFontData->color = D2D1::ColorF(D2D1::ColorF::White);

    // 登録
    Master::m_pDirectWriteManager->SetFontData(pFontData);
    SAFE_DELETE(pFontData);

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

    Master::m_pShaderManager->Term();
    Master::m_pLightManager->Term();
    Master::m_pDirectWriteManager->Term();
    Master::m_pBlendManager->Term();
    Master::m_pGameObjectManager->Term(*m_pRenderer);
    Master::m_pResourceManager->Term();
    Master::m_pInputManager->Term();
    Master::m_pSoundManager->UninitXA2Sound();
    m_pRenderer->Term();
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
            DWORD difference = (crntTime - lastTime);

            // 現在時刻と前回更新時刻の差分が１６.６６msより大きければゲーム処理を実行する
            if (difference > 1000.0f / g_Fps)
            {
                // 終了ボタンが押されるか、ゲームの終了フラグが立っているか
                if (GetInput(GAME_CONFIG::PAUSE) || m_pGameManager->get_IsGameClose()) break;   

                // 前回時刻を更新
                lastTime = crntTime;

                float win_width = static_cast<float>(m_pRenderer->get_ScreenWidth());
                float win_height = static_cast<float>(m_pRenderer->get_ScreenHeight());

                Master::m_pDebugger->BeginFrame(win_width, win_height);

                // アプリケーション情報
                Master::m_pDebugger->BeginDebugWindow("Application");
                Master::m_pDebugger->DG_TextValue("CrntTime : %d", crntTime);
                Master::m_pDebugger->DG_TextValue("LastTime : %d", lastTime);
                Master::m_pDebugger->DG_TextValue("Difference : %f.3", difference);
                Master::m_pDebugger->EndDebugWindow(); 
                
                using namespace Tool;


                // 操作ガイド
                Master::m_pDebugger->BeginDebugWindow(U8ToChar(u8"説明"));
                Master::m_pDebugger->DG_Text(U8ToChar(u8"DirectX11を使用した自作3Dグラフィックフレームワークです。"));
                Master::m_pDebugger->DG_Text(U8ToChar(u8"レンダリングパイプラインから自作し、基本的なライティング、\nポストエフェクト、シャドウ等を実装しました。"));
                Master::m_pDebugger->DG_Text(U8ToChar(u8"プログラム側はコンポーネント指向で設計し、\n柔軟にオブジェクトを作成できるようにしています。"));
                Master::m_pDebugger->DG_Text(U8ToChar(u8"※ 現在は技術デモ的なものとなってしまっているため、\nクリア画面等はありません。"));
                Master::m_pDebugger->DG_Separator();    // 区切り
                Master::m_pDebugger->DG_BulletText(U8ToChar(u8"☆操作方法"));
                Master::m_pDebugger->DG_Text(U8ToChar(u8"WASD：移動"));
                Master::m_pDebugger->DG_Text(U8ToChar(u8"マウスまたは矢印キー：視点操作"));
                Master::m_pDebugger->DG_Text(U8ToChar(u8"Space：ジャンプ"));
                Master::m_pDebugger->DG_Text(U8ToChar(u8"AまたはDを押しながらSpace：緊急回避"));
                Master::m_pDebugger->DG_Text(U8ToChar(u8"左クリック：射撃"));
                Master::m_pDebugger->DG_Separator();    // 区切り
                Master::m_pDebugger->DG_BulletText(
                    U8ToChar(
                        u8"オブジェクトリストからオブジェクトを選択し、インスペクタでコンポーネントの\n一部パラメータ等が見れたりいじれるようになっています。"));


                Master::m_pDebugger->DG_Separator();    // 区切り
                Master::m_pDebugger->DG_BulletText(U8ToChar(u8"☆実装コンポーネント"));

                Master::m_pDebugger->DG_BulletText(U8ToChar(u8"ユーティリティ"));
                Master::m_pDebugger->DG_Text(U8ToChar(u8"・Transform\n・Camera3D\n"));
                Master::m_pDebugger->DG_Separator();    // 区切り


                Master::m_pDebugger->DG_BulletText(U8ToChar(u8"メッシュ"));
                Master::m_pDebugger->DG_Text(U8ToChar(u8"・ModelMeshResource\n・ModelMeshRenderer\n・SkinnedMeshAnimator\n・SkyRenderer\n・SpriteRenderer\n・MeshResource\n・MeshRenderer\n・LineRenderer\n・TrailRenderer\n・BillboardResource\n・BillboardRenderer\n"));
                Master::m_pDebugger->DG_Separator();    // 区切り


                Master::m_pDebugger->DG_BulletText(U8ToChar(u8"ライト"));
                Master::m_pDebugger->DG_Text(U8ToChar(u8"・DirectionLight\n・PointLight\n"));
                Master::m_pDebugger->DG_Separator();    // 区切り

                
                Master::m_pDebugger->DG_BulletText(U8ToChar(u8"コライダー"));
                Master::m_pDebugger->DG_Text(U8ToChar(u8"・BoxCollider\n・SphereCollider\n"));
                Master::m_pDebugger->DG_Separator();    // 区切り

                
                Master::m_pDebugger->DG_BulletText(U8ToChar(u8"ゲームプレイ用"));
                Master::m_pDebugger->DG_Text(U8ToChar(u8"・PlayerController\n・EnemyController\n・Health\n"));
                Master::m_pDebugger->DG_Separator();    // 区切り

                Master::m_pDebugger->EndDebugWindow();

                // DirectWrite描画開始
                Master::m_pDirectWriteManager->BeginDraw();

                // 入力更新
                Master::m_pInputManager->Update();
                
                // 描画の開始
                m_pRenderer->BeginRender();

                // ゲーム更新
                m_pGameManager->Update(*m_pRenderer);

                // ゲーム描画
                m_pGameManager->Draw(*m_pRenderer);

                // エフェクト更新
                Master::m_pEffectManager->UpdateEffect(*m_pRenderer);
                Master::m_pEffectManager->DrawEffect();

                // ImGUI描画終了
                Master::m_pDebugger->EndFrame();

                // DirectWrite描画終了
                Master::m_pDirectWriteManager->EndDraw();

                // 描画の終了
                m_pRenderer->EndRender();

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

    DWORD dwStyle = WS_OVERLAPPED | /*WS_CAPTION | */ WS_SYSMENU | /*WS_MAXIMIZEBOX |*/ WS_MINIMIZEBOX;

    // windowの作成
    // Create window
    m_hInst = hInstance;
    RECT rc = { WND_RECT_LEFT, WND_RECT_TOP,  WND_RECT_RIGHT, WND_RECT_BOTTOM };      // 矩形を設定する
    int res = AdjustWindowRect(&rc, dwStyle, FALSE);   // ウインドウの形を整える

    m_hWnd = CreateWindowW(
        g_WindowClassNameW,                              // 上で設定した名前と合わせる
        g_WindowTitle,                                   // ウインドウタイトル
        dwStyle,                                         // ウインドウスタイル
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


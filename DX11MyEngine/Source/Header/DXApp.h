#pragma once
//--------------------------------------------------------------------------------------
//      * Includes *
//--------------------------------------------------------------------------------------
#include <windows.h>        // ウインドウ系
#include <time.h>
#include <dwrite.h>
#include <string.h>
#include <stdio.h>
#include <new>
#include <d2d1.h>
#include <wrl/client.h>
#include <cstdint>  // 追加
#include <memory>   // 追加

/* .libリンクしてねっていうやつ(プロパティからも出来る) */
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "imm32.lib")

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:DXApp Class --- */
//
// 【?】アプリケーションの管理
//      ウインドウの初期化や、DirectX制御クラスの管理
//
// ***************************************************************************************
class DXApp
{
private:
    HINSTANCE   m_hInst;    // インスタンスハンドル
    HWND        m_hWnd;     // ウインドウハンドル
    uint32_t    m_Width;    // ウインドウ横幅
    uint32_t    m_Height;   // ウインドウ縦幅

    WINDOW_MODE m_WindowMode = WINDOW_MODE::WINDOW;
    DWORD m_WindowStyle = 0;

    class GameManager *m_pGameManager;                  // ゲームマネージャー
    std::shared_ptr<class RendererEngine> m_pRenderer;  // 描画管理
    
    bool m_IsClose;     // 終了フラグ
    bool m_IsEditMode;  // 編集モードにするか

    // FPSは1フレームの計測値をそのまま表示すると数値が激しく変動するため、
    // 一定時間分のフレーム数と経過時間を蓄積し、その区間の平均値を表示する。
    uint32_t m_FpsFrameCount;      // 現在のFPS計測区間で描画したフレーム数
    float    m_FpsElapsedSeconds;  // 現在のFPS計測区間で経過した実時間（秒）
    float    m_CurrentFps;         // Debuggerに表示する、直近の計測区間の平均FPS
public:

    /// <summary>
    ///【?】 コンストラクタ
    /// </summary>
    /// <param name="width">ウインドウの横幅</param>
    /// <param name="height">ウインドウの縦幅</param>
    DXApp(uint32_t width, uint32_t height); 


    /// <summary>
    /// デストラクタ
    /// </summary>
    ~DXApp();                      

    /// <summary>
    /// 終了フラグを立てる
    /// </summary>
    void OnClose() { m_IsClose = true; };

    /// <summary>
    ///【?】DXAppの実行（エントリーポイント的な）
    /// </summary>
    /// <param name="hInstance">ウインドウのインスタンスハンドル</param>
    /// <param name="lpCmdLine">コマンドライン（今は使用していない）</param>
    /// <param name="nCmdShow"> ウインドウを表示するためのもの</param>
    void Run(HINSTANCE hInstance, LPSTR lpCmdLine, int nCmdShow);  // 実行
  
    void set_WindowMode(WINDOW_MODE mode);

    /// <summary>
    /// デバッグモードかどうか
    /// </summary>
    /// <returns>フラグ</returns>
    bool get_IsDebugMode()const { return m_IsEditMode; }

private:
    /// <summary>
    ///【?】 初期化
    /// </summary>
    /// <param name="hInstance">ウインドウのインスタンスハンドル</param>
    /// <param name="lpCmdLine">今は使用していない</param>
    /// <param name="nCmdShow">ウインドウの表示状態</param>
    /// <returns>初期化が成功したか</returns>
    bool Init(HINSTANCE hInstance, LPSTR lpCmdLine, int nCmdShow); 


    /// <summary>
    ///【?】 終了
    /// </summary>
    void Term();    


    /// <summary>
    ///【?】 更新処理
    /// </summary>
    /// <returns></returns>
    int MainLoop();      


    /// <summary>
    /// アプリケーションのImGui関連
    /// </summary>
    void AppEditDrawImGui();


    /// <summary>
    /// 1フレーム分の実経過時間をFPS計測値へ反映する
    /// </summary>
    /// <param name="frameElapsedSeconds">前回の描画開始から今回の描画開始までの実時間（秒）</param>
    void UpdateFps(float frameElapsedSeconds);


    /// <summary>
    /// UpdateFpsで算出したFPSをDebuggerへ表示する
    /// </summary>
    /// <remarks>DebuggerのBeginDebugWindowとEndDebugWindowの間で呼び出すこと</remarks>
    void DrawFps();


    // ======================================================================================================
    //
    /*                      ウインドウ制御関連                 */
    //
    // ======================================================================================================
    /// <summary>
    ///【?】ウインドウの初期化
    /// </summary>
    /// <param name="hInstance">ウインドウのインスタンスハンドル</param>
    /// <param name="nCmdShow"> ウインドウを表示するためのもの</param>
    /// <returns>初期化が成功したか</returns>
    HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);  

    
    /// <summary>
    /// ウインドウの終了
    /// </summary>
    void TermWindow();    


    /// <summary>
    /// ウインドウプロシージャ
    /// </summary>
    /// <param name=""></param>
    /// <param name=""></param>
    /// <param name=""></param>
    /// <param name=""></param>
    /// <returns></returns>
    static LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);    



    void SetBorderlessFullscreen(HWND hwnd);
    void SetWindowMode(HWND hwnd);
};


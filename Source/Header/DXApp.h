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

/* .libリンクしてねっていうやつ(プロパティからも出来る) */
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "imm32.lib")

//#include "RendererManager.h"
//#include "GameManager.h"

using Microsoft::WRL::ComPtr;


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

    class GameManager      *m_pGameManager; // ゲームマネージャー
    std::shared_ptr<class RendererManager> m_pRenderer;// 描画管理

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
    ///【?】DXAppの実行（エントリーポイント的な）
    /// </summary>
    /// <param name="hInstance">ウインドウのインスタンスハンドル</param>
    /// <param name="lpCmdLine">コマンドライン（今は使用していない）</param>
    /// <param name="nCmdShow"> ウインドウを表示するためのもの</param>
    void Run(HINSTANCE hInstance, LPSTR lpCmdLine, int nCmdShow);  // 実行
    
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
};


//------------------------------------------------------------------------------------------

// Includes

//------------------------------------------------------------------------------------------
#include "pch.h"
#include <windows.h>        // ウインドウ系
#include <time.h>
#include "DxApp.h"
//------------------------------------------------------------------------------------------

// メインエントリーポイント

//------------------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // 乱数初期化
    srand((unsigned int)time(NULL));

    // アプリケーションを実行
    DXApp dxApp(960, 540);
    dxApp.Run(hInstance, lpCmdLine, nCmdShow);
}
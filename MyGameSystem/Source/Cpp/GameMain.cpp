

//------------------------------------------------------------------------------------------
// エンジンのリンク
//------------------------------------------------------------------------------------------
#ifdef _DEBUG
#pragma comment(lib, "DX11MyEngined.lib")   
#else
#pragma comment(lib, "DX11MyEngine.lib")
#endif


//------------------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------------------
#include <windows.h>        // ウインドウ系
#include <time.h>
#include "../../Engine/Include/DXApp.h"
#include "../../Engine/Include/Window.h"




//------------------------------------------------------------------------------------------
// メインエントリーポイント
//------------------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // 乱数初期化
    srand((unsigned int)time(NULL));

    // アプリケーションを実行
    DXApp dxApp(WND_RECT_RIGHT, WND_RECT_BOTTOM);
    dxApp.Run(hInstance, lpCmdLine, nCmdShow);
}
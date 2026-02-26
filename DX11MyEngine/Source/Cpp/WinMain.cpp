#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include<crtdbg.h>
#endif//defined(DEBUG)||defined(_DEBUG)

//------------------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------------------
#include "pch.h"
#include <windows.h>        // ウインドウ系
#include <time.h>
#include "DxApp.h"
#if _DEBUG
#include "../../../XML_Excel/XmlArrange.h"
#endif

//------------------------------------------------------------------------------------------

// メインエントリーポイント

//------------------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
#if _DEBUG
    XmlArrange *xmlArrange = new XmlArrange();
    xmlArrange->Arrange();
    delete xmlArrange;
#endif

    // 乱数初期化
    srand((unsigned int)time(NULL));

#if defined(DEBUG) || defined(_DEBUG)
    //_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif//defined(DEBUG)||defined(_DEBUG)

    //_CrtSetBreakAlloc(7202115);

    // アプリケーションを実行
    DXApp dxApp(WND_RECT_RIGHT, WND_RECT_BOTTOM);
    dxApp.Run(hInstance, lpCmdLine, nCmdShow);
}
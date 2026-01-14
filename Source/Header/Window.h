#pragma once

/* ウインドウの大きさ */
#define WND_RECT_TOP		0		// 上部
#define WND_RECT_BOTTOM		1080	// 下部
#define WND_RECT_RIGHT		1920	// 右側
#define WND_RECT_LEFT		0		// 左側


// ウインドウのタイトルバーのとこ
constexpr LPCWSTR g_WindowTitle = L"DX11_3DGraphicFramework";

// ウインドウクラス名 内部的な識別名？
constexpr LPCWSTR g_WindowClassNameW = L"DX11_3DGAME_SL";
constexpr LPCSTR  g_WindowClassNameA = "DX11_3DGAME_SL";

// FPS
constexpr float g_Fps = 60.0f;
#pragma once

//
/* プリコンパイル済みヘッダ */
//

/* マクロ定義 */
#define SAFE_RELEASE(x)		  {if(x) { (x)->Release(); (x)=nullptr; } }	// 解放
#define SAFE_DELETE(p)		  {if(p) { delete p; p = nullptr; } }	// メモリの開放
#define SAFE_DELETE_ARRAY(p)  {if(p) { delete[] p; p = nullptr; } }	// メモリの解放(配列)
#define CHECK_HRESULT(hr)       do { if (FAILED(hr)) { assert(false); return hr; } } while(0)    // HRESULTチェック do while(0)は複数行マクロの定石らしい？
#define CHECK_HRESULT_NO_BOOL(hr)       do { if (FAILED(hr)) { assert(false); return; } } while(0)    // HRESULTチェック do while(0)は複数行マクロの定石らしい？

#define DEBUG_MODE false	// 内部的にデバッグモードにするかどうか

#define min NOMINMAX    // min STLとwindows.hのマクロがかぶっているのを回避する



/* 組み込みヘッダ */
#include <d3d11.h>          // directX11
#include <d3d12.h>          // directX11
#include <DirectXMath.h>    // 数学系
#include <DirectXTex.h>     // テクスチャ読み込み
#pragma comment(lib, "d3d11.lib")			  // directX11ライブラリのリンク


// COMポインタ用
#include <wrl/client.h>



// 文字表示用
#include <d2d1.h>   // Direct2D用
#include <dwrite.h> // DirectWrite用
#pragma comment(lib,"d2d1.lib")     // Direct2D用
#pragma comment(lib,"dwrite.lib")   // DirectWrite用

//!**************************************************************
//! Texを内部でリンクさせようとすると正体不明のエラーが出るため、
//! プロパティ側で対応
//!**************************************************************
// Texライブラリのリンク
//#ifdef _DEBUG
//#pragma comment(lib, "DirectXTexd.lib")
//#else
//#pragma comment(lib, "DirectXTex.lib")
//#endif

// シェーダー
#include <d3dcompiler.h>				// シェーダーをコンパイルするためのやつ
#pragma comment(lib, "d3dcompiler.lib") // シェーダコンパイル用の静的ライブラリをリンク

// 入力用
#include <dinput.h>         
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dInput8.lib")

// スタンダードヘッダ
#include <unordered_map>    // 連想配列用
#include <map>      // 連想配列用
#include <memory>   // スマートポインタ用
#include <string>   // 文字列
#include <vector>   // 動的配列



/* 定数 */
constexpr int POINTLIGHT_MAX_NUM		= 500; // ポイントライトの最大数
constexpr int DIRECTIONLIGHT_MAX_NUM	= 1;   // ディレクションライトの最大数


/* 自作ヘッダ */
#include "VERTEX.h"			// 頂点情報
#include "SHADER.h"			// シェーダ情報
#include "Window.h"			// ウインドウ情報
#include "Helper.h"			// ヘルパー
#include "MyStruct.h"		// 定数バッファ用構造体
#include "Master.h"   
//#include "ShaderManager.h"	// シェーダ管理
//#include "Debugger.h"	    // Imguiデバッグ用
//#include "BlendManager.h"   // ブレンドステート用

//#include "InputFactory.h"// 入力処理

/* クラス前方宣言 */
class RendererEngine;
class Camera;
class SceneManager;
class GameManager;



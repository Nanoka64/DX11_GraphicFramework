#pragma once
//--------------------------------------------------------------------------------------
//      * Includes *
//--------------------------------------------------------------------------------------
#include "TRIANGLE.h"
#include "RenderParam.h"	// 描画情報
#include "SHADER.h"	// 描画情報

using namespace DirectX;

// =======================================================================================
//
// *---        RendererManager Class         ---*
//		描画関連(dirextX)の処理をまとめたもの
//		
// =======================================================================================

class RendererManager
{
private:
    D3D_DRIVER_TYPE                           m_driverType;
    D3D_FEATURE_LEVEL                         m_featureLevel;         // DirectXのバージョン
    ID3D11Device                            * m_pd3dDevice;           // GPUを抽象化してくれる
    ID3D11DeviceContext                     * m_pImmediateContext;    // 絵を描く部分を抽象化したもの
    IDXGISwapChain                          * m_pSwapChain;           // バッファの入れ替え(バッファ数増やすと入力遅延が起きる)
    ID3D11RenderTargetView                  * m_pRenderTargetView;    // 絵を描く領域を切り替える(メモリ領域を覗く、眼鏡みたいなもの)
    ID3D11Texture2D                         * m_pDepthStencil;        // 画像を読み込んで使えるようにするもの
    ID3D11DepthStencilView                  * m_pDepthStencilView;    // 深度バッファ(ZBuffer)奥行き
    ID3D11DepthStencilState                 * m_pDepthStencilState;   // Z比較をするための設定
    ID3D11Buffer                            * m_pVertexBuffer;        // 頂点バッファ(実際の頂点のデータが詰まっている)
    ID3D11Buffer                            * m_pCBNeverChanges;      // コンスタントバッファの実体
    ID3D11Buffer                            * m_pCBChangeOnResize;    // ...
    ID3D11Buffer                            * m_pCBChangesEveryFrame; // ...
    ID3D11SamplerState                      * m_pSamplerLinear;       // テクスチャからどうピクセルをもらうか、サンプルをどうするか
    ID3D11RasterizerState                   * m_pRasterState;         // どこを塗るのか決める(実際には塗るのはピクセルシェーダ)
    ID3D11BlendState                        * m_pBlendStateAlpha;      // αブレンド用
    ID3D11BlendState                        * m_pBlendStateAdd;        // 加算合成用
    ID3D11BlendState                        * m_pBlendStateSub;        // 減算合成用
	
    SHADER_DATA m_Shader;   // シェーダ情報
    RenderParam m_RenderParam;  // 描画パラメータ

    UINT m_ScreenWidht; // ウインドウ横幅
    UINT m_Screenheight;// ウインドウ縦幅
    HWND m_hWnd;        // ウインドウハンドル

    VERTEX3 test;

    DWORD m_StartTime;

    float m_NearClipDist;
    float m_FarClipDist;
    float m_Fov;


public:
    RendererManager();
    ~RendererManager();

    bool Init(HWND hWnd);
	void Draw();
	void Term();
    void Swap();    // 裏表切り替え
private:


    bool InitDx11();
    HRESULT InitDX11_SwapChain();               // ＤＸ１１ スワップチェイン初期化
    HRESULT InitDX11_RenderTargetView();        // ＤＸ１１ レンダーターゲットビューの初期化
    HRESULT InitDX11_ZBuff();                   // ＤＸ１１ Ｚバッファ初期化
    HRESULT InitDX11_Rasterizer();              // ＤＸ１１ ラスタライザ初期化
    HRESULT InitDX11_BuffResource();            // ＤＸ１１ バッファリソース初期化
    HRESULT InitDX11_Sampler();                 // ＤＸ１１ サンプラー初期化
    HRESULT InitDX11_BlendState();              // ＤＸ１１ ブレンドステート初期化
    HRESULT InitDX11_ConstantBuff();            // ＤＸ１１ 定数バッファ初期化
    HRESULT InitShader();	                    // シェーダーの初期化
    void CleanupDX11();       // ＤＸ１１ 開放

    void EnableDebugLayer();    // デバッグレイヤーの有効化

	HRESULT CompileShaderFromFile(LPCWSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut); // シェーダーのコンパイル

    bool SetupProjectionTransform();    //透視投影変換計算
public:
    /* ↓ get・set ↓*/
    ID3D11Device* get_Device()const { return m_pd3dDevice; }                      // デバイス取得
    ID3D11DeviceContext* get_DeviceContext()const { return m_pImmediateContext; } // デバイスコンテキスト取得
    IDXGISwapChain *get_SwapChain() const { return m_pSwapChain; }        // スワップチェイン取得
    ID3D11SamplerState* get_Sampler() const { return m_pSamplerLinear; }  // サンプラー取得
    RenderParam &get_RenderParam() { return m_RenderParam; }        // 描画に必要な定数バッファ取得                        
    //SHADER_DATA &get_ShaderData() { return m_Shader; }              // シェーダ情報取得
    HWND get_WndHandle()const { return m_hWnd; }                        // ウインドウハンドル取得

    bool SetupViewTransform(const XMMATRIX& viewMat);
};


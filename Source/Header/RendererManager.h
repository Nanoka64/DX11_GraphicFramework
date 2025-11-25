#pragma once
//--------------------------------------------------------------------------------------
//      * Includes *
//--------------------------------------------------------------------------------------
#include "TRIANGLE.h"
#include "RenderParam.h"	// 描画情報
#include "SHADER.h"	// 描画情報

using namespace DirectX;


// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:RendererManager Class --- */
//
// 【?】描画関連(dirextX)の処理をまとめたもの
//
// ***************************************************************************************
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
    ID3D11SamplerState                      * m_pSamplerLinear;       // テクスチャからどうピクセルをもらうか、サンプルをどうするか
    ID3D11RasterizerState                   * m_pRasterState;         // どこを塗るのか決める(実際には塗るのはピクセルシェーダ)
    //ID3D11BlendState                        * m_pBlendStateAlpha;     // αブレンド用
    //ID3D11BlendState                        * m_pBlendStateAdd;       // 加算合成用
    //ID3D11BlendState                        * m_pBlendStateSub;       // 減算合成用
	

    DirectX::XMMATRIX m_Proj;
    DirectX::XMMATRIX m_View;

    // レンダーターゲット用
    std::shared_ptr<class RenderTarget> m_pRenderTarget;


    ID3D11Buffer                            * m_pVertexBuffer;        // 頂点バッファ(実際の頂点のデータが詰まっている)
    ID3D11Buffer                            * m_pIndexBuffer;         // 頂点バッファ(実際の頂点のデータが詰まっている)


    RenderParam m_RenderParam;  // 描画パラメータ

    UINT m_ScreenWidht; // ウインドウ横幅
    UINT m_Screenheight;// ウインドウ縦幅
    HWND m_hWnd;        // ウインドウハンドル

    DWORD m_StartTime;

    float m_NearClipDist;
    float m_FarClipDist;
    float m_Fov;


public:
    RendererManager();
    ~RendererManager();

    bool Init(HWND hWnd);
	void BeginRender();
    void EndRender();
	void Term();
    void Swap();    // 裏表切り替え
private:
    bool InitDx11();                            // ＤＸ１１の初期化
    HRESULT InitDX11_SwapChain();               // ＤＸ１１ スワップチェイン初期化
    HRESULT InitDX11_RenderTargetView();        // ＤＸ１１ レンダーターゲットビューの初期化
    HRESULT InitDX11_ZBuff();                   // ＤＸ１１ Ｚバッファ初期化
    HRESULT InitDX11_Rasterizer();              // ＤＸ１１ ラスタライザ初期化
    HRESULT InitDX11_Sampler();                 // ＤＸ１１ サンプラー初期化
    //HRESULT InitDX11_BlendState();              // ＤＸ１１ ブレンドステート初期化
    void CleanupDX11();                         // ＤＸ１１ 開放
    void EnableDebugLayer();                    // デバッグレイヤーの有効化
    bool SetupProjectionTransform();            //透視投影変換計算


public:
    ID3D11Device* get_Device()const { return m_pd3dDevice; }                      // デバイス取得
    ID3D11DeviceContext* get_DeviceContext()const { return m_pImmediateContext; } // デバイスコンテキスト取得
    IDXGISwapChain *get_SwapChain() const { return m_pSwapChain; }                // スワップチェイン取得
    ID3D11SamplerState* get_Sampler() const { return m_pSamplerLinear; }          // サンプラー取得
    RenderParam &get_RenderParam() { return m_RenderParam; }                      // 描画に必要な定数バッファ取得                        
    HWND get_WndHandle()const { return m_hWnd; }                                  // ウインドウハンドル取得
    UINT get_ScreenWidth()const { return m_ScreenWidht; };
    UINT get_ScreenHeight()const { return m_Screenheight; };


    /// <summary>
    /// レンダーターゲットをフレームバッファに変更
    /// </summary>
    void ChangeRenderTargetFrameBuffer();


    /// <summary>
    /// レンダーターゲットを解除しNULL設定
    /// </summary>
    void ReleaseRenderTargetSetNull();

    /// <summary>
    /// レンダーターゲットを登録
    /// </summary>
    /// <param name="num"></param>
    /// <param name="renderTargets"></param>
    void RegisterRenderTargets(UINT num, class DX_RenderTarget *renderTargets[]);

    /// <summary>
    /// レンダーターゲットのクリア
    /// </summary>
    void ClearRenderTargetViews(UINT num, class DX_RenderTarget *renderTargets[]);


    /// <summary>
    /// ビューマトリクスを設定し更新
    /// </summary>
    /// <param name="viewMat"></param>
    /// <returns></returns>
    bool SetupViewTransform(const XMMATRIX& viewMat);


    /// <summary>
    /// ビュープロジェクション行列の取得
    /// </summary>
    /// <returns></returns>
    XMMATRIX get_ViewProjectionMatrix();


    /// <summary>
    /// ビュープロジェクション行列の逆行列取得
    /// </summary>
    /// <returns></returns>
    XMFLOAT4X4 get_ViewProjectionInvMatrix();
};


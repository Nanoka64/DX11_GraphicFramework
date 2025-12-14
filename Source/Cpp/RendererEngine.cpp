//--------------------------------------------------------------------------------------
//      * Includes *
//--------------------------------------------------------------------------------------
#include "pch.h"
#include "RendererEngine.h"
#include "MyStruct.h"
#include "Path.h"
#include "DX_RenderTarget.h"
#include "BlendManager.h"
#include <string>

//*---------------------------------------------------------------------------------------
//* @:RendererEngine Class 
//*【?】コンストラクタ
//* 引数：なし
//*----------------------------------------------------------------------------------------
RendererEngine::RendererEngine() :
    m_driverType(D3D_DRIVER_TYPE_UNKNOWN),
    m_featureLevel(D3D_FEATURE_LEVEL_11_0),
    m_pd3dDevice(nullptr),
    m_pImmediateContext(nullptr),
    m_pSwapChain(nullptr),
    m_pRenderTargetView(nullptr),
    m_pDepthStencil(nullptr),
    m_pDepthStencilView(nullptr),
    m_pVertexBuffer(nullptr),
    m_pSamplerLinear(nullptr),
    m_pRasterState(nullptr),
    m_pDepthStencilState(nullptr),
    //m_pBlendStateAlpha(nullptr),
    //m_pBlendStateAdd(nullptr),
    //m_pBlendStateSub(nullptr),
    m_ScreenWidht(0),
    m_Screenheight(0),
    m_hWnd(0),
    m_StartTime(0ul),
    m_NearClipDist(0.1f),
    m_FarClipDist(1000.f),
    m_Fov(XMConvertToRadians(30.0f)),
    m_Proj(XMMatrixIdentity()),
    m_View(XMMatrixIdentity())
{
}


//*---------------------------------------------------------------------------------------
//* @:RendererEngine Class 
//*【?】デストラクタ
//* 引数：なし
//*----------------------------------------------------------------------------------------
RendererEngine::~RendererEngine()
{

}


//*---------------------------------------------------------------------------------------
//* @:RendererEngine Class 
//*【?】初期化
//* 引数：1.ウインドウハンドル
//* 戻値：成功したか
//*----------------------------------------------------------------------------------------
bool RendererEngine::Init(HWND hWnd)
{
    m_driverType            = D3D_DRIVER_TYPE_NULL;
    m_featureLevel          = D3D_FEATURE_LEVEL_11_0;    // DirectXのバージョン
    m_pd3dDevice            = NULL;        // GPUを抽象化してくれる
    m_pImmediateContext     = NULL;        // 絵を描く部分を抽象化したもの
    m_pSwapChain            = NULL;        // バッファの入れ替え(バッファ数増やすと入力遅延が起きる)
    m_pRenderTargetView     = NULL;        // 絵を描く領域を切り替える(メモリ領域を覗く、眼鏡みたいなもの)
    m_pDepthStencil         = NULL;        // 画像を読み込んで使えるようにするもの
    m_pDepthStencilView     = NULL;        // 深度バッファ(ZBuffer)奥行き
    m_pVertexBuffer         = NULL;        // 頂点バッファ(実際の頂点のデータが詰まっている)
    m_pSamplerLinear        = NULL;        // テクスチャからどうピクセルをもらうか、サンプルをどうするか
    m_pRasterState          = NULL;        // どこを塗るのか決める(実際には塗るのはピクセルシェーダ)
    m_pDepthStencilState    = NULL;        // Z比較をするための設定
    //m_pBlendStateAlpha = NULL;        // αブレンド用
    //m_pBlendStateAdd   = NULL;        // 加算合成用
    //m_pBlendStateSub   = NULL;        // 減算合成用

    // ガクつくときはここを大きくするとよい
    m_NearClipDist = 1.0f;
    m_FarClipDist  = 50000.0f;
    m_Fov = XMConvertToRadians(30.0f);


    m_hWnd = hWnd;  // ウインドウハンドル受け取る

    RECT rc;
    GetClientRect(m_hWnd, &rc); // ウインドウの黒い部分の大きさを返す
    m_ScreenWidht = rc.right - rc.left;
    m_Screenheight = rc.bottom - rc.top;


    // 正常に初期化されたか
    if (!InitDx11()) {
        CleanupDX11();  // されて無ければ解放
        return false;
    }

    // 初期化
    m_RenderParam.Init(*this);

    m_StartTime = timeGetTime();    // 開始時間取得

    SetupProjectionTransform(m_ScreenWidht, m_Screenheight);

    return true;
}


//*---------------------------------------------------------------------------------------
//* @:RendererEngine Class 
//*【?】描画の開始
//* 引数：なし
//* 戻値：void
//*----------------------------------------------------------------------------------------
void RendererEngine::BeginRender()
{
    // フレームバッファのレンダリングターゲットとデプスステンシルのクリア
    FLOAT clearColor[] = { 1.0f,0.0f,1.0f,1.0f };
    m_pImmediateContext->ClearRenderTargetView(m_pRenderTargetView, clearColor);
    m_pImmediateContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0, 0);


    //// OMにブレンドステートオブジェクトを設定
    //FLOAT BlendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
    //m_pImmediateContext->OMSetBlendState(m_pBlendStateAlpha, nullptr, 1);   
    
    // 深度ステンシルステート設定
    m_pImmediateContext->OMSetDepthStencilState(m_pDepthStencilState, 0);   

    // ラスタライザ設定
    m_pImmediateContext->RSSetState(m_pRasterState); 

    // サンプラー設定
    m_pImmediateContext->PSSetSamplers(0, 1, &m_pSamplerLinear);
}


//*---------------------------------------------------------------------------------------
//* @:RendererEngine Class 
//*【?】描画の終了
//* 引数：なし
//* 戻値：void
//*----------------------------------------------------------------------------------------
void RendererEngine::EndRender()
{
    // なにかあれば
}

//*---------------------------------------------------------------------------------------
//* @:RendererEngine Class 
//*【?】終了
//* 引数：なし
//* 戻値：void
//*----------------------------------------------------------------------------------------
void RendererEngine::Term()
{
    CleanupDX11();  // リソース解放
}


//*---------------------------------------------------------------------------------------
//* @:RendererEngine Class 
//*【?】スワップ・裏表切り替え
//* 引数：なし
//* 戻値：void
//*----------------------------------------------------------------------------------------
void RendererEngine::Swap()
{
    HRESULT hr;
    hr = m_pSwapChain->Present(0, 0);    // 引数１:垂直同期　２：特殊フラグ(0でいい)

    if (FAILED(hr))return;
}


//
//
/* ------------------------ ↓↓↓ ＤｉｒｅｃｔＸ１１ ↓↓↓ ------------------------ */
//
// 

//--------------------------------------------------------------------------------------
//      * RendererEngine Class - デバッグレイヤーの有効化 - *
//      https://zenn.dev/kd_gamegikenblg/articles/431326b7f0cb2b
//--------------------------------------------------------------------------------------
void RendererEngine::EnableDebugLayer()
{
    ID3D11Debug* debugLayer = nullptr;
}


//*---------------------------------------------------------------------------------------
//* @:RendererEngine Class 
//*【?】DirectX11の初期化
//* 引数：なし
//* 戻値：成功したか
//*----------------------------------------------------------------------------------------
bool RendererEngine::InitDx11()
{
    // ＤｉｒｅｃｔＸ１１ 各初期化
    if (FAILED(InitDX11_SwapChain()))       return false;
    if (FAILED(InitDX11_RenderTargetView()))return false;
    if (FAILED(InitDX11_ZBuff()))           return false;
    if (FAILED(InitDX11_Rasterizer()))      return false;
    if (FAILED(InitDX11_Sampler()))         return false;


    //if (FAILED(InitDX11_BlendState()))      return false;

    return true;
}

//*---------------------------------------------------------------------------------------
//* @:RendererEngine Class 
//*【?】スワップチェインの初期化
//* 引数：なし
//* 戻値：成功したか
//*----------------------------------------------------------------------------------------
HRESULT RendererEngine::InitDX11_SwapChain()
{
    HRESULT hr = S_OK;

    /* ここ 0指定だとDirect2Dを使う際は D3D11_CREATE_DEVICE_BGRA_SUPPORT を指定しないと動かない！！！！ */
    UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT ;


    /* ここフラグ指定するとエラーが出るので一旦コメントアウト */
#ifdef  _DEBUG
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif 

    // ドライバタイプ 描画をハードウェアとソフトウェアのどちらで行うか
    // ※ Direct2Dを使う場合、HADWAREじゃないと動かない http://mitsunagistudio.net/old_posts/tips/d2d-d3d11-sharing/
    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,   // GPUにレンダリングを任せる
        //D3D_DRIVER_TYPE_WARP,       // 高機能？
        //D3D_DRIVER_TYPE_REFERENCE,  // 精度重視(速度遅め)
    };

    UINT numDriverTypes = ARRAYSIZE(driverTypes);   // 配列サイズ取得

    // ドライバの機能レベル
    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };

    UINT numFeatureLevels = ARRAYSIZE(featureLevels); // 配列サイズ取得

    //
    /* スワップチェインの設定 */
    //
    {
        DXGI_SWAP_CHAIN_DESC sd;
        ZeroMemory(&sd, sizeof(sd));    // メモリを０で埋める
        sd.BufferCount          = 2;                                // バッファ(裏画面)の数(絵を描くキャンパスの枚数)
        sd.BufferDesc.Width     = m_ScreenWidht;                    // バッファの横幅
        sd.BufferDesc.Height    = m_Screenheight;                   // バッファの縦幅
        sd.BufferDesc.Format    = DXGI_FORMAT_B8G8R8A8_UNORM;       // カラーフォーマット
        sd.BufferDesc.RefreshRate.Numerator   = 60;                 // リフレッシュレートの分母
        sd.BufferDesc.RefreshRate.Denominator = 1;                  // リフレッシュレートの分子
        sd.BufferUsage          = DXGI_USAGE_RENDER_TARGET_OUTPUT;  // バッファの使い方
        sd.OutputWindow         = m_hWnd;                           // 出力対象のウインドウハンドル
        sd.SampleDesc.Count     = 1;                                // マルチサンプリングのサンプル数(未使用は1)
        sd.SampleDesc.Quality   = 0;                                // マルチサンプリングの品質(未使用は0)
        sd.Windowed             = TRUE;                             // ウインドウモード指定
        sd.SwapEffect           = DXGI_SWAP_EFFECT_FLIP_DISCARD;    // ..._FLIP...のものが現在は推奨(それ以外はビットで切り替え。これはポインタの差し替えで画面更新)

        // ドライバー種別を上から検証し選択
        // 選択したデバイスを用いて描画する
        for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
        {
            m_driverType = driverTypes[driverTypeIndex];
            hr = D3D11CreateDeviceAndSwapChain(
                NULL,                   // ビデオアダプタの指定(NULLで既定)
                m_driverType,           // ドライバのタイプ
                NULL,                   // D3D_DRIVER_TYPE_SOFTWARE(ソフトウェア)指定時に使用
                createDeviceFlags,      // フラグ指定
                featureLevels,          // D3D_FEATURE_LEVEL指定で自分で定義した配列を指定可能
                numFeatureLevels,       // 上のD3D_FEATURE_LEVEL配列の要素数
                D3D11_SDK_VERSION,      // SDKバージョン
                &sd,                    // DXGI_SWAP_CHAIN_DESC
                &m_pSwapChain,          // 関数成功時のSwapChainの出力先
                &m_pd3dDevice,          // 関数成功時のDeviceの出力先
                &m_featureLevel,        // 成功したD3D_FEATURE_LEVELの出力先
                &m_pImmediateContext    // 関数成功時のContextの出力先
            );

            if (SUCCEEDED(hr)) break;
        }

        if (FAILED(hr))return hr;
    }

    return hr;
}


//*---------------------------------------------------------------------------------------
//* @:RendererEngine Class 
//*【?】レンダーターゲットビューの作成
//* 引数：なし
//* 戻値：成功したか
//*----------------------------------------------------------------------------------------
HRESULT RendererEngine::InitDX11_RenderTargetView()
{
    HRESULT hr = S_OK;

    ID3D11Texture2D* pBackBuffer = NULL;

    // バックバッファ取得
    hr = m_pSwapChain->GetBuffer(
        0,                          // バックバッファのインデックス(0は裏)
        __uuidof(ID3D11Texture2D),  // 取得したいインターフェース型のGUID
        (LPVOID*)&pBackBuffer       // バックバッファの格納先
    );
    if (FAILED(hr)) return hr;

    // 作成
    hr = m_pd3dDevice->CreateRenderTargetView(
        pBackBuffer,                // 元となるテクスチャ(バックバッファ)
        NULL,                       // D3D11_RENDER_TARGET_VIEW_DESC ( NULLなら自動設定 )
        &m_pRenderTargetView        // 生成したレンダーターゲットの出力先
    );
    pBackBuffer->Release();
    if (FAILED(hr))return hr;


    //
    /* レンダリングターゲットのどの部分に絵を描くか */
    //
    D3D11_VIEWPORT vp;
    ZeroMemory(&vp, sizeof(vp));
    vp.Width = (FLOAT)m_ScreenWidht;   // 描画範囲の横幅       
    vp.Height = (FLOAT)m_Screenheight; // 描画範囲の縦幅
    vp.MinDepth = 0.0f;                // Ｚバッファの最小値
    vp.MaxDepth = 1.0f;                // Ｚバッファの最大値
    vp.TopLeftX = 0.0f;                // 描画範囲の左側Ｘ座標
    vp.TopLeftY = 0.0f;                // 描画範囲の上部Ｙ座標
    m_pImmediateContext->RSSetViewports(1, &vp);

    return hr;
}


//*---------------------------------------------------------------------------------------
//* @:RendererEngine Class 
//*【?】Z（デプス）バッファの初期化
//* 引数：なし
//* 戻値：成功したか
//*----------------------------------------------------------------------------------------
HRESULT RendererEngine::InitDX11_ZBuff()
{
    HRESULT hr = S_OK;

    // 深度ステンシル用テクスチャリソース作成
    D3D11_TEXTURE2D_DESC descDepth;                     // テクスチャの情報を入れる構造体
    ZeroMemory(&descDepth, sizeof(descDepth));          // メモリを0で埋める
    descDepth.Width              = m_ScreenWidht;       // バックバッファと同じサイズ
    descDepth.Height             = m_Screenheight;      //...
    descDepth.MipLevels          = 1;                   // LOD処理の一つ0にすると限界まで作成する
    descDepth.ArraySize          = 1;                   // 基本的には１(キューブマップとかやる場合は使う)
    descDepth.SampleDesc.Count   = 1;                   // MSAA(マルチサンプリングエイリアス)の設定
    descDepth.SampleDesc.Quality = 0;                   // サンプリングのモード切替え(ハードウェア依存) 基本的に 0 
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;   // デプスバッファを24分割(0.0～1.0) 32bitのうちデプス24、ステンシル8

    // D3D11_USAGE_DEFAULT = 0,     // GPUからRead,Write
    // D3D11_USAGE_IMMUTABLE = 1,   // GPUからRead
    // D3D11_USAGE_DYNAMIC = 2,     // CPUからWrite,GPUからRead
    // D3D11_USAGE_STAGING = 3      // CPUからRead,Write
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    // D3D11_CPU_ACCESS_READ
    // D3D11_CPU_ACCESS_WRITE
    descDepth.CPUAccessFlags = 0;   // ＣＰＵの読み書き
    descDepth.MiscFlags      = 0;   // 基本的に 0 (特殊なフラグ)

    // 作成
    hr = m_pd3dDevice->CreateTexture2D(&descDepth, NULL, &m_pDepthStencil);
    if (FAILED(hr)) return hr;

    // 深度ステンシルビュー作成
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    ZeroMemory(&descDSV, sizeof(descDSV));
    descDSV.Format          = descDepth.Format;              // 上で設定したものを入れる
    descDSV.ViewDimension   = D3D11_DSV_DIMENSION_TEXTURE2D; // 2Dテクスチャ
    descDSV.Texture2D.MipSlice = 0;

    // 作成
    hr = m_pd3dDevice->CreateDepthStencilView(m_pDepthStencil, &descDSV, &m_pDepthStencilView);
    if (FAILED(hr))return hr;

    // MRT(マルチレンダリングターゲット)の設定もできる
    //m_pImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);


    //
    /* Ｚテストの設定 */
    //
    D3D11_DEPTH_STENCIL_DESC depthSD;
    ZeroMemory(&depthSD, sizeof(depthSD));
    depthSD.DepthEnable = TRUE;     // 深度テスト有効化

    // D3D11_DEPTH_WRITE_MASK_ZERO  = 0, Z(depth)Test OFF
    // D3D11_DEPTH_WRITE_MASK_ALL   = 1  Z(depth)Test ON
    depthSD.DepthWriteMask  = D3D11_DEPTH_WRITE_MASK_ALL;  // Zテストオン
    depthSD.DepthFunc       = D3D11_COMPARISON_LESS;       // ＜ 小なり(描画対象のＺ値が既存ピクセルより手前なら描画)
    depthSD.StencilEnable   = TRUE;                        // 今回はステンシルテスト有効化

    m_pd3dDevice->CreateDepthStencilState(&depthSD, &m_pDepthStencilState); // ステート作成
    m_pImmediateContext->OMSetDepthStencilState(m_pDepthStencilState, 0);   // ステート設定

    return hr;
}


//*---------------------------------------------------------------------------------------
//* @:RendererEngine Class 
//*【?】ラスタライザの初期化
//* 引数：なし
//* 戻値：成功したか
//*----------------------------------------------------------------------------------------
HRESULT RendererEngine::InitDX11_Rasterizer()
{
    /* ラスタライザにどんな処理をしてもらうか */

    HRESULT hr = S_OK;

    D3D11_RASTERIZER_DESC rd;
    ZeroMemory(&rd, sizeof(rd));

    // ※ CullMode
    // D3D11_CULL_NONE  = 1,      // カリングしない(重い)
    // D3D11_CULL_FRONT = 2,      // 表はカリング(時計回り)
    // D3D11_CULL_BACK  = 3       // 裏はカリング(逆時計回り)
    rd.CullMode              = D3D11_CULL_NONE;
    rd.FillMode              = D3D11_FILL_SOLID; // どう塗りつぶすか（ここでは普通に描画）
    rd.MultisampleEnable     = FALSE;   // マルチサンプリング時の配慮をするか(ポリゴンの端を滑らかにできるが処理コスト増)
    rd.FrontCounterClockwise = FALSE;   // 時計回りが裏面
    rd.DepthBias             = 0;       // 深度バイアス
    rd.DepthBiasClamp        = 0;
    rd.SlopeScaledDepthBias  = 0;
    rd.DepthClipEnable       = TRUE;    // 0.0より小さい1.0より大きい場合計算しない(カメラのnarとfarのやつ)
    rd.ScissorEnable         = FALSE;   // シザー矩形なし
    rd.AntialiasedLineEnable = FALSE;   // ライン・アンチエイリアシングなし
    m_pd3dDevice->CreateRasterizerState(&rd, &m_pRasterState);  // ステート作成
    m_pImmediateContext->RSSetState(m_pRasterState);;           // ステート設定

    return hr;
}


//*---------------------------------------------------------------------------------------
//* @:RendererEngine Class 
//*【?】サンプラーの初期化
//* 引数：なし
//* 戻値：成功したか
//*----------------------------------------------------------------------------------------
HRESULT RendererEngine::InitDX11_Sampler()
{
    /* サンプラーの設定 */

    HRESULT hr = S_OK;

    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));

    /* サンプリングするときに使用するフィルター方法 */
    // D3D11_FILTER_MIN_MAG_MIP_POINT   そのまま
    // D3D11_FILTER_MIN_MAG_MIP_LINEAR  色の平均値(ぼやける)
    // D3D11_FILTER_ANISOTROPIC         異方性フィルタ(3Dの時はこれ使う)

    sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;  // 異方性フィルタ

    /* 0～1の範囲外のu v w テクスチャ座標の解決方法 */
    // D3D11_TEXTURE_ADDRESS_WRAP = 1,       繰り返し
    // D3D11_TEXTURE_ADDRESS_MIRROR = 2,     反転
    // D3D11_TEXTURE_ADDRESS_CLAMP = 3,      最後のピクセルを繰り返す
    // D3D11_TEXTURE_ADDRESS_BORDER = 4,     自分で色を設定する
    // D3D11_TEXTURE_ADDRESS_MIRROR_ONCE = 5 一回だけ反転させる
    sampDesc.AddressU       = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressV       = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressW       = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;   // サンプリングされたデータの比較方法
    sampDesc.MinLOD         = 0;
    sampDesc.MaxLOD         = D3D11_FLOAT32_MAX;
    sampDesc.MipLODBias     = 0.0f;
    sampDesc.MaxAnisotropy  = 16;
    // 作成
    hr = m_pd3dDevice->CreateSamplerState(&sampDesc, &m_pSamplerLinear);
    if (FAILED(hr))return hr;

    return hr;
}


//*---------------------------------------------------------------------------------------
//* @:RendererEngine Class 
//*【?】ブレンドステートの初期化
//* 引数：なし
//* 戻値：成功したか
//*----------------------------------------------------------------------------------------
//HRESULT RendererEngine::InitDX11_BlendState()
//{
//    /* ブレンドステート設定 */
//
//    HRESULT hr = S_OK;
//
//    D3D11_BLEND_DESC blendDesc;
//    ZeroMemory(&blendDesc, sizeof(blendDesc));
//    blendDesc.AlphaToCoverageEnable       = FALSE;  // α値をカバレッジマスクに変換するか(高密度の葉のようなものに使用するといいらしい？)
//    blendDesc.IndependentBlendEnable      = FALSE;  // 複数のレンダーターゲットでブレンド設定を独立させるかどうか
//    blendDesc.RenderTarget[0].BlendEnable = TRUE;   // そのレンダーターゲットのブレンド設定を有効にするかどうか
//
//    // αブレンド
//    blendDesc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;
//    blendDesc.RenderTarget[0].DestBlend             = D3D11_BLEND_INV_SRC_ALPHA;
//    blendDesc.RenderTarget[0].SrcBlend              = D3D11_BLEND_SRC_ALPHA;
//    blendDesc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;
//    blendDesc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ZERO;
//    blendDesc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ONE;
//    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
//    hr = m_pd3dDevice->CreateBlendState(&blendDesc, &m_pBlendStateAlpha);
//    if (FAILED(hr))return hr;
//
//    // 加算ブレンド
//    blendDesc.RenderTarget[0].BlendOp   = D3D11_BLEND_OP_ADD;
//    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
//    blendDesc.RenderTarget[0].SrcBlend  = D3D11_BLEND_SRC_ALPHA;
//    m_pd3dDevice->CreateBlendState(&blendDesc, &m_pBlendStateAdd);
//    if (FAILED(hr))return hr;
//
//    // 減算ブレンド
//    blendDesc.RenderTarget[0].BlendOp   = D3D11_BLEND_OP_SUBTRACT;
//    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
//    blendDesc.RenderTarget[0].SrcBlend  = D3D11_BLEND_SRC_ALPHA;
//    m_pd3dDevice->CreateBlendState(&blendDesc, &m_pBlendStateSub);
//    if (FAILED(hr))return hr;
//
//    // ブレンドステートを設定 ( ここではとりまαブレンドで )
//    float blendFactor[4]{ 0.0f,0.0f,0.0f,0.0f };
//    m_pImmediateContext->OMSetBlendState(m_pBlendStateAlpha, blendFactor, 0xffffffff);
//
//    return hr;
//}
//

//*---------------------------------------------------------------------------------------
//* @:RendererEngine Class 
//*【?】リソース解放
//* 引数：なし
//* 戻値：void
//*----------------------------------------------------------------------------------------
void RendererEngine::CleanupDX11()
{
    if (m_pImmediateContext) m_pImmediateContext->ClearState();
    SAFE_RELEASE(m_pImmediateContext);
    SAFE_RELEASE(m_pRasterState);
    SAFE_RELEASE(m_pDepthStencilState);
    SAFE_RELEASE(m_pSamplerLinear);
    //SAFE_RELEASE(m_pBlendStateAlpha);
    //SAFE_RELEASE(m_pBlendStateAdd);
    //SAFE_RELEASE(m_pBlendStateSub);
    SAFE_RELEASE(m_pVertexBuffer);
    SAFE_RELEASE(m_pDepthStencil);
    SAFE_RELEASE(m_pDepthStencilView);
    SAFE_RELEASE(m_pRenderTargetView);
    SAFE_RELEASE(m_pSwapChain);
    SAFE_RELEASE(m_pd3dDevice)
}

//*---------------------------------------------------------------------------------------
//* @:RendererEngine Class 
//*【?】ビューポートの設定
//* 引数：1.描画範囲の左上Ｘ座標、
//* 引数：2.描画範囲の左上Ｙ座標
//* 引数：3.描画範囲の横幅
//* 引数：4.描画範囲の縦幅
//* 戻値：void
//*----------------------------------------------------------------------------------------
void RendererEngine::set_ViewPort(UINT _topLeftX, UINT _topLeftY, UINT _width, UINT _height)
{
    D3D11_VIEWPORT vp;
    ZeroMemory(&vp, sizeof(vp));
    vp.Width = (FLOAT)_width;   // 描画範囲の横幅       
    vp.Height = (FLOAT)_height; // 描画範囲の縦幅
    vp.MinDepth = 0.0f;                // Ｚバッファの最小値
    vp.MaxDepth = 1.0f;                // Ｚバッファの最大値
    vp.TopLeftX = _topLeftX;                // 描画範囲の左側Ｘ座標
    vp.TopLeftY = _topLeftY;                // 描画範囲の上部Ｙ座標
    m_pImmediateContext->RSSetViewports(1, &vp);
}


//*---------------------------------------------------------------------------------------
//* @:RendererEngine Class 
//*【?】透視投影変換計算
//* 引数：なし
//* 戻値：成功したか
//*----------------------------------------------------------------------------------------
bool RendererEngine::SetupProjectionTransform(float _w, float _h)
{
    auto pDeviceContext = get_DeviceContext();

    // 遠近投影マトリクス作成 https://learn.microsoft.com/ja-jp/windows/win32/api/directxmath/nf-directxmath-xmmatrixperspectivefovlh
    XMMATRIX mat = XMMatrixPerspectiveFovLH(
        m_Fov,
        static_cast<float>(_w) / static_cast<float>(_h), // アスペクト比
        m_NearClipDist,
        m_FarClipDist
    );    

    // 保持
    m_Proj = mat;

    mat = XMMatrixTranspose(mat);   // 転置

    auto& cb = m_RenderParam.cbProjectionSet;
    XMStoreFloat4x4(&cb.Data.Projection, mat);

    // サブリソース
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    ZeroMemory(&mappedResource, sizeof(mappedResource));

    // CPUアクセス
    HRESULT hr = pDeviceContext->Map(
        cb.pBuff,
        0,
        D3D11_MAP_WRITE_DISCARD,
        0,
        &mappedResource
    );

    if (FAILED(hr))return false;

    CopyMemory(mappedResource.pData, &cb.Data, sizeof(cb.Data));

    // マップ解除
    pDeviceContext->Unmap(cb.pBuff, 0);

    // VSにProjectionMatrixをセット(ここで1度セットして以後不変)
    pDeviceContext->VSSetConstantBuffers(2, 1, &cb.pBuff);

    return true;
}


//*---------------------------------------------------------------------------------------
//* @:RendererEngine Class 
//*【?】ビュー変換更新
//* 引数：ビュー行列
//* 戻値：成功したか
//*----------------------------------------------------------------------------------------
bool RendererEngine::SetupViewTransform(const XMMATRIX& viewMat)
{
    auto pDeviceContext = get_DeviceContext();

    // ビュー行列保持
    m_View = viewMat;

    auto& cb = m_RenderParam.cbViewSet;
    XMStoreFloat4x4(&cb.Data.View, XMMatrixTranspose(viewMat));

    // ビュープロジェクション逆行列取得
    cb.Data.ViewProjInvMatrix = get_ViewProjectionInvMatrix();

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    ZeroMemory(&mappedResource, sizeof(mappedResource));
    // CPUアクセス
    HRESULT hr = pDeviceContext->Map(
        cb.pBuff,
        0,
        D3D11_MAP_WRITE_DISCARD,
        0,
        &mappedResource
    );

    if (FAILED(hr))return false;

    CopyMemory(mappedResource.pData, &cb.Data, sizeof(cb.Data));

    // マップ解除
    pDeviceContext->Unmap(cb.pBuff, 0);

    // VSにViewMatrixをセット
    pDeviceContext->VSSetConstantBuffers(1, 1, &cb.pBuff);

    // ピクセルシェーダにもセット！！
    pDeviceContext->PSSetConstantBuffers(1, 1, &cb.pBuff);

    return true;
}

//*---------------------------------------------------------------------------------------
//* @:RendererEngine Class 
//*【?】ビュープロジェクション行列を取得
//* 引数：なし
//* 戻値：void
//*----------------------------------------------------------------------------------------
XMMATRIX RendererEngine::get_ViewProjectionMatrix() const
{
    XMMATRIX view = m_View;
    XMMATRIX proj = m_Proj;
    return XMMatrixMultiply(view, proj);
}


//*---------------------------------------------------------------------------------------
//* @:RendererEngine Class 
//*【?】ビュープロジェクション行列の逆行列取得
//* 引数：なし
//* 戻値：void
//*----------------------------------------------------------------------------------------
XMFLOAT4X4 RendererEngine::get_ViewProjectionInvMatrix()
{
    XMMATRIX vp = get_ViewProjectionMatrix();
    XMFLOAT4X4 res{};

    // 転置逆行列
    XMStoreFloat4x4(&res, XMMatrixTranspose(XMMatrixInverse(NULL, vp)));
    return res;
}


//*---------------------------------------------------------------------------------------
//* @:RendererEngine Class 
//*【?】ビュー行列の行列取得
//* 引数：なし
//* 戻値：行列
//*----------------------------------------------------------------------------------------
XMMATRIX RendererEngine::get_ViewMatrix()const
{
    return m_View;
}

//*---------------------------------------------------------------------------------------
//* @:RendererEngine Class 
//*【?】ビュー行列の逆行列取得
//* 引数：なし
//* 戻値：逆行列
//*----------------------------------------------------------------------------------------
XMMATRIX RendererEngine::get_ViewInvMatrix()const
{
    return XMMatrixInverse(NULL, m_View);
}


//*---------------------------------------------------------------------------------------
//* @:RendererEngine Class 
//*【?】レンダーターゲットをフレームバッファに変更
//* 引数：なし
//* 戻値：void
//*----------------------------------------------------------------------------------------
void RendererEngine::ChangeRenderTargetFrameBuffer()
{
    m_pImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);
}

//*---------------------------------------------------------------------------------------
//* @:RendererEngine Class 
//*【?】レンダーターゲットをフレームバッファに変更
//      デプスステンシルびゅーを指定可能
//* 引数：なし
//* 戻値：void
//*----------------------------------------------------------------------------------------
void RendererEngine::ChangeRenderTargetFrameBuffer(ID3D11DepthStencilView* pDsv)
{
    m_pImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView, pDsv);
}


//*---------------------------------------------------------------------------------------
//* @:RendererEngine Class 
//*【?】レンダーターゲットを解除しNULL設定
//* 引数：なし
//* 戻値：void
//*----------------------------------------------------------------------------------------
void RendererEngine::ReleaseRenderTargetSetNull()
{
    m_pImmediateContext->OMSetRenderTargets(0, nullptr, nullptr);
}

//*---------------------------------------------------------------------------------------
//* @:RendererEngine Class 
//*【?】MRTレンダーターゲットの登録
//* 引数：1.ターゲットの数
//* 引数：2.ターゲットの配列
//* 戻値：void
//*----------------------------------------------------------------------------------------
void RendererEngine::RegisterRenderTargets(UINT num, class DX_RenderTarget *renderTargets[])
{
    ID3D11RenderTargetView *rtv[16]{};

    for (UINT i = 0; i < num; i++) {
        if (renderTargets[i]->get_RTV() != nullptr) {
            rtv[i] = renderTargets[i]->get_RTV();
        }
    }

    if (renderTargets[3]->HasDepthStencilBuffer()) {
        //深度バッファがある。
        m_pImmediateContext->OMSetRenderTargets(num, rtv, renderTargets[3]->get_DSV());
    }
    else  {
        //深度バッファがない。
        m_pImmediateContext->OMSetRenderTargets(num, rtv, nullptr);
    }
}

//*---------------------------------------------------------------------------------------
//* @:RendererEngine Class 
//*【?】単一レンダーターゲットの登録
//* 引数：1.レンダーターゲットビュー
//* 引数：2.デプスステンシルビュー
//* 戻値：void
//*----------------------------------------------------------------------------------------
void RendererEngine::RegisterRenderTarget(ID3D11RenderTargetView* pRtv, ID3D11DepthStencilView* pDsv)
{
    m_pImmediateContext->OMSetRenderTargets(1, &pRtv, pDsv);
}


//*---------------------------------------------------------------------------------------
//* @:RendererEngine Class 
//*【?】レンダーターゲットのクリア
//* 引数：1.ターゲットの数
//* 引数：2.ターゲットの配列
//* 戻値：void
//*----------------------------------------------------------------------------------------
void RendererEngine::ClearRenderTargetViews(UINT num, class DX_RenderTarget *renderTargets[])
{
    if (renderTargets[3]->HasDepthStencilBuffer()) {
        // デプスステンシルバッファがあるならクリア 
        m_pImmediateContext->ClearDepthStencilView(renderTargets[3]->get_DSV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0, 0);
    }

    for (UINT i = 0; i < num; i++) {
        if (renderTargets[i]->get_RTV() != nullptr) {
            // ターゲットのクリア
            m_pImmediateContext->ClearRenderTargetView(renderTargets[i]->get_RTV(), renderTargets[i]->get_ClearColor());
        }
    }
}


//*---------------------------------------------------------------------------------------
//* @:RendererEngine Class 
//*【?】単一のレンダーターゲットのクリア
//* 引数：1.ターゲット
//* 戻値：void
//*----------------------------------------------------------------------------------------
void RendererEngine::ClearRenderTargetView(class DX_RenderTarget* pRT)
{
    if (pRT->HasDepthStencilBuffer()){
        // デプスステンシルバッファがあるならクリア
        m_pImmediateContext->ClearDepthStencilView(pRT->get_DSV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0, 0);
    }

    if (pRT->get_RTV() != nullptr) {
        // ターゲットのクリア
        m_pImmediateContext->ClearRenderTargetView(pRT->get_RTV(), pRT->get_ClearColor());
    }
}



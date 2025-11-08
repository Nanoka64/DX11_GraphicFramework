//--------------------------------------------------------------------------------------
//      * Includes *
//--------------------------------------------------------------------------------------
#include "pch.h"
#include "RendererManager.h"
#include "MyStruct.h"
#include "Path.h"
#include <string>

//--------------------------------------------------------------------------------------
//      * RendererManager Class - デストラクタ - *
//--------------------------------------------------------------------------------------
RendererManager::RendererManager():
    m_driverType(D3D_DRIVER_TYPE_UNKNOWN),
    m_featureLevel(D3D_FEATURE_LEVEL_11_0),
    m_pd3dDevice(nullptr),
    m_pImmediateContext(nullptr),
    m_pSwapChain(nullptr),
    m_pRenderTargetView(nullptr),
    m_pDepthStencil(nullptr),
    m_pDepthStencilView(nullptr),
    m_pVertexBuffer(nullptr),
    m_pCBNeverChanges(nullptr),
    m_pCBChangeOnResize(nullptr),
    m_pCBChangesEveryFrame(nullptr),
    m_pSamplerLinear(nullptr),
    m_pRasterState(nullptr),
    m_pDepthStencilState(nullptr),
    m_pBlendStateAlpha(nullptr),
    m_pBlendStateAdd(nullptr),
    m_pBlendStateSub(nullptr),
    m_ScreenWidht(0),
    m_Screenheight(0),
    m_hWnd(0),
    test(VECTOR3::VEC3(0,0,0),0.25f),
    m_StartTime(0ul),
    m_NearClipDist(0.1f),
    m_FarClipDist(1000.f),
    m_Fov(XMConvertToRadians(30.0f))
{
}


//--------------------------------------------------------------------------------------
//      * RendererManager Class - デストラクタ - *
//--------------------------------------------------------------------------------------
RendererManager::~RendererManager()
{

}


//--------------------------------------------------------------------------------------
//      * RendererManager Class - 初期化 - *
//--------------------------------------------------------------------------------------
bool RendererManager::Init(HWND hWnd)
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
    m_pCBNeverChanges       = NULL;        // コンスタントバッファの実体
    m_pCBChangeOnResize     = NULL;        // ...
    m_pCBChangesEveryFrame  = NULL;        // ...
    m_pSamplerLinear        = NULL;        // テクスチャからどうピクセルをもらうか、サンプルをどうするか
    m_pRasterState          = NULL;        // どこを塗るのか決める(実際には塗るのはピクセルシェーダ)
    m_pDepthStencilState    = NULL;        // Z比較をするための設定
    //m_World;                               // 行列   
    //m_View;                                // ...
    //m_Projection;                          // ...
    m_pBlendStateAlpha = NULL;        // αブレンド用
    m_pBlendStateAdd   = NULL;        // 加算合成用
    m_pBlendStateSub   = NULL;        // 減算合成用

    m_NearClipDist = 0.1f;
    m_FarClipDist  = 10000.0f;
    m_Fov = XMConvertToRadians(30.0f);


    m_hWnd = hWnd;  // ウインドウハンドル受け取る

    RECT rc;
    GetClientRect(m_hWnd, &rc); // ウインドウの黒い部分の大きさを返す
    m_ScreenWidht = rc.right - rc.left;
    m_Screenheight = rc.bottom - rc.top;


    // 正常に初期化されたか
    if (!InitDx11())
    {
        CleanupDX11();  // されて無ければ解放
        return false;
    }


    // 初期化
    m_RenderParam.Init(*this);

    // ポリゴンの頂点バッファの作成
    //test.CreateVertexBuffer(*this);

    m_StartTime = timeGetTime();    // 開始時間取得

    SetupProjectionTransform();

    return true;
}

//--------------------------------------------------------------------------------------
//      * RendererManager Class - 描画 - *
//--------------------------------------------------------------------------------------
void RendererManager::Draw()
{
    if (!m_pImmediateContext || !m_pRenderTargetView) return;
    {
        //// コンスタントバッファ更新
        //CBNeverChanges cbNeverChanges;
        //CBChangeOnResize cbChangeOnResize;
        //CBChangesEveryFrame cbChangeEveryFrame;

        //cbNeverChanges.mView = XMMatrixTranspose(m_View);   // ビュー行列の転置

        //cbChangeOnResize.mProjection = XMMatrixTranspose(m_Projection); // プロジェクション行列の転置

        //cbChangeEveryFrame.mWorld = XMMatrixTranspose(m_World); // ワールド行列の転置
        ////cbChangeEveryFrame.Vertex = BASE_VERTEX::VERTEX(VECTOR3::VEC3(0, 0, 0), VECTOR3::VEC3(0, 0, 0), VECTOR4::VEC4(0, 0, 0, 1), VECTOR2::VEC2(0, 0));
        //cbChangeEveryFrame.Time   = (timeGetTime() - m_StartTime) / 1000.0f;

        //m_pImmediateContext->UpdateSubresource(m_pCBNeverChanges,      0, NULL, &cbNeverChanges, 0, 0);
        //m_pImmediateContext->UpdateSubresource(m_pCBChangeOnResize,    0, NULL, &cbChangeOnResize, 0, 0);
        //m_pImmediateContext->UpdateSubresource(m_pCBChangesEveryFrame, 0, NULL, &cbChangeEveryFrame, 0, 0);

        //m_pImmediateContext->PSSetConstantBuffers(0, 1, &m_pCBChangesEveryFrame);

        //m_pImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView, NULL); // 出力先ビューの設定
        //float color[] = { 0.f,0.f,0.f,0.f };
        //m_pImmediateContext->ClearRenderTargetView(m_pRenderTargetView, color);

        //m_pImmediateContext->IASetInputLayout(m_Shader.m_pVertexLayout);
        //m_pImmediateContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        //m_pImmediateContext->VSSetShader(m_Shader.m_pVertexShader, NULL, 0);
        //m_pImmediateContext->PSSetShader(m_Shader.m_pPixelShader, NULL, 0);

        //// ポリゴンのカラーアニメーション
        //double t = cbChangeEveryFrame.Time;
        //test.vertex[0].color = VECTOR4::VEC4(1, 0, 0, (std::max)(0.0, (abs(sin(t * 3.0)))));
        //test.vertex[1].color = VECTOR4::VEC4(0, 1, 0, (std::max)(0.0, (abs(sin(t * 3.0)))));
        //test.vertex[2].color = VECTOR4::VEC4(0, 0, 1, (std::max)(0.0, (abs(sin(t * 3.0)))));

        // 表示
        //test.Draw(*this);
    }


    // 描画ターゲット (深度ステンシルビューもここに入れる)
    m_pImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

    // 描画ターゲットビュークリア
    float color[] = { 0.15f, 0.3f, 0.3f, 0.f };
    m_pImmediateContext->ClearRenderTargetView(m_pRenderTargetView, color);

    // 深度ステンシルビューをクリア
    m_pImmediateContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    // OMにブレンドステートオブジェクトを設定
    FLOAT BlendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
    m_pImmediateContext->OMSetBlendState(m_pBlendStateAlpha, nullptr, 1);   
    
    // 深度ステンシルステート設定
    m_pImmediateContext->OMSetDepthStencilState(m_pDepthStencilState, 0);   

    // ラスタライザ設定
    m_pImmediateContext->RSSetState(m_pRasterState); 

    //// レイアウト
    //m_pImmediateContext->IASetInputLayout(m_Shader.m_pVertexLayout);
    //
    //// シェーダ設定
    //m_pImmediateContext->VSSetShader(m_Shader.m_pVertexShader, nullptr, 0);
    //m_pImmediateContext->PSSetShader(m_Shader.m_pPixelShader, nullptr, 0);

    // サンプラー設定
    m_pImmediateContext->PSSetSamplers(0, 1, &m_pSamplerLinear);
}


//--------------------------------------------------------------------------------------
//      * RendererManager Class - 終了 - *
//--------------------------------------------------------------------------------------
void RendererManager::Term()
{
    CleanupDX11();  // リソース解放
}


//--------------------------------------------------------------------------------------
//      * RendererManager Class - スワップ・裏表切り替え - *
//--------------------------------------------------------------------------------------
void RendererManager::Swap()
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
//      * RendererManager Class - デバッグレイヤーの有効化 - *
//      https://zenn.dev/kd_gamegikenblg/articles/431326b7f0cb2b
//--------------------------------------------------------------------------------------
void RendererManager::EnableDebugLayer()
{
    ID3D11Debug* debugLayer = nullptr;
}

//--------------------------------------------------------------------------------------
//      * RendererManager Class - ＤｉｒｅｃｔＸ１１初期化 - *
//--------------------------------------------------------------------------------------
bool RendererManager::InitDx11()
{
    // ＤｉｒｅｃｔＸ１１ 各初期化
    if (FAILED(InitDX11_SwapChain()))       return false;
    if (FAILED(InitDX11_RenderTargetView()))return false;
    if (FAILED(InitDX11_ZBuff()))           return false;
    if (FAILED(InitDX11_Rasterizer()))      return false;
    if (FAILED(InitDX11_BuffResource()))    return false;
    if (FAILED(InitShader()))               return false;
    if (FAILED(InitDX11_Sampler()))         return false;
    if (FAILED(InitDX11_BlendState()))      return false;
    if (FAILED(InitDX11_ConstantBuff()))    return false;

    return true;

}


//--------------------------------------------------------------------------------------
//      * RendererManager Class - スワップチェイン 初期化 - *
//--------------------------------------------------------------------------------------
HRESULT RendererManager::InitDX11_SwapChain()
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


//--------------------------------------------------------------------------------------
//      * RendererManager Class - レンダーターゲットビューの 初期化 - *
//--------------------------------------------------------------------------------------
HRESULT RendererManager::InitDX11_RenderTargetView()
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


//--------------------------------------------------------------------------------------
//      * RendererManager Class - Ｚバッファ(デプスバッファ) 初期化 - *
//--------------------------------------------------------------------------------------
HRESULT RendererManager::InitDX11_ZBuff()
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
    m_pImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);


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


//--------------------------------------------------------------------------------------
//      * RendererManager Class - ラスタライザ 初期化 - *
//--------------------------------------------------------------------------------------
HRESULT RendererManager::InitDX11_Rasterizer()
{
    /* ラスタライザにどんな処理をしてもらうか */

    HRESULT hr = S_OK;

    D3D11_RASTERIZER_DESC rd;
    ZeroMemory(&rd, sizeof(rd));

    // ※ CullMode
    // D3D11_CULL_NONE  = 1,      // カリングしない(重い)
    // D3D11_CULL_FRONT = 2,      // 表はカリング(時計回り)
    // D3D11_CULL_BACK  = 3       // 裏はカリング(逆時計回り)
    rd.CullMode              = D3D11_CULL_BACK;
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


//--------------------------------------------------------------------------------------
//      * RendererManager Class - バッファリソース 初期化 - *
//--------------------------------------------------------------------------------------
HRESULT RendererManager::InitDX11_BuffResource()
{
    /* バッファリソースの設定 */

    HRESULT hr = S_OK;

    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));

    // バッファの作成
    bd.Usage          = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth      = sizeof(SimpleVertex) * 6;
    bd.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    hr = m_pd3dDevice->CreateBuffer(&bd, NULL, &m_pVertexBuffer);

    // 頂点バッファをセット
    UINT stride = sizeof(SimpleVertex);
    UINT offset = 0;
    m_pImmediateContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

    // どういう形式で書くか(これは三角形を組み合わせる)
    // LINEとかはデバッグ表示で書くとよい
    m_pImmediateContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    /* RenderParamに移動 */
    {
        //// コンスタントバッファの作成
        //bd.Usage          = D3D11_USAGE_DEFAULT;
        //bd.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
        //bd.CPUAccessFlags = 0;

        //bd.ByteWidth = sizeof(CBNeverChanges);
        //hr = m_pd3dDevice->CreateBuffer(&bd, NULL, &m_pCBNeverChanges);
        //if (FAILED(hr))return hr;

        //bd.ByteWidth = sizeof(CBChangeOnResize);
        //hr = m_pd3dDevice->CreateBuffer(&bd, NULL, &m_pCBChangeOnResize);
        //if (FAILED(hr))return hr;

        //bd.ByteWidth = sizeof(CBChangesEveryFrame);
        //hr = m_pd3dDevice->CreateBuffer(&bd, NULL, &m_pCBChangesEveryFrame);
        //if (FAILED(hr))return hr;
    }

    return hr;
}


//--------------------------------------------------------------------------------------
//      * RendererManager Class - サンプラー 初期化 - *
//--------------------------------------------------------------------------------------
HRESULT RendererManager::InitDX11_Sampler()
{
    /* サンプラーの設定 */

    HRESULT hr = S_OK;

    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));

    /* サンプリングするときに使用するフィルター方法 */
    // D3D11_FILTER_MIN_MAm_MIP_POINT   そのまま
    // D3D11_FILTER_MIN_MAm_MIP_LINEAR  色の平均値(ぼやける)
    // D3D11_FILTER_ANISOTROPIC         異方性フィルタ(3Dの時はこれ使う)

    sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;  // 異方性フィルタ

    /* 0～1の範囲外のu v w テクスチャ座標の解決方法 */
    // D3D11_TEXTURE_ADDRESS_WRAP = 1,       繰り返し
    // D3D11_TEXTURE_ADDRESS_MIRROR = 2,     反転 ( ここではこれ )
    // D3D11_TEXTURE_ADDRESS_CLAMP = 3,      最後のピクセルを繰り返す
    // D3D11_TEXTURE_ADDRESS_BORDER = 4,     自分で色を設定する
    // D3D11_TEXTURE_ADDRESS_MIRROR_ONCE = 5 一回だけ反転させる
    sampDesc.AddressU       = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV       = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW       = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;   // サンプリングされたデータの比較方法
    sampDesc.MinLOD         = 0;
    sampDesc.MaxLOD         = D3D11_FLOAT32_MAX;
    sampDesc.MaxAnisotropy = 16;
    // 作成
    hr = m_pd3dDevice->CreateSamplerState(&sampDesc, &m_pSamplerLinear);
    if (FAILED(hr))return hr;

    return hr;
}


//--------------------------------------------------------------------------------------
//      * RendererManager Class - ブレンドステート 初期化 - *
//--------------------------------------------------------------------------------------
HRESULT RendererManager::InitDX11_BlendState()
{
    /* ブレンドステート設定 */

    HRESULT hr = S_OK;

    D3D11_BLEND_DESC blendDesc;
    ZeroMemory(&blendDesc, sizeof(blendDesc));
    blendDesc.AlphaToCoverageEnable       = FALSE;  // α値をカバレッジマスクに変換するか(高密度の葉のようなものに使用するといいらしい？)
    blendDesc.IndependentBlendEnable      = FALSE;  // 複数のレンダーターゲットでブレンド設定を独立させるかどうか
    blendDesc.RenderTarget[0].BlendEnable = TRUE;   // そのレンダーターゲットのブレンド設定を有効にするかどうか

    // αブレンド
    blendDesc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].DestBlend             = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].SrcBlend              = D3D11_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ZERO;
    blendDesc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    hr = m_pd3dDevice->CreateBlendState(&blendDesc, &m_pBlendStateAlpha);
    if (FAILED(hr))return hr;

    // 加算ブレンド
    blendDesc.RenderTarget[0].BlendOp   = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].SrcBlend  = D3D11_BLEND_SRC_ALPHA;
    m_pd3dDevice->CreateBlendState(&blendDesc, &m_pBlendStateAdd);
    if (FAILED(hr))return hr;

    // 減算ブレンド
    blendDesc.RenderTarget[0].BlendOp   = D3D11_BLEND_OP_SUBTRACT;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].SrcBlend  = D3D11_BLEND_SRC_ALPHA;
    m_pd3dDevice->CreateBlendState(&blendDesc, &m_pBlendStateSub);
    if (FAILED(hr))return hr;

    // ブレンドステートを設定 ( ここではとりまαブレンドで )
    float blendFactor[4]{ 0.0f,0.0f,0.0f,0.0f };
    m_pImmediateContext->OMSetBlendState(m_pBlendStateAlpha, blendFactor, 0xffffffff);

    return hr;
}


//--------------------------------------------------------------------------------------
//      * RendererManager Class - 定数バッファ 初期化 - *
//--------------------------------------------------------------------------------------
HRESULT RendererManager::InitDX11_ConstantBuff()
{
    /* マトリクスとコンスタントバッファの設定 */

    HRESULT hr = S_OK;

    //m_World = XMMatrixIdentity();   // ワールド行列を単位行列で初期化
    //m_View  = XMMatrixIdentity();   // ビュー行列を単位行列で初期化
    //m_Projection =                  // プロジェクション行列を正射影で設定
    //    XMMatrixOrthographicOffCenterLH(0.0f, (float)m_ScreenWidht, (float)m_Screenheight, 0.0f, 0.0f, 1.0f);

    //// コンスタントバッファ初期化
    //CBNeverChanges cbNeverChanges;
    //cbNeverChanges.mView = XMMatrixTranspose(m_View);   // ビュー行列の転置
    //m_pImmediateContext->UpdateSubresource(m_pCBNeverChanges, 0, NULL, &cbNeverChanges, 0, 0);

    //CBChangeOnResize cbChangeOnResize;
    //cbChangeOnResize.mProjection = XMMatrixTranspose(m_Projection); // プロジェクション行列の転置
    //m_pImmediateContext->UpdateSubresource(m_pCBChangeOnResize, 0, NULL, &cbChangeOnResize, 0, 0);

    //CBChangesEveryFrame cbChangeEveryFrame;
    //cbChangeEveryFrame.mWorld     = XMMatrixTranspose(m_World); // ワールド行列の転置
    ////cbChangeEveryFrame.Vertex     = BASE_VERTEX::VERTEX(VECTOR3::VEC3(0, 0, 0), VECTOR3::VEC3(0, 0, 0), VECTOR4::VEC4(1, 0, 0, 1), VECTOR2::VEC2(0, 0));
    //cbChangeEveryFrame.Time       = m_StartTime;
    //m_pImmediateContext->UpdateSubresource(m_pCBChangesEveryFrame, 0, NULL, &cbChangeEveryFrame, 0, 0);

    //// 頂点シェーダにコンスタントバッファをセット
    //m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pCBNeverChanges);
    //m_pImmediateContext->VSSetConstantBuffers(1, 1, &m_pCBChangeOnResize);
    //m_pImmediateContext->VSSetConstantBuffers(2, 1, &m_pCBChangesEveryFrame);

    //// ピクセルシェーダにセット
    //m_pImmediateContext->PSSetConstantBuffers(0, 1, &m_pCBChangesEveryFrame);

    return hr;
}


//--------------------------------------------------------------------------------------
//      * RendererManager Class - シェーダーの初期化 - *
//--------------------------------------------------------------------------------------
HRESULT RendererManager::InitShader()
{
    HRESULT hr;

    ID3D11VertexShader* pVertexShader = nullptr;  // 頂点シェーダ
    ID3D11PixelShader*  pPixelShader  = nullptr;  // ピクセルシェーダ
    ID3D11InputLayout*  pVertexLayout = nullptr;  // 頂点フォーマットの設定

    // ファイルパス
    std::wstring filePath[2] = { 
        std::wstring(Path::Shader) + L"VS.hlsl",
        std::wstring(Path::Shader) + L"PS.hlsl" 
    };

    //         
    /* 頂点シェーダー */
    // 
    ID3DBlob* pVSBlob = NULL;

    // 頂点シェーダーのコンパイル
    hr = CompileShaderFromFile(filePath[0].c_str(), "VS", "vs_5_0", &pVSBlob);
    if (FAILED(hr))
    {
        MessageBox(NULL, "頂点シェーダーがコンパイルできませんでした", "Error", MB_OK);
        return hr;
    }

    // 頂点シェーダーの作成
    hr = m_pd3dDevice->CreateVertexShader(
        pVSBlob->GetBufferPointer(), 
        pVSBlob->GetBufferSize(), 
        NULL, 
        &pVertexShader
    );

    if (FAILED(hr))
    {
        pVSBlob->Release();
        return hr;
    }

    // 入力データ型の配列
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        // ↓頂点シェーダのinputに使ってるセマンティック
        {"POSITION",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, offsetof(BASE_VERTEX::MODEL_VERTEX, pos),   D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL",     0, DXGI_FORMAT_R32G32B32_FLOAT,    0, offsetof(BASE_VERTEX::MODEL_VERTEX, normal),D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR",      0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(BASE_VERTEX::MODEL_VERTEX, color), D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD",   0, DXGI_FORMAT_R32G32_FLOAT,       0, offsetof(BASE_VERTEX::MODEL_VERTEX, uv),    D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"BONEIDS",    0, DXGI_FORMAT_R32G32B32A32_SINT,  0, offsetof(BASE_VERTEX::MODEL_VERTEX, boneIDs),    D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(BASE_VERTEX::MODEL_VERTEX, boneWeights),    D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    UINT numElements = ARRAYSIZE(layout);   // 配列の要素数取得

    // 入力レイアウト作成
    hr = m_pd3dDevice->CreateInputLayout(
        layout,                     // 入力データ型の配列設定    
        numElements,                // 配列の要素数
        pVSBlob->GetBufferPointer(),// コンパイルされたシェーダへのポインタ
        pVSBlob->GetBufferSize(),   // コンパイルされたシェーダのサイズ
        &pVertexLayout              // 生成した入力レイアウトの出力先
    );

    if (FAILED(hr)) return hr;

    // 入力レイアウトのセット
    m_pImmediateContext->IASetInputLayout(pVertexLayout);


    //
    /* ピクセルシェーダー */
    // 
    ID3DBlob* pPSBlob = NULL;

    // ピクセルシェーダーのコンパイル
    hr = CompileShaderFromFile(filePath[1].c_str(), "PS", "ps_5_0", &pPSBlob);
    if (FAILED(hr))
    {
        MessageBox(NULL, "ピクセルシェーダーがコンパイルできませんでした", "Error", MB_OK);
        return hr;
    }

    // ピクセルシェーダーの作成
    hr = m_pd3dDevice->CreatePixelShader(
        pPSBlob->GetBufferPointer(),
        pPSBlob->GetBufferSize(), 
        NULL, 
        &pPixelShader
    );

    pPSBlob->Release();

    if (FAILED(hr))return hr;

    m_Shader.m_pVertexLayout = pVertexLayout;
    m_Shader.m_pVertexShader = pVertexShader;
    m_Shader.m_pPixelShader = pPixelShader;

    return hr;
}


//--------------------------------------------------------------------------------------
//      * RendererManager Class - DX11 解放 - *
//--------------------------------------------------------------------------------------
void RendererManager::CleanupDX11()
{
    if (m_pImmediateContext) m_pImmediateContext->ClearState();
    SAFE_RELEASE(m_pImmediateContext);
    SAFE_RELEASE(m_pRasterState);
    SAFE_RELEASE(m_pDepthStencilState);
    SAFE_RELEASE(m_pSamplerLinear);
    SAFE_RELEASE(m_pBlendStateAlpha);
    SAFE_RELEASE(m_pBlendStateAdd);
    SAFE_RELEASE(m_pBlendStateSub);
    SAFE_RELEASE(m_pCBNeverChanges);
    SAFE_RELEASE(m_pCBChangeOnResize);
    SAFE_RELEASE(m_pCBChangesEveryFrame);
    SAFE_RELEASE(m_pVertexBuffer);
    SAFE_RELEASE(m_pDepthStencil);
    SAFE_RELEASE(m_pDepthStencilView);
    SAFE_RELEASE(m_pRenderTargetView);
    SAFE_RELEASE(m_pSwapChain);
    SAFE_RELEASE(m_pd3dDevice)
}


//--------------------------------------------------------------------------------------
//      * RendererManager Class - シェーダーのコンパイル - *
//--------------------------------------------------------------------------------------
HRESULT RendererManager::CompileShaderFromFile(LPCWSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
    HRESULT hr = S_OK;
    //DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
    DWORD dwShaderFlags = D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR;  // こっちの方が計算が速いらしい https://proglog.site/triangle-transform-on-shader/
    
#if defined(DEBUG) || defined(_DEBUG)
    // D3DCOMPILE_DEBUG フラグを設定して、シェーダーにデバッグ情報を埋め込む。
    // このフラグを設定すると、シェーダーのデバッグ体験が向上するが、それでも
    // シェーダーは最適化され、このプログラムのリリース構成で実行されるのと同じように
    // 実行される。
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob* pErrorBlob;
    hr = D3DCompileFromFile(
        szFileName,         // コンパイルするhlslファイルパス
        NULL,               // hlsl上で事前に定義する#defineの指定
        D3D_COMPILE_STANDARD_FILE_INCLUDE,               // インクルードファイルを取り扱うためのID3DIncludeの指定
        szEntryPoint,       // エントリーポイントとなる関数名指定
        szShaderModel,      // コンパイル時のターゲットとなるシェーダモデル指定
        dwShaderFlags,      // コンパイルオプション
        0,                  // エフェクトファイル用のコンパイルオプション(シェーダのコンパイルの場合は０)
        ppBlobOut,          // コンパイル結果のバイナリコードの格納
        &pErrorBlob         // コンパイル時のError内容の格納
    );

    if (FAILED(hr))
    {
        if (pErrorBlob != NULL)
        {
            OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
            if (pErrorBlob)pErrorBlob->Release();
            return hr;
        }
    }
    if (pErrorBlob)pErrorBlob->Release();

    return S_OK;
}


//--------------------------------------------------------------------------------------
//      * RendererManager Class - 透視投影変換計算 - *
//--------------------------------------------------------------------------------------
bool RendererManager::SetupProjectionTransform()
{
    auto pDeviceContext = get_DeviceContext();

    // 遠近投影マトリクス作成 https://learn.microsoft.com/ja-jp/windows/win32/api/directxmath/nf-directxmath-xmmatrixperspectivefovlh
    XMMATRIX mat = XMMatrixPerspectiveFovLH(
        m_Fov,
        static_cast<float>(m_ScreenWidht) / static_cast<float>(m_Screenheight), // アスペクト比
        m_NearClipDist,
        m_FarClipDist
    );

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


//--------------------------------------------------------------------------------------
//      * RendererManager Class - ビュー変換計算 - *
//--------------------------------------------------------------------------------------
bool RendererManager::SetupViewTransform(const XMMATRIX& viewMat)
{
    auto pDeviceContext = get_DeviceContext();

    auto& cb = m_RenderParam.cbViewSet;
    XMStoreFloat4x4(&cb.Data.View, XMMatrixTranspose(viewMat));

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

    return true;
}



#include "pch.h"
#include "DirectWriteManager.h"
#include "FontConfig.h"
#include "RendererEngine.h"




//--------------------------------------------------------------------------------------
//      * DirectWriteManager Class - コンストラクタ - *
//--------------------------------------------------------------------------------------
DirectWriteManager::DirectWriteManager():
    m_pD2DFactory(nullptr),
    m_pWriteFactory(nullptr),
    m_pTextFormat(nullptr),
    m_pRenderTarget(nullptr),
    m_pSolidBrush(nullptr),
    m_pSurfaceBackBuffer(nullptr)/*,
    m_pFontData(nullptr)*/
{
    //m_pFontData = new FONT_DATA("");  
}


//--------------------------------------------------------------------------------------
//      * DirectWriteManager Class - デストラクタ - *
//--------------------------------------------------------------------------------------
DirectWriteManager::~DirectWriteManager()
{
    Term();
}


//--------------------------------------------------------------------------------------
//      * DirectWriteManager Class - 初期化 - *
//--------------------------------------------------------------------------------------
HRESULT DirectWriteManager::Init(RendererEngine &render)
{
    HRESULT hr = S_OK;

    // Direct2D初期化
    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);
    CHECK_HRESULT(hr);
    
    // バックバッファ取得
    ID3D11Texture2D *pBackBufferTex = nullptr;
    
    // 変換しなくても行けた
    hr = render.get_SwapChain()->GetBuffer(0, IID_PPV_ARGS(&m_pSurfaceBackBuffer));
    CHECK_HRESULT(hr);

    // ID3D11Texture2D を IDXGISurfaceに変換する
    //hr = pBackBufferTex->QueryInterface(IID_PPV_ARGS(&m_pSurfaceBackBuffer));
    //CHECK_HRESULT(hr);


    // DPI を取得
    // dpiはx、yで用意してるけど大体のモニターは正方ピクセルだから同じ
    //m_pD2DFactory->GetDesktopDpi(&dpiX, &dpiY); ←非推奨らしい！！  https://learn.microsoft.com/ja-jp/windows/win32/api/d2d1/nf-d2d1-id2d1factory-getdesktopdpi
    UINT dpi = GetDpiForWindow(render.get_WndHandle()); // ←変わりがこれ
    FLOAT dpiX = static_cast<FLOAT>(dpi);
    FLOAT dpiY = static_cast<FLOAT>(dpi);

    // レンダーターゲットの作成
    D2D1_RENDER_TARGET_PROPERTIES prop = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT,
        D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
        dpiX, dpiY
    ); 
    

    // サーフェスに描画するレンダーターゲット作成
    hr = m_pD2DFactory->CreateDxgiSurfaceRenderTarget(m_pSurfaceBackBuffer, &prop, &m_pRenderTarget);
    CHECK_HRESULT(hr);

    // アンチエイリアシングモードの設定
    m_pRenderTarget->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE);

    // DWriteFactoryの作成
    hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown **>(&m_pWriteFactory));
    CHECK_HRESULT(hr);


    //// フォントフォーマットの作成
    //hr = m_pWriteFactory->CreateTextFormat(
    //    FontNameList[(int)m_pFontData->type],// フォントファミリー名
    //    m_pFontData->fontCollection,         // フォントコレクション
    //    m_pFontData->fontWeight,             // フォントの太さ
    //    m_pFontData->fontStyle,              // フォントスタイル
    //    m_pFontData->fontStretch,            // フォントの幅
    //    m_pFontData->fontSize,               // フォントの大きさ
    //    m_pFontData->localName,              // ローカル名
    //    &m_pTextFormat                       // テキストの書式
    //);
    //CHECK_HRESULT(hr);

    //// テキストの配置、整列
    //// 
    //// 縦書き、横書きで変わる？
    //// DWRITE_TEXT_ALIGNMENT_LEADING   : 先頭 左揃え
    //// DWRITE_TEXT_ALIGNMENT_TRAILING  : 後ろ 右揃え
    //// DWRITE_TEXT_ALIGNMENT_CENTER    : 中央
    //// DWRITE_TEXT_ALIGNMENT_JUSTIFIED : 均等
    //hr = m_pTextFormat->SetTextAlignment(m_pFontData->textAlignment);
    //CHECK_HRESULT(hr);

    //// 色の設定
    //hr = m_pRenderTarget->CreateSolidColorBrush(m_pFontData->color, &m_pSolidBrush);
    //CHECK_HRESULT(hr);

    return hr;
}


//--------------------------------------------------------------------------------------
//      * DirectWriteManager Class - 終了 - *
//                  主に解放
//--------------------------------------------------------------------------------------
void DirectWriteManager::Term()
{
    SAFE_RELEASE(m_pD2DFactory);
    SAFE_RELEASE(m_pWriteFactory);
    SAFE_RELEASE(m_pTextFormat);
    SAFE_RELEASE(m_pRenderTarget);
    SAFE_RELEASE(m_pSolidBrush);
    SAFE_RELEASE(m_pSurfaceBackBuffer);

    //SAFE_DELETE(m_pFontData);
}


//*---------------------------------------------------------------------------------------
//*【?】描画の開始
//* ※ フレームの最初に呼ぶ  
//* [引数]なし
//* [返値]なし
//*----------------------------------------------------------------------------------------
void DirectWriteManager::BeginDraw()
{
    m_pRenderTarget->BeginDraw();
}

//*---------------------------------------------------------------------------------------
//*【?】描画の終了
//* ※ フレームの最後に呼ぶ  
//* [引数]なし
//* [返値]なし
//*----------------------------------------------------------------------------------------
HRESULT DirectWriteManager::EndDraw()
{
    return m_pRenderTarget->EndDraw();
}

//--------------------------------------------------------------------------------------
//      * DirectWriteManager Class - stringをwstringに変換する - *
//--------------------------------------------------------------------------------------
std::wstring DirectWriteManager::StringToWString(std::string oString)
{
    int iBufferSize = MultiByteToWideChar(CP_ACP, 0, oString.c_str(), -1, (wchar_t*)NULL, 0);

    // バッファの取得
    wchar_t *cpUCS2 = new wchar_t[iBufferSize];

    // マルチバイト(string)からワイド文字(wchar)への変換
    MultiByteToWideChar(CP_ACP, 0, oString.c_str(), -1, cpUCS2, iBufferSize);

    // stringの生成
    std::wstring oRet(cpUCS2, cpUCS2 + iBufferSize - 1);

    // バッファの破棄
    delete[] cpUCS2;

    // 変換したものを返す
    return oRet;
}


//--------------------------------------------------------------------------------------
//      * DirectWriteManager Class - 色の設定 - *
//--------------------------------------------------------------------------------------
void DirectWriteManager::SetColor(const D2D1_COLOR_F& col)
{
    // 使用する色の指定
    HRESULT hr = m_pRenderTarget->CreateSolidColorBrush(col, &m_pSolidBrush);
    if (FAILED(hr)) {
        assert(false);
    }
}


//--------------------------------------------------------------------------------------
//      * DirectWriteManager Class - フォント情報を設定 - *
//                  必ず最初らへんに呼ぶ
//--------------------------------------------------------------------------------------
HRESULT DirectWriteManager::SetFontData(FONT_DATA *data)
{
    HRESULT hr = S_OK;

    // 既に登録済みなら返す
    auto it = m_pTextFormatMap.find(data->tag);
    if (it != m_pTextFormatMap.end()){
        MessageBox(NULL, "既に登録済みのフォントです", "DirectWriteManager", MB_OK);
        return S_FALSE;
    }

    //m_pFontData->type = data->type;

    // テキスト書式オブジェクトの作成
    hr = m_pWriteFactory->CreateTextFormat(
        FontNameList[(int)data->type], 
        data->fontCollection,
        data->fontWeight, 
        data->fontStyle, 
        data->fontStretch, 
        data->fontSize, 
        data->localName, 
        m_pTextFormatMap[data->tag].GetAddressOf()    // 指定キー
    );
    CHECK_HRESULT(hr);

    // テキストの配置設定
    hr = m_pTextFormatMap[data->tag]->SetTextAlignment(data->textAlignment);
    CHECK_HRESULT(hr);

    // 使用する色の指定
    hr = m_pRenderTarget->CreateSolidColorBrush(data->color, &m_pSolidBrush);
    CHECK_HRESULT(hr);

    return hr;
}


//--------------------------------------------------------------------------------------
//      * DirectWriteManager Class - 文字列の描画 - *
//       :文字列
//       :座標
//       :整形オプション
//--------------------------------------------------------------------------------------
void DirectWriteManager::DrawString(std::string str, const VECTOR2::VEC2& _pos, const std::string &formatTag,  D2D1_DRAW_TEXT_OPTIONS options)
{
    HRESULT hr = S_OK;
    //return;
    
    Microsoft::WRL::ComPtr<IDWriteTextLayout> pTextLayout;  // テキスト情報

    // ワイド文字に変換
    std::wstring wstr = StringToWString(str);

    // レンダーターゲットのサイズ取得
    D2D1_SIZE_F RVSize = m_pRenderTarget->GetSize();

    // テキストレイアウトの作成
    hr = m_pWriteFactory->CreateTextLayout(
        wstr.c_str(),
        static_cast<UINT32>(wstr.size()),
        m_pTextFormatMap[formatTag].Get(),
        RVSize.width,
        RVSize.height,
        pTextLayout.GetAddressOf()
    );
    CHECK_HRESULT_NO_BOOL(hr);

    // 下線
    DWRITE_TEXT_RANGE textRange = { 0,      // 下線を引く始まりの文字インデックス
                                    static_cast<UINT32>(str.length())};    // 下線の長さ(何文字分か).

    // 下線設定
    hr = pTextLayout->SetUnderline(TRUE, textRange);
    CHECK_HRESULT_NO_BOOL(hr);

    // 描画位置の確定
    D2D1_POINT_2F pos = { 0,0 };
    pos.x = _pos.x;
    pos.y = _pos.y;

    // 描画
    m_pRenderTarget->DrawTextLayout(
        pos,
        pTextLayout.Get(),
        m_pSolidBrush,
        options
    );

}


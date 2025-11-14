#include "pch.h"
#include "RenderTarget.h"
#include "RendererManager.h"


//*---------------------------------------------------------------------------------------
//* @:RenderTarget Class 
//*【?】コンストラクタ
//* 引数：なし
//*----------------------------------------------------------------------------------------
RenderTarget::RenderTarget():
    m_Width(0),
    m_Height(0),
    m_pRenderTargetTexture(nullptr),
    m_pRendertargetView(nullptr),
    m_pShaderResouceView(nullptr)
{
}

//*---------------------------------------------------------------------------------------
//* @:RenderTarget Class 
//*【?】デストラクタ
//*----------------------------------------------------------------------------------------
RenderTarget::~RenderTarget()
{
}

//*---------------------------------------------------------------------------------------
//* @:RenderTarget Class 
//*【?】レンダーターゲットの作成 
//* 引数：1.RendererManager&
//* 引数：2.ターゲットの横幅
//* 引数：3.ターゲットの縦幅
//* 引数：4.ミップマップレベル。0を指定した場合はミップマップがサポートされているGPUでは1*1ピクセルまでのミップマップが作成される。
//* 引数：5.テクスチャ配列のサイズ
//* 引数：6.カラーバッファのフォーマット
//* 引数：7.深度ステンシルバッファのフォーマット
//* 返値：成功したか
//*----------------------------------------------------------------------------------------
bool RenderTarget::Create(RendererManager &renderer, int w, int h, int mipLevel, int arraySize, DXGI_FORMAT colorFormat, DXGI_FORMAT depthStencilFormat, float clearColor[4])
{
    auto pDevice = renderer.get_Device();

    m_Width = w;
    m_Height = h;

    // レンダリングターゲットとなるテクスチャの作成
    if (!CreateRenderTargetTexture(renderer, w, h, mipLevel, arraySize, colorFormat, clearColor))
    {
        MessageBoxA(nullptr, "レンダリングターゲットとなるテクスチャの作成に失敗しました。", "エラー", MB_OK);
        return false;
    }

    // 深度ステンシルバッファの作成
    if (depthStencilFormat != DXGI_FORMAT_UNKNOWN) 
    {
        if (!CreateDepthStencil(renderer, w, h, depthStencilFormat)) {
            MessageBoxA(nullptr, "深度ステンシルバッファとなるテクスチャの作成に失敗しました。", "エラー", MB_OK);
            return false;
        }
    }

    return true;
}


//*---------------------------------------------------------------------------------------
//* @:RenderTarget Class 
//*【?】レンダーターゲットの作成  内部公開
//* 引数：1.RendererManager&
//* 引数：2.ターゲットの横幅
//* 引数：3.ターゲットの縦幅
//* 引数：4.ミップマップレベル。0を指定した場合はミップマップがサポートされているGPUでは1*1ピクセルまでのミップマップが作成される。
//* 引数：5.テクスチャ配列のサイズ
//* 引数：6.カラーバッファのフォーマット
//* 引数：7.深度ステンシルバッファのフォーマット
//* 返値：成功したか
//*----------------------------------------------------------------------------------------
bool RenderTarget::CreateRenderTargetTexture(RendererManager &renderer, int w, int h, int mipLevel, int arraySize, DXGI_FORMAT format, float clearColor[4])
{
    auto pDevice = renderer.get_Device();
    HRESULT hr = S_OK;

    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = w;
    desc.Height = h;
    desc.MipLevels = mipLevel;
    desc.ArraySize = arraySize;
    desc.Format = format;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

    // Texture2D作成
    hr = pDevice->CreateTexture2D(&desc, nullptr, m_pRenderTargetTexture.GetAddressOf());
    if (FAILED(hr)) return false;

    // RTV作成
    hr = pDevice->CreateRenderTargetView(m_pRenderTargetTexture.Get(), nullptr, m_pRendertargetView.GetAddressOf());
    if (FAILED(hr))return false;

    // SRVの作成
    hr = pDevice->CreateShaderResourceView(m_pRenderTargetTexture.Get(), nullptr, m_pShaderResouceView.GetAddressOf());
    if (FAILED(hr))return false;

    return true; // 正常終了
}


//*---------------------------------------------------------------------------------------
//* @:RenderTarget Class 
//*【?】レンダーターゲットの作成  内部公開
//* 引数：1.RendererManager&
//* 引数：2.ターゲットの横幅
//* 引数：3.ターゲットの縦幅
//* 引数：4.テクスチャのフォーマット
//* 返値：成功したか
//*----------------------------------------------------------------------------------------
bool RenderTarget::CreateDepthStencil(RendererManager &renderer, float w, float h, DXGI_FORMAT format)
{
    return true;
}


//*---------------------------------------------------------------------------------------
//* @:RenderTarget Class 
//*【?】レンダーターゲットテクスチャの取得
//* 引数：なし
//* 返値：テクスチャの参照ポインタ
//*----------------------------------------------------------------------------------------
std::weak_ptr<class Texture> RenderTarget::get_RTTexture()const
{
    return {};
}


//*---------------------------------------------------------------------------------------
//* @:RenderTarget Class 
//*【?】横幅の取得
//* 引数：なし
//* 返値：横幅
//*----------------------------------------------------------------------------------------
int RenderTarget::get_Width()const
{
    return m_Width;
}


//*---------------------------------------------------------------------------------------
//* @:RenderTarget Class 
//*【?】縦幅の取得
//* 引数：なし
//* 返値：縦幅
//*----------------------------------------------------------------------------------------
int RenderTarget::get_Height()const
{
    return m_Height;
}


//*---------------------------------------------------------------------------------------
//* @:RenderTarget Class 
//*【?】SRVのComポインタ取得
//* 引数：なし
//* 返値：ComPtr
//*----------------------------------------------------------------------------------------
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> RenderTarget::get_SRV_ComPtr()
{
    //  AddRef がかかってカウンタが増える
    return m_pShaderResouceView; 
}
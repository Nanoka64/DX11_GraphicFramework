#pragma once


// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:RenderTarget Class --- */
//
// 【?】レンダーターゲット
//
// ***************************************************************************************
class DX_RenderTarget
{
private:
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pRenderTargetTexture;             // ターゲットとなるテクスチャ情報
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRendertargetView;         // ビュー
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pShaderResouceView;      // シェーダビュー 

    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pDepthStencilTexture;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;

    int m_Width;
    int m_Height;

    float m_ClearColor[4];

public:
    DX_RenderTarget();
    ~DX_RenderTarget();


    bool Create(
        RendererManager& renderer,
        UINT w,
        UINT h,
        int mipLevel,
        int arraySize,
        DXGI_FORMAT colorFormat,
        DXGI_FORMAT depthStencilFormat,
        float clearColor[4] = nullptr
    );

    /// <summary>
    /// レンダーターゲットテクスチャの取得
    /// </summary>
    /// <returns></returns>
    std::weak_ptr<class Texture> get_RTTexture()const;
    int get_Width()const;
    int get_Height()const;

    
    ID3D11RenderTargetView *get_RTV() { return m_pRendertargetView.Get(); }
    ID3D11ShaderResourceView *get_SRV() { return m_pShaderResouceView.Get(); }
    ID3D11DepthStencilView *get_DSV() { return m_pDepthStencilView.Get(); };

    /// <summary>
    /// SRVのComポインタ取得
    /// </summary>
    /// <returns></returns>
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> get_SRV_ComPtr();


    /// <summary>
    /// デプスステンシルバッファを保持しているか
    /// </summary>
    /// <returns></returns>
    bool HasDepthStencilBuffer()const { return m_pDepthStencilView; };


    /// <summary>
    /// クリアカラーの取得 4要素
    /// </summary>
    /// <returns></returns>
    const float *get_ClearColor()const { return m_ClearColor; };

private:

    /// <summary>
    /// レンダーターゲットテクスチャの作成
    /// </summary>
    /// <param name="renderer"></param>
    /// <param name="w">テクスチャの横幅</param>
    /// <param name="h">テクスチャの縦幅</param>
    /// <param name="mipLevel">ミップマップレベル</param>
    /// <param name="arraySize">テクスチャ配列のサイズ</param>
    /// <param name="format">カラーバッファのフォーマット</param>
    /// <param name="clearColor">深度ステンシルバッファのフォーマット</param>
    /// <returns></returns>
    bool CreateRenderTargetTexture(
        RendererManager &renderer,
        UINT w,
        UINT h,
        int mipLevel,
        int arraySize,
        DXGI_FORMAT format,
        float clearColor[4]
    );


    /// <summary>
    /// デプスステンシルの作成
    /// </summary>
    /// <param name="renderer"></param>
    /// <param name="w">テクスチャの横幅</param>
    /// <param name="h">テクスチャの縦幅</param>
    /// <param name="format">テクスチャのフォーマット</param>
    /// <returns></returns>
    bool CreateDepthStencil(
        RendererManager &renderer,
        UINT w,
        UINT h,
        DXGI_FORMAT format
    );

};


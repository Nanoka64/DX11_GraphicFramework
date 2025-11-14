#pragma once
// =======================================================================================
//
// *---      Texture Class         ---*
//		  DirectXTexでテクスチャの読み込み
//        テクスチャの作成etc...
// 参考サイト：
// https://sites.google.com/view/jc21engine2023/direct3d/3d%E6%8F%8F%E7%94%BB/%E3%83%86%E3%82%AF%E3%82%B9%E3%83%81%E3%83%A3directxtex%E7%89%88
// https://qiita.com/harayuu10/items/88e1a3a7f5535c805052
// https://araramistudio.jimdofree.com/2017/10/18/%E3%83%97%E3%83%AD%E3%82%B0%E3%83%A9%E3%83%9F%E3%83%B3%E3%82%B0-directx-11%E3%81%A7%E3%83%86%E3%82%AF%E3%82%B9%E3%83%81%E3%83%A3%E3%83%9E%E3%83%83%E3%83%94%E3%83%B3%E3%82%B0/
// =======================================================================================
class Texture
{
private:
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pSRV;   // テクスチャをシェーダに渡すためのもの
    std::wstring m_FilePath;

public:
    Texture();
    ~Texture();

    /// <summary>
    /// テクスチャのロード
    /// </summary>
    /// <param name="path">ファイルパス</param>
    /// <param name="renderer"></param>
    /// <returns></returns>
    HRESULT Load(const std::wstring& path, class RendererManager &renderer);
    
    /// <summary>
    /// 外からそのままSRVをセット
    /// Comポインタで渡す
    /// レンダリングターゲットなど
    /// </summary>
    /// <param name="pSrv"></param>
    /// <returns></returns>
    void set_SRV_ComPtr(const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& pSrv);


    void Release();
    
    /// <summary>
    /// ファイルパスの取得
    /// </summary>
    /// <returns></returns>
    std::wstring get_FilePath()const { return m_FilePath; }
    
    /// <summary>
    /// SRビューの取得
    /// </summary>
    /// <returns></returns>
    ID3D11ShaderResourceView* get_SRV() { return m_pSRV.Get(); }  
};


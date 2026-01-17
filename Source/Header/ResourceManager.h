#pragma once
//--------------------------------------------------------------------------------------
//      * Includes *
//--------------------------------------------------------------------------------------
#include <unordered_map>
#include <d3d11.h>
#include <wrl/client.h>
#include "Texture.h"

// =======================================================================================
//      * ResourceManager Class *
//        - シングルトン - 
//          リソース管理
// =======================================================================================
class ResourceManager
{
private:
    // コピー禁止
    ResourceManager(const ResourceManager &) = delete;
    ResourceManager &operator=(const ResourceManager &) = delete;
	// ------------------------------------------------------
     
    // テクスチャリソース Textureの共有ポインタを持つ
    std::unordered_map<std::wstring, std::shared_ptr<class Texture>> m_TexturesMap;

    // レンダーターゲット用のテクスチャ
    std::unordered_map<std::string, std::shared_ptr<class Texture>> m_RTTextureMap;
    
    // モデルリソース
    std::unordered_map<std::string, std::shared_ptr<class ModelData>> m_ModelsMap;

    // マテリアルリソース
    std::unordered_map<std::string, std::shared_ptr<class Material>> m_pMaterialMap;

    std::weak_ptr<class RendererEngine> m_pRenderer;    // 描画クラスの弱参照を持つ

public:
    ResourceManager() = default;
    ~ResourceManager();

    bool Init(std::shared_ptr<class RendererEngine> renderer);   // 初期化
    void Term();                            // 終了

    /// <summary>
    /// WIC対応テクスチャのロード
    /// </summary>
    /// <param name="path"></param>
    /// <returns></returns>
    std::shared_ptr<Texture> LoadWIC_Texture(const std::wstring &path);           

    /// <summary>
    /// キューブマップ用DDSテクスチャのロード
    /// </summary>
    /// <param name="path"></param>
    /// <returns></returns>
    std::shared_ptr<Texture> LoadDDS_CubeMap_Texture(const std::wstring &path);    

    /// <summary>
    /// FBXモデルのロード
    /// ※ 確認できていないけど、Assimpを使っているのでFBX以外もいけるかも？
    /// </summary>
    /// <param name="path"></param>
    /// <returns></returns>
    std::shared_ptr<class ModelData> LoadModel(const char* path);                      

    // SRVからtextureに変換 （Textureはあくまでこのクラスが持つ）
    std::shared_ptr<Texture> Convert_SRVToTexture(const std::string& tag, const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> &pSrv = NULL, UINT w = 0, UINT h = 0);

    /// <summary>
    /// マテリアルの取得
    /// 事前に登録したタグを指定しマテリアルを返す
    /// もし、存在しなければデフォルトのものを返す
    /// </summary>
    /// <param name="tag"></param>
    /// <returns></returns>
    std::shared_ptr<Material> LoadMaterial(const std::string &tag);

    void RegisterMaterialData(const std::string &tag, const Material &mat);
};


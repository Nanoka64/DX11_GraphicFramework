//--------------------------------------------------------------------------------------
//      * Includes *
//--------------------------------------------------------------------------------------
#include "pch.h"
#include "ResourceManager.h"    // リソース
#include "RendererEngine.h"
#include "Texture.h"
#include "ModelData.h"
#include <DirectXTex.h>
#include <locale>
#include <codecvt>

using namespace DirectX;

ResourceManager::~ResourceManager()
{
    Release();
}

//--------------------------------------------------------------------------------------
//      * ResourceManager - 初期化 - *
// 引数：デバイスの共有ポインタ
//--------------------------------------------------------------------------------------
bool ResourceManager::Init(std::shared_ptr<RendererEngine> renderer)
{
    m_pRenderer = renderer;

    return true;
}


//--------------------------------------------------------------------------------------
//      * ResourceManager - 終了 - *
//--------------------------------------------------------------------------------------
void ResourceManager::Term()
{
    Release();
}


//--------------------------------------------------------------------------------------
//      * ResourceManager  - 解放 - *
//--------------------------------------------------------------------------------------
void ResourceManager::Release()
{
    m_TexturesMap.clear();
}


//*---------------------------------------------------------------------------------------
//* @:ResourceManager Class 
//*【?】テクスチャのロード
//* 引数：1.ファイルパス
//* 返値：ロードしたTexture
//*----------------------------------------------------------------------------------------
std::shared_ptr<Texture> ResourceManager::LoadWIC_Texture(const std::wstring &path)
{
    auto it = m_TexturesMap.find(path);

    // 既に登録済みならそれを返す
    if (it != m_TexturesMap.end())
    {
        return it->second;
    }

    // shared_ptrを受け取る
    auto render = m_pRenderer.lock();

    // 参照が切れていないなら
    if (render)
    {
        // 登録されていないなら、作成して登録
        auto texture = std::make_shared<Texture>();

        // 読み込み
        if (FAILED(texture->Load_WIC(path, *render)))
        {
            std::string errMsg = "のロードに失敗";
            errMsg = Tool::WStringToString(path.c_str()) + errMsg;
            int result = MessageBox(NULL, errMsg.c_str() , "Texture LoadError", MB_ABORTRETRYIGNORE | MB_ICONWARNING);

            return nullptr;
        }
        // 配列に登録
        m_TexturesMap[path] = std::move(texture);

        return m_TexturesMap[path];
    }


    return nullptr;
}


//*---------------------------------------------------------------------------------------
//* @:ResourceManager Class 
//*【?】キューブマップ用 DDSフォーマットテクスチャのロード
//* 引数：1.ファイルパス
//* 返値：ロードしたTexture
//*----------------------------------------------------------------------------------------
std::shared_ptr<Texture> ResourceManager::LoadDDS_CubeMap_Texture(const std::wstring& path)
{
    auto it = m_TexturesMap.find(path);

    // 既に登録済みならそれを返す
    if (it != m_TexturesMap.end())
    {
        return it->second;
    }

    // shared_ptrを受け取る
    auto render = m_pRenderer.lock();

    // 参照が切れていないなら
    if (render)
    {
        // 登録されていないなら、作成して登録
        auto texture = std::make_shared<Texture>();

        // 読み込み
        if (FAILED(texture->Load_DDS_CubeMap(path, *render)))
        {
            std::string errMsg = "のロードに失敗";
            errMsg = Tool::WStringToString(path.c_str()) + errMsg;
            int result = MessageBox(NULL, errMsg.c_str(), "DDS Texture LoadError", MB_ABORTRETRYIGNORE | MB_ICONWARNING);
            return nullptr;
        }
        // 配列に登録
        m_TexturesMap[path] = std::move(texture);

        return m_TexturesMap[path];
    }
    return nullptr;
};


//*---------------------------------------------------------------------------------------
//* @:ResourceManager Class 
//*【?】モデルのロード
//* 引数：1.ファイルパス
//* 返値：ロードしたモデル情報
//*----------------------------------------------------------------------------------------
std::shared_ptr<ModelData> ResourceManager::LoadModel(const char *path)
{
    auto it = m_ModelsMap.find(path);

    // 既に登録済みならそれを返す
    if (it != m_ModelsMap.end())
    {
        return it->second;
    }

    // shared_ptrを受け取る
    auto render = m_pRenderer.lock();

    // 参照が切れていないなら
    if (render)
    {
        // 登録されていないなら、作成して登録
        auto model = std::make_shared<ModelData>();

        // 読み込み
        if (!model->Setup(*render, path))
        {
            std::string errMsg = "のロードに失敗";
            errMsg = path + errMsg;
            int result = MessageBox(NULL, errMsg.c_str(), "Model Load Error", MB_ABORTRETRYIGNORE | MB_ICONWARNING);
            return nullptr;
        }

        // 配列に登録
        m_ModelsMap[path] = std::move(model);

        return m_ModelsMap[path];
    }
    return nullptr;
}



//*---------------------------------------------------------------------------------------
//* @:ResourceManager Class 
//*【?】SRVからtextureに変換 （）
//* 引数：1.タグ（TextureはあくまでResourceManagerが持つので取り出したいときなどに）
//* 引数：2.変換したいRSVのポインタ
//* 引数：3.横幅
//* 引数：4.縦幅
//* 返値：変換したTexture
//*----------------------------------------------------------------------------------------
std::shared_ptr<Texture> ResourceManager::Convert_SRVToTexture(const std::string &tag, const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> &pSrv, UINT w, UINT h)
{
    auto it = m_RTTextureMap.find(tag);

    // 既に登録済みならそれを返す
    if (it != m_RTTextureMap.end())
    {
        return it->second;
    }

    // shared_ptrを受け取る
    auto render = m_pRenderer.lock();

    // 参照が切れていないなら
    if (render)
    {
        // 登録されていないなら、作成して登録
        auto texture = std::make_shared<Texture>();

        // 読み込み
        texture->set_SRV_ComPtr(pSrv);
        texture->set_Width(w);
        texture->set_Height(h);

        // 配列に登録
        m_RTTextureMap[tag] = std::move(texture);

        return m_RTTextureMap[tag];
    }
    return nullptr;
}

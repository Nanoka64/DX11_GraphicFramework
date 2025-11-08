//--------------------------------------------------------------------------------------
//      * Includes *
//--------------------------------------------------------------------------------------
#include "pch.h"
#include "ResourceManager.h"    // リソース
#include "RendererManager.h"
#include "Texture.h"
#include <DirectXTex.h>
#include <locale>
#include <codecvt>

using namespace DirectX;

//--------------------------------------------------------------------------------------
//      * ResourceManager - 初期化 - *
// 引数：デバイスの共有ポインタ
//--------------------------------------------------------------------------------------
bool ResourceManager::Init(std::shared_ptr<RendererManager> renderer)
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
std::shared_ptr<Texture> ResourceManager::LoadTexture(const std::wstring &path)
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
        if(FAILED(texture->Load(path, *render))) return nullptr;

        // 配列に登録
        m_TexturesMap[path] = std::move(texture);

        return m_TexturesMap[path];
    }
    return nullptr;
}


//*---------------------------------------------------------------------------------------
//* @:ResourceManager Class 
//*【?】モデルのロード
//* 引数：1.ファイルパス
//* 返値：ロードしたTexture
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
        if (!model->Setup(*render, path)) return nullptr;

        // 配列に登録
        m_ModelsMap[path] = std::move(model);

        return m_ModelsMap[path];
    }
    return nullptr;
}


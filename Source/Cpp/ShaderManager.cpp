#include "pch.h"
#include "RendererManager.h"
#include "ShaderManager.h"
#include "PixelShader.h"
#include "VertexShader.h"
#include "InputLayout.h"
#include "Path.h"
#include <algorithm>

#define SHADER_NUM 2    // シェーダの数

using namespace Path;
using namespace Microsoft::WRL;

// シンプルシェーダ（スプライト兼用）
static const D3D11_INPUT_ELEMENT_DESC SimpleLayout[] =
{
    {"POSITION",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, offsetof(BASE_VERTEX::VERTEX, pos),   D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"NORMAL",     0, DXGI_FORMAT_R32G32B32_FLOAT,    0, offsetof(BASE_VERTEX::VERTEX, normal),D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"COLOR",      0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(BASE_VERTEX::VERTEX, color), D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"TEXCOORD",   0, DXGI_FORMAT_R32G32_FLOAT,       0, offsetof(BASE_VERTEX::VERTEX, uv),    D3D11_INPUT_PER_VERTEX_DATA, 0},
};

// モデルアニメーション用シェーダ
static const D3D11_INPUT_ELEMENT_DESC ModelLayout[] =
{
    {"POSITION",   0, DXGI_FORMAT_R32G32B32_FLOAT,     0, offsetof(BASE_VERTEX::MODEL_VERTEX, pos),   D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"NORMAL",     0, DXGI_FORMAT_R32G32B32_FLOAT,     0, offsetof(BASE_VERTEX::MODEL_VERTEX, normal),D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"COLOR",      0, DXGI_FORMAT_R32G32B32A32_FLOAT,  0, offsetof(BASE_VERTEX::MODEL_VERTEX, color), D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"TEXCOORD",   0, DXGI_FORMAT_R32G32_FLOAT,        0, offsetof(BASE_VERTEX::MODEL_VERTEX, uv),    D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"BONEIDS",    0, DXGI_FORMAT_R32G32B32A32_SINT,   0, offsetof(BASE_VERTEX::MODEL_VERTEX, boneIDs),    D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(BASE_VERTEX::MODEL_VERTEX, boneWeights),    D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"TANGENT",     0, DXGI_FORMAT_R32G32B32_FLOAT,     0, offsetof(BASE_VERTEX::MODEL_VERTEX, tangent),    D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"BINORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT,     0, offsetof(BASE_VERTEX::MODEL_VERTEX, biNormal),   D3D11_INPUT_PER_VERTEX_DATA, 0},
};



/* ---------------------------------------------------------------------------------------
/* - @:ShaderManager Class - デストラクタ - * - */
/* 【?】リソースの破棄
/* ---------------------------------------------------------------------------------------*/
ShaderManager::~ShaderManager()
{

}


bool ShaderManager::Init(std::shared_ptr<RendererManager> renderer)
{
    m_pRenderer = renderer;

    // 入力レイアウト定義
    InputLayoutSetupData layout[] =
    {
        {
            /* ベース用 */
            SHADER_TYPE::SIMPLE,
            ARRAYSIZE(SimpleLayout),
            SimpleLayout,
        },            
        {
            /* モデルアニメーション用 */
            SHADER_TYPE::MODEL,
            ARRAYSIZE(ModelLayout),
            ModelLayout,
        },      
        {
            /* スプライト用 */
            SHADER_TYPE::SPRITE,
            ARRAYSIZE(SimpleLayout),
            SimpleLayout,
        }, 
        {
            /* ディファードシェーディング用 */
            SHADER_TYPE::DEFFERD,
            ARRAYSIZE(SimpleLayout),
            SimpleLayout,
        },
    };

    // 格納
    for (size_t i = 0; i < ARRAYSIZE(layout); i++)
    {
        m_InputLayoutSetupDataList.push_back(layout[i]);
    }

    return true;
}


/* ---------------------------------------------------------------------------------------
/* - @:ShaderManager Class - シェーダの作成 - * - */
/* 【?】指定タイプのシェーダを作成
/* 引数：1.シェーダタイプ
/* ---------------------------------------------------------------------------------------*/
bool ShaderManager::CreateShader(SHADER_TYPE type)
{
    ShaderInfo shaderInfo{};

    // 頂点・ピクセルシェーダ、入力レイアウトのセットアップ
    if (!VertexShaderFactory(type, &shaderInfo))return false;
    if (!PixelShaderFactory(type, &shaderInfo))return false;
    
    shaderInfo.Type = type;

    m_ShaderList.push_back(shaderInfo); // 無事作成出来たらリストに追加

    return true;
}

/* ---------------------------------------------------------------------------------------
/* - @:ShaderManager Class - シェーダをデバイスにセット - * - */
/* 【?】指定タイプのシェーダをデバイスにセット
/* 引数：1.シェーダ種類
/* ---------------------------------------------------------------------------------------*/
void ShaderManager::DeviceToSetShader(SHADER_TYPE type)
{
    auto begin = m_ShaderList.begin();
    auto end = m_ShaderList.end();

    // 指定シェーダを探す
    auto shader = std::find_if(begin, end,
        [type](const ShaderInfo& val)
        {
            return (val.Type == type);
        }
    );

    if (shader == end){
        assert(false);
        return;
    }

    auto pContext = m_pRenderer.lock().get()->get_DeviceContext();

    pContext->IASetInputLayout((*shader).pInputLayout->get());
    pContext->VSSetShader((*shader).pVShader->get(), NULL, 0);
    pContext->PSSetShader((*shader).pPShader->get(), NULL, 0);
}



/* ---------------------------------------------------------------------------------------
/* - @:ShaderManager Class - 入力レイアウトの作成 - * - */
/* 【?】入力レイアウト作成
/* ---------------------------------------------------------------------------------------*/
bool ShaderManager::InputLayoutFactory(SHADER_TYPE type, ShaderInfo* out, ID3DBlob* blob)
{
    auto renderer = m_pRenderer.lock();
    if (renderer == nullptr)return false;

    auto pDevice = renderer.get()->get_Device();
    auto pContext = renderer.get()->get_DeviceContext();
    HRESULT hr = S_OK;


    // 指定種別と合致する要素を取得
    auto begin = m_InputLayoutSetupDataList.begin();
    auto end = m_InputLayoutSetupDataList.end();
    auto target = std::find_if(begin, end,
        [type](const InputLayoutSetupData& val)
        {
            return (val.type == type);
        }
    );

    if (target == end) {
        MessageBox(NULL, "入力レイアウトの作成に失敗しました", "Error", MB_OK);
        return false;
    }


    UINT numElements = target->LayoutArraySize;   // 配列の要素数取得
    const D3D11_INPUT_ELEMENT_DESC* pLayout = target->pLayout;

    ComPtr<ID3D11InputLayout> pOutputLayout = nullptr; // 結果格納用

    // 入力レイアウト作成
    hr = pDevice->CreateInputLayout(
        pLayout,                     // 入力データ型の配列設定    
        numElements,                 // 配列の要素数
        blob->GetBufferPointer(),    // コンパイルされたシェーダへのポインタ
        blob->GetBufferSize(),       // コンパイルされたシェーダのサイズ
        pOutputLayout.GetAddressOf() // 生成した入力レイアウトの出力先
    );

    if (FAILED(hr)){
        MessageBox(NULL, "入力レイアウトの作成に失敗しました", "Error", MB_OK);
        return false;
    }

    out->pInputLayout = std::make_shared<InputLayout>();   // 共有ポインタ作成
    out->pInputLayout->set(std::move(pOutputLayout));  // レイアウトのセット

    return true;
}


/* ---------------------------------------------------------------------------------------
/* - @:ShaderManager Class - 頂点シェーダの作成 - * - */
/* 【?】頂点シェーダの作成
/* ---------------------------------------------------------------------------------------*/
bool ShaderManager::VertexShaderFactory(SHADER_TYPE type, ShaderInfo* out)
{
    auto renderer = m_pRenderer.lock();
    if (renderer == nullptr)return false;
    auto pDevice = renderer.get()->get_Device();
    auto pContext = renderer.get()->get_DeviceContext();

    ID3DBlob* pVSBlob = NULL;
    HRESULT hr = S_OK;
    ComPtr<ID3D11VertexShader> pVertex = nullptr;  // 結果格納用

    // 頂点シェーダのコンパイル
    switch (type)
    {
    case SHADER_TYPE::NONE:
        MessageBox(NULL, "不明な頂点シェーダ", "Error", MB_OK);
        break;
    case SHADER_TYPE::SIMPLE:
        hr = this->CompileShader(HLSL__SimpleVS_PATH.c_str(), "SimpleVSMain", "vs_5_0", &pVSBlob);
        break;
    case SHADER_TYPE::MODEL:
        hr = this->CompileShader(HLSL__VS_PATH.c_str(), "VS", "vs_5_0", &pVSBlob);
        break;
    case SHADER_TYPE::SPRITE:
        hr = this->CompileShader(HLSL__SpriteVS_PATH.c_str(), "VSMain", "vs_5_0", &pVSBlob);
        break;
    case SHADER_TYPE::DEFFERD:
        hr = this->CompileShader(HLSL__SpriteVS_PATH.c_str(), "VSMain", "vs_5_0", &pVSBlob);
        break;
    default:
        MessageBox(NULL, "不明な頂点シェーダ", "Error", MB_OK);
        break;
    }

    // 失敗
    if (FAILED(hr)){
        MessageBox(NULL, "頂点シェーダーがコンパイルできませんでした", "Error", MB_OK);
        return false;
    }


    // 頂点シェーダーの作成
    hr = pDevice->CreateVertexShader(
        pVSBlob->GetBufferPointer(),
        pVSBlob->GetBufferSize(),
        NULL,
        pVertex.GetAddressOf()
    );

    if (FAILED(hr)) {
        pVSBlob->Release();
        MessageBoxA(NULL, "頂点シェーダーの作成に失敗しました。", "Error", MB_OK);
        return false;
    }

    // 頂点レイアウトの作成
    bool res = InputLayoutFactory(type, out, pVSBlob);
    pVSBlob->Release();

    if (!res) {
        assert(false);
        return false;
    }

    out->pVShader = std::make_shared<VertexShader>();   // 共有ポインタ作成
    out->pVShader->set(std::move(pVertex));  // 頂点シェーダセット

    return true;    // 成功
}


/* ---------------------------------------------------------------------------------------
/* - @:ShaderManager Class - ピクセルシェーダの作成 - * - */
/* 【?】ピクセルシェーダの作成
/* ---------------------------------------------------------------------------------------*/
bool ShaderManager::PixelShaderFactory(SHADER_TYPE type, ShaderInfo* out)
{
    auto renderer = m_pRenderer.lock();
    if (renderer == nullptr)return false;
    auto pDevice = renderer.get()->get_Device();
    auto pContext = renderer.get()->get_DeviceContext();
    HRESULT hr = S_OK;
    ID3DBlob* pPSBlob = NULL;
    ComPtr<ID3D11PixelShader> pPixel = nullptr;    // 結果格納用

    // ピクセルシェーダーのコンパイル
    switch (type)
    {
    case SHADER_TYPE::NONE:
        break;
    case SHADER_TYPE::SIMPLE:
        hr = CompileShader(HLSL__SimplePS_PATH.c_str(), "SimplePSMain", "ps_5_0", &pPSBlob);
        break;
    case SHADER_TYPE::MODEL:
        hr = CompileShader(HLSL__PS_PATH.c_str(), "PS", "ps_5_0", &pPSBlob);
        break;    
    case SHADER_TYPE::SPRITE:
        hr = this->CompileShader(HLSL__SpritePS_PATH.c_str(), "PSMain", "ps_5_0", &pPSBlob);
        break;
    case SHADER_TYPE::DEFFERD:
        hr = this->CompileShader(HLSL__DefferdPS_PATH.c_str(), "PSMain", "ps_5_0", &pPSBlob);
        break;
    default:
        break;
    }
    
    // 失敗
    if (FAILED(hr)){
        MessageBox(NULL, "ピクセルシェーダーがコンパイルできませんでした", "Error", MB_OK);
        return false;
    }

    // ピクセルシェーダーの作成
    hr = pDevice->CreatePixelShader(
        pPSBlob->GetBufferPointer(),
        pPSBlob->GetBufferSize(),
        NULL,
        pPixel.GetAddressOf()
    );

    pPSBlob->Release();
    if (FAILED(hr))return false;    // 失敗

    out->pPShader = std::make_shared<PixelShader>();   // 共有ポインタ作成
    out->pPShader.get()->set(std::move(pPixel));   // ピクセルシェーダのセット
    
    return true;    // 成功
}


/* ---------------------------------------------------------------------------------------
/* - @:ShaderManager Class - シェーダのコンパイル - * - */
/* 【?】シェーダをコンパイルする
/* 引数：1.
/* ---------------------------------------------------------------------------------------*/
HRESULT ShaderManager::CompileShader(LPCWSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob **ppBlobOut)
{
    HRESULT hr = S_OK;
    //DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
    DWORD dwShaderFlags =D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR;  // こっちの方が計算が速いらしい https://proglog.site/triangle-transform-on-shader/

#if defined(DEBUG) || defined(_DEBUG)
    // D3DCOMPILE_DEBUG フラグを設定して、シェーダーにデバッグ情報を埋め込む。
    // このフラグを設定すると、シェーダーのデバッグ体験が向上するが、それでも
    // シェーダーは最適化され、このプログラムのリリース構成で実行されるのと同じように
    // 実行される。
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob *pErrorBlob;
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
            OutputDebugStringA((char *)pErrorBlob->GetBufferPointer());
            if (pErrorBlob)pErrorBlob->Release();
            return hr;
        }

        return hr;
    }
    if (pErrorBlob)pErrorBlob->Release();

    return S_OK;
}
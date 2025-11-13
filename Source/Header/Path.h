#pragma once

//--------------------------------------------------------------------------------------
/*      相対パス    */
//--------------------------------------------------------------------------------------
namespace Path
{
    constexpr const wchar_t* Shader = L"Source//Shader//";      // シェーダー
    constexpr const wchar_t* Header = L"Source//Header//";      // ヘッダー
    constexpr const wchar_t* Image  = L"Resource//Img//";       // 画像
    constexpr const wchar_t* BGM    = L"Resource//BGM//";       // BGM
    constexpr const wchar_t* SE     = L"Resource//SE//";        // SE
    constexpr const wchar_t* Font   = L"Resource//Font//";      // フォント
    constexpr const wchar_t* Model  = L"Resource//Model//Player//";     // モデル

    // パス取得 
    //     ファイル種別↓    ↓ファイル名  
    const std::wstring HLSL__VS_PATH = std::wstring(Shader) + L"VS.hlsl"; 
    const std::wstring HLSL__PS_PATH = std::wstring(Shader) + L"PS.hlsl"; 
    
    // シンプル
    const std::wstring HLSL__SimplePS_PATH = std::wstring(Shader) + L"SimplePS.hlsl";
    const std::wstring HLSL__SimpleVS_PATH = std::wstring(Shader) + L"SimpleVS.hlsl";
    
    // スプライト
    const std::wstring HLSL__SpriteVS_PATH = std::wstring(Shader) + L"SpriteVS.hlsl";
    const std::wstring HLSL__SpritePS_PATH = std::wstring(Shader) + L"SpritePS.hlsl";
    
    const std::wstring HLSL__DefferdPS_PATH = std::wstring(Shader) + L"DefefferdTest.hlsl";
}
#pragma once

//--------------------------------------------------------------------------------------
/*      相対パス    */
//--------------------------------------------------------------------------------------
namespace Path
{
    constexpr const wchar_t* Shader = L"Source//Shader//";              // シェーダー
    constexpr const wchar_t* Header = L"Source//Header//";              // ヘッダー
    constexpr const wchar_t* Image  = L"Resource//Img//";               // 画像
    constexpr const wchar_t* BGM    = L"Resource//BGM//";               // BGM
    constexpr const wchar_t* SE     = L"Resource//SE//";                // SE
    constexpr const wchar_t* Font   = L"Resource//Font//";              // フォント
    constexpr const wchar_t* Model  = L"Resource//Model//Player//";     // モデル
    constexpr const wchar_t* CSO    = L"Resource//CSO//";               // CSOファイル

    // パス取得 
    //     ファイル種別↓    ↓ファイル名  
    //const std::wstring HLSL__VS_PATH = std::wstring(Shader) + L"VS.hlsl"; 

    // ランタイムコンパイル
    const std::wstring HLSL__VS_PATH = std::wstring(Shader) + L"VS.hlsl"; 
    const std::wstring HLSL__PS_PATH = std::wstring(Shader) + L"PS.hlsl"; 
    const std::wstring HLSL__SimplePS_PATH = std::wstring(Shader) + L"SimplePS.hlsl";
    const std::wstring HLSL__SimpleVS_PATH = std::wstring(Shader) + L"SimpleVS.hlsl";
    const std::wstring HLSL__SpriteVS_PATH = std::wstring(Shader) + L"SpriteVS.hlsl";
    const std::wstring HLSL__SpritePS_PATH = std::wstring(Shader) + L"SpritePS.hlsl";
    const std::wstring HLSL__DefferdPS_PATH = std::wstring(Shader) + L"DefefferdTest.hlsl";

    // csoファイル(コンパイル済みシェーダ)
    const std::wstring HLSL_CSO__VS_PATH = std::wstring(CSO) + L"VS.cso";
    const std::wstring HLSL_CSO__PS_PATH = std::wstring(CSO) + L"PS.cso";
    const std::wstring HLSL_CSO__SimplePS_PATH = std::wstring(CSO) + L"SimplePS.cso";
    const std::wstring HLSL_CSO__SimpleVS_PATH = std::wstring(CSO) + L"SimpleVS.cso";
    const std::wstring HLSL_CSO__SpriteVS_PATH = std::wstring(CSO) + L"SpriteVS.cso";
    const std::wstring HLSL_CSO__SpritePS_PATH = std::wstring(CSO) + L"SpritePS.cso";
    const std::wstring HLSL_CSO__DefferdPS_PATH = std::wstring(CSO) + L"DefefferdTest.cso";
}
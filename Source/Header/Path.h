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
    //        ファイル種別↓    ↓ファイル名  
    //const std::wstring HLSL__VS_PATH = std::wstring(Shader) + L"VS.hlsl"; 

    // ランタイムコンパイル *****************************************************************************************
    // PS
    const std::wstring HLSL__GBuffer_Standard_PS_PATH = std::wstring(Shader)        + L"GBuffer_Standard_PS.hlsl";
    const std::wstring HLSL__GBuffer_Simple_PS_PATH = std::wstring(Shader)          + L"GBuffer_Simple_PS.hlsl";
    const std::wstring HLSL__LightingPath_Standard_PS_PATH = std::wstring(Shader)   + L"LightingPath_Standard_PS.hlsl";
    const std::wstring HLSL__Sprite_PS_PATH = std::wstring(Shader)                  + L"Sprite_PS.hlsl";
    const std::wstring HLSL__Simple_PS_PATH = std::wstring(Shader)                  + L"Simple_PS.hlsl";
    const std::wstring HLSL__GaussianBlur_PS_PATH = std::wstring(Shader)            + L"GaussianBlur_PS.hlsl";
    const std::wstring HLSL__Skybox_PS_PATH = std::wstring(Shader)                  + L"Skybox_PS.hlsl";

    // VS
    const std::wstring HLSL__Skinned_VS_PATH = std::wstring(Shader)                 + L"Skinned_VS.hlsl";
    const std::wstring HLSL__Static_VS_PATH = std::wstring(Shader)                  + L"Static_VS.hlsl";
    const std::wstring HLSL__Static_Tan_VS_PATH = std::wstring(Shader)              + L"Static_Tan_VS.hlsl";
    const std::wstring HLSL__Sprite_VS_PATH = std::wstring(Shader)                  + L"Sprite_VS.hlsl";
    const std::wstring HLSL__XBlur_VS_PATH = std::wstring(Shader)                   + L"XBlur_VS.hlsl";
    const std::wstring HLSL__YBlur_VS_PATH = std::wstring(Shader)                   + L"YBlur_VS.hlsl";
    const std::wstring HLSL__Skybox_VS_PATH = std::wstring(Shader)                  + L"Skybox_VS.hlsl";

    // csoファイル(コンパイル済みシェーダ) ***************************************************************************
    // PS
    const std::wstring HLSL_CSO__GBuffer_Standard_PS_PATH = std::wstring(CSO)       + L"GBuffer_Standard_PS.cso";
    const std::wstring HLSL_CSO__GBuffer_Simple_PS_PATH = std::wstring(CSO)         + L"GBuffer_Simple_PS.cso";
    const std::wstring HLSL_CSO__LightingPath_Standard_PS_PATH = std::wstring(CSO)  + L"LightingPath_Standard_PS.cso";
    const std::wstring HLSL_CSO__Sprite_PS_PATH = std::wstring(CSO)                 + L"Sprite_PS.cso";
    const std::wstring HLSL_CSO__Simple_PS_PATH = std::wstring(CSO)                 + L"Simple_PS.cso";
    const std::wstring HLSL_CSO__GaussianBlur_PS_PATH = std::wstring(CSO)           + L"GaussianBlur_PS.cso";
    const std::wstring HLSL_CSO__Skybox_PS_PATH = std::wstring(CSO)                 + L"Skybox_PS.cso";


    // VS
    const std::wstring HLSL_CSO__Skinned_VS_PATH = std::wstring(CSO)                + L"Skinned_VS.cso";
    const std::wstring HLSL_CSO__Static_VS_PATH = std::wstring(CSO)                 + L"Static_VS.cso";
    const std::wstring HLSL_CSO__Static_Tan_VS_PATH = std::wstring(CSO)             + L"Static_Tan_VS.cso";
    const std::wstring HLSL_CSO__Sprite_VS_PATH = std::wstring(CSO)                 + L"Sprite_VS.cso";
    const std::wstring HLSL_CSO__XBlur_VS_PATH = std::wstring(CSO)                  + L"XBlur_VS.cso";
    const std::wstring HLSL_CSO__YBlur_VS_PATH = std::wstring(CSO)                  + L"YBlur_VS.cso";
    const std::wstring HLSL_CSO__Skybox_VS_PATH = std::wstring(CSO)                 + L"Skybox_VS.cso";
}
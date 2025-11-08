#pragma once

//--------------------------------------------------------------------------------------
//      * 列挙 定義 *
//--------------------------------------------------------------------------------------

/*
* フォントリスト
*/
enum class FONT_TYPE
{
    Meiryo,
    Arial,
    MeiryoUI,
};

namespace
{
    const WCHAR *FontNameList[] =
    {
        L"メイリオ",
        L"Arial",
        L"Meiryo UI"
    };
}

//--------------------------------------------------------------------------------------
//      * 構造体 *
//--------------------------------------------------------------------------------------

/*
* フォント情報 
*/
struct FONT_DATA
{
    FONT_TYPE type;                         // フォント種類
    IDWriteFontCollection *fontCollection;  // フォントコレクション
    DWRITE_FONT_WEIGHT fontWeight;          // フォントの太さ
    DWRITE_FONT_STYLE fontStyle;            // フォントスタイル
    DWRITE_FONT_STRETCH fontStretch;        // フォントの幅
    FLOAT fontSize;                         // フォントのサイズ
    WCHAR const *localName;                 // ローカル名
    DWRITE_TEXT_ALIGNMENT textAlignment;    // テキストの配置
    D2D1_COLOR_F color;                     // フォントの色

    // コンストラクタ
    FONT_DATA()
    {
        type            = FONT_TYPE::Meiryo;
        fontCollection  = nullptr;
        fontWeight      = DWRITE_FONT_WEIGHT_NORMAL;
        fontStyle       = DWRITE_FONT_STYLE_NORMAL;
        fontStretch     = DWRITE_FONT_STRETCH_NORMAL;
        fontSize        = 20.0f;
        localName       = L"ja-jp";
        textAlignment   = DWRITE_TEXT_ALIGNMENT_LEADING;
        color           = D2D1::ColorF(D2D1::ColorF::White);
    }
};







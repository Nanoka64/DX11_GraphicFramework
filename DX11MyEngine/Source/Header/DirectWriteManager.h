#pragma once
//--------------------------------------------------------------------------------------
//      * Includes *
//--------------------------------------------------------------------------------------
#include <string>
#include <format>


/// <summary>
/// 水平方向のアライメント指定用の列挙型
/// </summary>
enum class H_ALIGNMENT
{
	LEADING = 0,   // 左寄せ
	CENTER = 1,    // 中央寄せ
	TRAILING = 2,  // 右寄せ
};

/// <summary>
/// 垂直方向のアライメント指定用の列挙型
/// </summary>
enum class V_ALIGNMENT {
    TOP,
    CENTER,
    BOTTOM
};

// =======================================================================================
//
// *---     DirectWriteManager Class         ---*
//         * シングルトンパターン *
// 
//			DirectWriteを扱うクラス
//          ※ RenderDocを使う際はこれを使ってると怒られる
// 
// 参考サイト：https://islingtonsystem.hatenablog.jp/entry/2022/06/18/222040
//             http://mitsunagistudio.net/old_posts/tips/d2d-d3d11-sharing/
// =======================================================================================

class DirectWriteManager
{
private:
    ID2D1Factory         *m_pD2DFactory;        // Direct2D    描画オブジェクトの生成用
    IDWriteFactory       *m_pWriteFactory;      // DirectWrite 描画オブジェクトの生成用
    IDWriteTextFormat    *m_pTextFormat;        // テキストの配置、書式
    //IDWriteTextLayout    *m_pTextLayout;        // テキストの形式    ※ローカル変数にした
    ID2D1RenderTarget    *m_pRenderTarget;      // 描画先
    ID2D1SolidColorBrush *m_pSolidBrush;        // 塗りつぶしに使う
    ID2D1SolidColorBrush *m_pOutLineSolidBrush; // 塗りつぶしに使う 
    IDXGISurface         *m_pSurfaceBackBuffer; // 描画APIとのやり取りに使う 描画先として使える

	float m_OutLineSize; // アウトラインのサイズ

    //struct FONT_DATA *m_pFontData; // フォントデータ 

    // テキストデータ
    std::map<std::string, Microsoft::WRL::ComPtr<IDWriteTextFormat>> m_pTextFormatMap;    

	/// <summary>
	/// アウトラインの描画
	/// </summary>
	/// <param name="_pos">位置</param>
	/// <param name="_pTextLayout">テキストレイアウトポインタの参照</param>
	void DrawOutLine(const D2D1_POINT_2F& _pos, const Microsoft::WRL::ComPtr<IDWriteTextLayout> &_pTextLayout);

public:
    DirectWriteManager();
    ~DirectWriteManager();

    // コピーコンストラクタと代入演算子を削除
    DirectWriteManager(const DirectWriteManager &) = delete;;
    DirectWriteManager &operator = (const DirectWriteManager &) = delete;
    
    // stringをwstringへ変換する
    std::wstring StringToWString(std::string oString);

    HRESULT Init(RendererEngine &render);  // 初期化
    void Term();    // 終了

    void BeginDraw();   // 描画の開始（フレームの最初に）
    HRESULT EndDraw();     // 描画終了（フレームの最後）

    // フォントデータを設定
    HRESULT SetFontData(struct FONT_DATA *data);

    /// <summary>
    // 文字の色の設定
	// ※これを呼ぶと次に描画する文字の色が変わる
	//   DrawString系を呼んだ後、デフォルト（白）に戻さないと、その後の文字もその色になってしまうので注意！！
    /// </summary>
    /// <param name="col">色</param>
    void SetColor(const D2D1_COLOR_F& col);

	/// <summary>
	/// アウトラインの設定
	/// </summary>
	/// <param name="_size">アウトラインのサイズ</param>
	/// <param name="_col">アウトラインの色</param>
	void SetOutLine(float _size, const D2D1_COLOR_F& _col = D2D1::ColorF(0.0f, 0.0f, 0.0f));

    /// <summary>
    /// 文字列の表示
    /// </summary>
    /// <param name="str">:文字列 </param>
    /// <param name="pos">:座標</param>
    /// <param name="formatTag">:文字の情報を格納した配列へのキー </param>
    /// <param name="options">:整形オプション</param>
    /// <param name="_isUnderLine">:下線を引くか</param>
    void DrawString(std::string str, const VECTOR2::VEC2 &_pos, const std::string& formatTag, D2D1_DRAW_TEXT_OPTIONS options = D2D1_DRAW_TEXT_OPTIONS_NONE, bool _isUnderLine = true, DWRITE_TEXT_ALIGNMENT _alignment = DWRITE_TEXT_ALIGNMENT_LEADING);
    void DrawString(std::wstring str, const VECTOR2::VEC2& _pos, const std::string& formatTag, D2D1_DRAW_TEXT_OPTIONS options = D2D1_DRAW_TEXT_OPTIONS_NONE, bool _isUnderLine = true, DWRITE_TEXT_ALIGNMENT _alignment = DWRITE_TEXT_ALIGNMENT_LEADING);


    /// <summary>
	/// アライメント指定の文字列の表示
    /// </summary>
    /// <param name="_str">文字</param>
    /// <param name="_pos">位置（親の位置の左上）</param>
    /// <param name="_formatTag">フォーマットタグ</param>
    /// <param name="_hAlign">水平揃え</param>
    /// <param name="_vAlign">垂直揃え</param>
    /// <param name="_parentSize">親の大きさ</param>
    void DrawStringToAligment(const std::string& _str, const VECTOR2::VEC2& _pos, const std::string& _formatTag, H_ALIGNMENT _hAlign, V_ALIGNMENT _vAligment, const VECTOR2::VEC2& _parentSize);
    void DrawStringToAligment(const std::wstring& _wstr, const VECTOR2::VEC2& _pos, const std::string& _formatTag, H_ALIGNMENT _hAlign, V_ALIGNMENT _vAligment, const VECTOR2::VEC2& _parentSize);


    /// <summary>
    /// ※テンプレートメソッド
    /// <para>フォーマット指定</para>
    /// <para>必ず文字列の中に"{:d}"みたいなのをフォーマット分含めること！！</para>
    /// </summary>
    /// <typeparam name="...Args"></typeparam>
    /// <param name="str"></param>
    /// <param name="_pos"></param>
    /// <param name="options"></param>
    /// <param name="...args"></param>
    template <typename... Args>
    void DrawFormatString(std::string str, const VECTOR2::VEC2& _pos, const std::string& formatTag,  Args&&... args)
    {
#ifndef IS_WRITE_ENABLE
        return;
#else
        HRESULT hr = S_OK;

        Microsoft::WRL::ComPtr<IDWriteTextLayout> pTextLayout;  // テキスト情報

        // フォーマット検索
        // 見つけた指定子にくっつける
        str = std::vformat(str, std::make_format_args(args...));

        // ワイド文字に変換
        std::wstring wstr = StringToWString(str);

        // レンダーターゲットのサイズ取得
        D2D1_SIZE_F RVSize = m_pRenderTarget->GetSize();

        // テキストレイアウトの作成
        hr = m_pWriteFactory->CreateTextLayout(
            wstr.c_str(),
            static_cast<UINT32>(wstr.size()),
            m_pTextFormatMap[formatTag].Get(),
            RVSize.width,
            RVSize.height,
            pTextLayout.GetAddressOf()
        );
        CHECK_HRESULT_NO_BOOL(hr);

        // 描画位置の確定
        D2D1_POINT_2F pos = { 0,0 };
        pos.x = _pos.x;
        pos.y = _pos.y;

        // アウトラインの描画
        if (m_OutLineSize > 0.0f)
        {
            this->DrawOutLine(pos, pTextLayout);
            m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
        }

        // 描画
        m_pRenderTarget->DrawTextLayout(
            pos,
            pTextLayout.Get(),
            m_pSolidBrush,
            D2D1_DRAW_TEXT_OPTIONS_NONE
        );
#endif
    }
};


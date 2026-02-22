#pragma once


/* =========================================================================
/* - @:BUFFER_TYPE 列挙体 Class - */
/* 【?】バッファの種別
/* =========================================================================*/
enum class BUFFER_TYPE
{
    NONE,       // なし
    VERTEX,     // 頂点バッファ
    INDEX,      // インデックスバッファ

    NUM,
};

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:BufferManeger Class --- */
//
//  ★★★シングルトン★★★
//
// 【?】頂点バッファ、インデックスバッファを作成、管理
//
// ***************************************************************************************
class BufferManeger
{
private:
    std::weak_ptr<class RendererEngine> m_pRenderer;    // 描画クラスの弱参照を持つ


public:
    /// <summary>
    /// シングルトンインスタンス取得
    /// </summary>
    /// <returns></returns>
    static BufferManeger &Instance() { static BufferManeger instance; return instance; }


    /// <summary>
    /// 初期化
    /// </summary>
    /// <returns>初期化出来たか</returns>
    bool Init(std::shared_ptr<class RendererEngine> renderer);

    /// <summary>
    /// シェーダの作成
    /// </summary>
    /// <param name="type">シェーダタイプ</param>
    /// <returns>作成出来たか</returns>
    bool CreateBuffer(BUFFER_TYPE type);
};


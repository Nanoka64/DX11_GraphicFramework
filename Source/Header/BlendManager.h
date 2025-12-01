#pragma once

class IDX_BlendState;

//* =========================================================================
//* - @:BLEND_MODE列挙体 - */
//* 【?】ブレンドモード
//* =========================================================================
enum class BLEND_MODE
{
    NONE,   // 何もしない
    ALPHA,  // アルファ
    ADD,    // 加算
    SUB,    // 減算

    NUM,
};

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:BlendManager Class --- */
//
//  ★★★シングルトン★★★
//
// 【?】IDX_BlendStateの所有権（外部に晒すことはほとんどないと思うのでunique_ptrで持つ）と、
//      Contextに対してブレンドステートの変更を行う役割を持つ
//      レンダリングターゲットごとにステートを分けたい場合もあると思うので、レンダリングターゲット番号を指定できるようにする。
//      シェーダと同じようにブレンドステートは事前定義（一応外部から作成関数を呼び出すようにする）
//      にして、外部からブレンドステートをenumで指定できるようにする。
//
// ***************************************************************************************
class BlendManager
{
private:
    std::weak_ptr<class RendererEngine> m_pRenderer;                                   // 描画クラスの弱参照を持つ
    ID3D11DeviceContext *m_pContext;
    std::unique_ptr<IDX_BlendState> m_pBlendStateArray[(int)BLEND_MODE::NUM];           // ブレンドステートの本体配列
    std::unique_ptr<IDX_BlendState> m_pBlendStateIndependentArray[(int)BLEND_MODE::NUM];// 独立ブレンドステート（使うかわからん）
public:
    /// <summary>
    /// インスタンス取得
    /// </summary>
    /// <returns></returns>
    static BlendManager &Instance(){ static BlendManager inst; return inst;};

    /// <summary>
    /// 初期化
    /// </summary>
    /// <returns>初期化出来たか</returns>
    bool Init(std::shared_ptr<class RendererEngine> renderer);

    /// <summary>
    /// ブレンドステートを設定する
    /// </summary>
    /// <param name="type">ブレンド種別</param>
    void DeviceToSetBlendState(BLEND_MODE type);

    /// <summary>
    /// レンダリングターゲットごとブレンドを設定する
    /// ※まだ使ってなーい
    /// </summary>
    /// <param name="rt_No">反映させるレンダリングターゲット番号</param>
    /// <param name="type">ブレンド種別</param>
    void DeviceToSetIndependentBlendState(int rt_No, BLEND_MODE type);


private:
    // プライベートコンストラクタ・デストラクタ
    BlendManager();
    ~BlendManager();
    BlendManager(const BlendManager &) = delete;
    BlendManager &operator=(const BlendManager &) = delete;
    // ------------------------------------------------------

    /// <summary>
    /// ブレンドステートの作成
    /// </summary>
    /// <param name="type">ブレンド種別</param>
    /// <param name="out">出力先</param>
    /// <returns></returns>
    std::unique_ptr<IDX_BlendState> BlendStateFactory(BLEND_MODE type);
};


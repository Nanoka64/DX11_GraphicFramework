#pragma once



// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:RenderPipeline Class --- */
//
// 【?】レンダリングの一連の流れを管理するクラス
//      RendererEngineが実体を持つ
//
// ***************************************************************************************
class RenderPipeline
{
private:
    // Gバッファ用
    class DX_RenderTarget *m_pAlbedo_RT;        // アルベド用
    class DX_RenderTarget *m_pNormal_RT;        // 法線用
    class DX_RenderTarget *m_pDepth_RT;         // 深度用
    class DX_RenderTarget *m_pSpecular_RT;      // スペキュラ用
    
    class DX_RenderTarget *m_pSceneFinal_RT;	// シーン最終合成用
    class DX_RenderTarget *m_pLuminance_RT;		// 輝度抽出用
    class DX_RenderTarget *m_pShadowMap_RT;		// シャドウマップ（ライトから見た深度を書き込む）

    static const int NUM_WEIGHTS = 8;
    float m_GaussWeights[NUM_WEIGHTS];  // ガウス重み係数

    static const int BLUR_COUNT = 4;    // ブラー回数（ブルーム用）
    class GaussianBlur *m_pBloomGaussianBlur;// ブルーム用ガウスブラー

    class GaussianBlur *m_pDoF_GaussianBlur; // 被写界深度用ガウスブラー
    float m_DoF_BlurIncensity;  // DOFブラーの強さ


    // レンダーターゲットのスプライト
    std::shared_ptr<class SpriteRenderer> m_pAlbed_Sprite;
    std::shared_ptr<class SpriteRenderer> m_pNormal_Sprite;
    std::shared_ptr<class SpriteRenderer> m_pSpecular_Sprite;
    std::shared_ptr<class SpriteRenderer> m_pDepth_Sprite;
    std::shared_ptr<class SpriteRenderer> m_pDefferdLighting_Sprite;
    std::shared_ptr<class SpriteRenderer> m_pLuminance_Sprite;
    std::shared_ptr<class SpriteRenderer> m_pCopyToFrameBuffer_Sprite;
    std::shared_ptr<class SpriteRenderer> m_pSceneFinal_Sprite;
    std::shared_ptr<class SpriteRenderer> m_pDoF_Sprite;
public:
    RenderPipeline();
    ~RenderPipeline();

    /// <summary>
    /// パイプラインを作成する
    /// </summary>
    /// <returns>正常に終了したか</returns>
    bool Setup(RendererEngine& renderer);

    /// <summary>
    /// パイプラインの実行
    /// </summary>
    void Execute(RendererEngine &renderer);

    /// <summary>
    /// 解放
    /// </summary>
    void Release();

private:
    //
    // 各描画パスの処理
    //
    void Geometry_PathRender(RendererEngine &renderer);
    void Shadow_PathRender(RendererEngine &renderer);
    void Lighting_PathRender(RendererEngine &renderer);
    void Forward_PathRender(RendererEngine &renderer);
    void PostEffect_PathRender(RendererEngine &renderer);
    void CopyToFrameBuffer_PathRender(RendererEngine &renderer);

    /// <summary>
    /// レンダーターゲットの作成
    /// </summary>
    /// <param name="renderer"></param>
    /// <returns></returns>
    bool CreateRenderTargets(RendererEngine &renderer);

    /// <summary>
    /// ポストエフェクトの作成
    /// </summary>
    /// <param name="renderer"></param>
    /// <returns></returns>
    bool CreatePostEffect(RendererEngine &renderer);

    /// <summary>
    /// レンダーターゲット用スプライトの作成
    /// </summary>
    /// <param name="renderer"></param>
    /// <returns></returns>
    bool CreateRenderTargetSprites(RendererEngine &renderer);
};


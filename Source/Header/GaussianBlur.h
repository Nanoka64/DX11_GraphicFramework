#pragma once


// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:GaussianBlur Class --- */
//
// 【?】指定のテクスチャにガウシアンブラーを掛ける
//		魔導書の内容を元に組み込み
//
// ***************************************************************************************
class GaussianBlur
{
private:
	static const int NUM_WEIGHTS = 8;
	float m_weights[NUM_WEIGHTS];

	std::weak_ptr<class Texture> m_pTexture;	// ブラー処理をするテクスチャ
	class DX_RenderTarget* m_pHorizontalBlur;   // 水平ブラー
	class DX_RenderTarget* m_pVerticalBlur;		// 垂直ブラー
	std::weak_ptr<class GameObject> m_pHorizontalBlurSprite;// 水平ブラースプライト
	std::weak_ptr<class GameObject> m_pVerticalBlurSprite;	// 垂直ブラースプライト
	int m_Id;	// タグ被りしてしまうため識別番号を持たせる

public:
	GaussianBlur();
	~GaussianBlur();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="pTex">ブラーを掛けるテクスチャ</param>
	/// <returns></returns>
	bool Setup(RendererEngine& renderer, std::shared_ptr<class Texture> pTex, int id);
	void Term();

	void ExcuteOnGPU(RendererEngine& renderer, float blurPow);

	/// <summary>
	/// ブラー実行後のテクスチャを取得
	/// </summary>
	/// <returns></returns>
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> get_AfterBlurTexture()const;

private:
	/// <summary>
	/// レンダリングターゲットを初期化。
	/// </summary>
	bool InitRenderTargets(RendererEngine& renderer);

	/// <summary>
	/// スプライトを初期化。
	/// </summary>
	bool InitSprites(RendererEngine& renderer);
};


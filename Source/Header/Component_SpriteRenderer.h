#pragma once
#include "IComponent.h"
#include "MeshInfoFactory.h"


/// <summary>
/// スプライト種別
/// </summary>
enum class SPRITE_USAGE_TYPE
{
	NORMAL,			// 通常
	RENDER_TARGET,	// レンダーターゲット用
};



// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:SpriteRenderer Class --- */
//
//  ★継承：IComponent ★
//
// 【?】スプライトの描画をする	
//		板ポリの情報もこの中で完結させる。
//		MeshRendererみたいにMeshResourceに情報を置くようなことはしないということ.
//		
// ***************************************************************************************
class SpriteRenderer : public IComponent
{
private:
	MeshInfo *m_pMeshInfo;	// マテリアルは使わない

	ID3D11Buffer *m_pVertexBuffer;	// 頂点バッファ
	ID3D11Buffer *m_pIndexBuffer;	// インデックスバッファ
	CB_TRANSFORM_SET *m_pCBTransformSet;		// 定数バッファ(ワールド行列用)
	std::weak_ptr<class Texture> m_pTexture;

	float m_Width;
	float m_Height;

public:
	SpriteRenderer(std::weak_ptr<GameObject> pOwner, int updateRank = 100);
	~SpriteRenderer();

	bool Setup(RendererManager &renderer, SPRITE_USAGE_TYPE type, std::weak_ptr<class Texture> pTex, float w, float h);

	void Init(RendererManager &renderer) override;		// 初期化
	void Update(RendererManager &renderer) override;	// 更新処理
	void Draw(RendererManager &renderer) override;		// 描画処理


	void set_Width(float w);
	void set_Height(float h);
	float get_Width()const;
	float get_Height()const;

private:

	/// <summary>
	/// 頂点情報の更新
	/// </summary>
	void VertexUpdate(RendererManager &renderer);

	// ※const void*はどんな型のアドレスでも受け取れる
	/// <summary>
	/// 頂点バッファの作成
	/// </summary>
	/// <param name="pDevice">デバイス</param>
	/// <param name="pVertices">頂点データ配列</param>
	/// <param name="sizeVertex">頂点データのサイズ</param>
	/// <param name="numVertices">頂点の数</param>
	/// <returns></returns>
	bool CreateVertexBuffer(ID3D11Device *pDevice, const void *pVertices, UINT vertexStride, UINT numVertices);


	/// <summary>
	/// インデックスバッファの作成
	/// </summary>
	/// <param name="pDevice">デバイス</param>
	/// <param name="pIndices">インデックスデータ配列</param>
	/// <param name="sizeIndex">インデックスデータのサイズ</param>
	/// <param name="numIndices">インデックスの数</param>
	/// <returns></returns>
	bool CreateIndexBuffer(ID3D11Device *pDevice, const void *pIndices, UINT indexStride, UINT numIndices);


	/// <summary>
	/// 定数バッファの作成
	/// </summary>
	/// <param name="pDevice">デバイス</param>
	/// <returns></returns>
	bool CreateCBuffer(ID3D11Device *pDevice);
};


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
	std::map<int , std::weak_ptr<class Texture>> m_pTextureMap;	// スロット番号とtexture

	float m_Width;
	float m_Height;

	SHADER_TYPE m_ShaderType;	// 使用するシェーダの種類

	CB_USER_EXPAND_SET *m_pVSUserExpandCBuffers;	// VSユーザー拡張用定数バッファ
	int m_VSUserExpandCBNum;						// VSユーザー拡張用定数バッファ数
	CB_USER_EXPAND_SET * m_pPSUserExpandCBuffers;	// PSユーザー拡張用定数バッファ
	int m_PSUserExpandCBNum;						// PSユーザー拡張用定数バッファ数

public:
	SpriteRenderer(std::weak_ptr<GameObject> pOwner, int updateRank = 100);
	~SpriteRenderer();

	bool Setup(const struct CreateSpriteInfo& info);

	void Init(RendererEngine &renderer) override;		// 初期化
	void Update(RendererEngine &renderer) override;		// 更新処理
	void Draw(RendererEngine &renderer) override;		// 描画処理


	void set_Width(float w);
	void set_Height(float h);
	float get_Width()const;
	float get_Height()const;

	/// <summary>
	/// 初期化時に設定したユーザー拡張用頂点定数バッファをGPUにセットする
	/// あくまで臨時の初期化時のデータ更新用なので全く違うものを入れないでね
	/// </summary>
	/// <param name="arrayNumber">初期化時にセットした配列番号</param>
	/// <param name="_pSrn">データ</param>
	void setToGPU_ExtendUserVS_CBuffer(RendererEngine& renderer,int arrayNumber,void* _pSrn);


	/// <summary>
	/// 初期化時に設定したユーザー拡張用ピクセル定数バッファをGPUにセットする
	/// あくまで臨時の初期化時のデータ更新用なので全く違うものを入れないでね
	/// </summary>
	/// <param name="arrayNumber">初期化時にセットした配列番号</param>
	/// <param name="_pSrn">データ</param>
	void setToGPU_ExtendUserPS_CBuffer(RendererEngine& renderer, int arrayNumber,void* _pSrn);

private:

	/// <summary>
	/// 頂点情報の更新
	/// </summary>
	void VertexUpdate(RendererEngine &renderer);

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

	/// <summary>
	/// ユーザー定義拡張用定数バッファの作成
	/// </summary>
	/// <param name="pDevice"></param>
	/// <param name="info"></param>
	/// <returns></returns>
	bool CreateUserExpandCBuffer(RendererEngine& renderer, UINT byteWidth, void* pSrc);
};


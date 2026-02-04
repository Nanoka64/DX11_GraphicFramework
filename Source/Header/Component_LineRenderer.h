#pragma once
#include "IComponent.h"


// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:LineRenderer Class --- */
//
//  ★継承：IComponent ★
//
// 【?】線を描画する
//		トレイルレンダラーと違い、頂点は4つのみ
//		Planeメッシュに近いけど、
//		ビルボードかつ方向と長さを指定できるようにする
// 
// ***************************************************************************************
class LineRenderer : public IComponent
{
private:
	float m_Width;					// 幅
	int m_DrawTime;					// 表示時間

	VECTOR3::VEC3 m_StartPos;		// 始点
	VECTOR3::VEC3 m_Dir;			// 線の方向
	float m_Length;					// 長さ


	Microsoft::WRL::ComPtr<ID3D11Buffer> m_pVertesBuffer;// 頂点バッファ
	bool m_IsView;								// カメラに向くようにするか（ビルボード）
	CB_MATERIAL_SET* m_pCBMaterialDataSet;		// 定数バッファ(マテリアル用)
	std::shared_ptr<class Texture> m_pTex;		//


public:
	LineRenderer(std::weak_ptr<GameObject> pOwner, int updateRank = 100);
	~LineRenderer();

	void Start(RendererEngine& renderer) override;		// 初期化
	void Update(RendererEngine& renderer) override;		// 更新処理
	void Draw(RendererEngine& renderer)override;		// 描画処理

	void set_Width(const float _w) { m_Width = _w; };		// 幅の設定
	float get_Width()const { return m_Width; };				// 幅の取得
	void set_DrawTime(const float _t) { m_DrawTime = _t; }	// 表示時間の設定
	float get_DrawTime()const { return m_DrawTime; }		// 表示時間の取得
	void set_Length(const float _l) { m_Length = _l; }		// 長さ設定
	float get_Length()const { return m_Length; }			// 長さ取得
	void set_Dir(const VECTOR3::VEC3& _v) { m_Dir = _v; }				// 方向設定
	VECTOR3::VEC3 get_Dir()const { return m_Dir; }						// 方向取得
	void set_StartPos(const VECTOR3::VEC3& _v) { m_StartPos = _v; }		// 始点設定
	VECTOR3::VEC3 get_StartPos()const { return m_StartPos; }			// 始点取得

private:
	bool Setup(RendererEngine& renderer);					// 初期化
	bool CreateConstantBuffer(RendererEngine& renderer);	// 定数バッファ作成
	bool CreateVertexBuffer(RendererEngine& renderer);		// 頂点バッファ作成
	void VertexUpdate(RendererEngine& renderer);			// 頂点更新
	void ConstantBufferUpdate(RendererEngine& renderer);	// 定数バッファ更新
};


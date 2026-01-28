#pragma once
#include "IComponent.h"

/// <summary>
/// 軌跡に必要な位置情報
/// </summary>
struct TrailInfo
{
	VECTOR3::VEC3 _headPos;	// 先端位置
	VECTOR3::VEC3 _tailPos;	// 末端位置

	TrailInfo() :
		_headPos(VECTOR3::VEC3()),
		_tailPos(VECTOR3::VEC3())
	{};
};

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:TrailRenderer Class --- */
//
//  ★継承：IComponent ★
//
// 【?】軌跡を描画する
//		参考サイト：https://qiita.com/himego/items/c043ca5d58912b9a58d2#4%E3%81%BE%E3%81%A8%E3%82%81
// 
// ***************************************************************************************
class TrailRenderer : public IComponent
{
private:
	std::deque<VECTOR3::VEC3> m_TrailPosList;			 // 軌跡位置リスト
	std::vector<VERTEX::VERTEX_Static> m_VertexList; // 頂点リスト
	VECTOR3::VEC3 m_CrntTrailPos;	// 現在の軌跡位置
	float m_Width;					// 幅
	float m_MinVertexDistance;		// 頂点同士の距離（大きいほどカクカク）
	float m_DrawTime;				// 表示時間
	const UINT MAX_TRAIL_VERTEX_NUM = 524;	// 最大長点数
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_pVertesBuffer;// 頂点バッファ
	bool m_IsView;								// カメラに向くようにするか（ビルボード）
	CB_MATERIAL_SET *m_pCBMaterialDataSet;		// 定数バッファ(マテリアル用)
public:
	TrailRenderer(std::weak_ptr<GameObject> pOwner, int updateRank = 100);
	~TrailRenderer();


	void Start(RendererEngine &renderer) override;		// 初期化
	void Update(RendererEngine &renderer) override;		// 更新処理
	void Draw(RendererEngine &renderer)override;		// 描画処理

	void set_Width(const float _w) { m_Width = _w; };	// 幅の設定
	float get_Width()const { return m_Width; };			// 幅の取得
	void set_MinVertexDistance(const float _dist) { m_MinVertexDistance = _dist; }	// 頂点同士の距離の設定
	float get_MinVertexDistance()const { return m_MinVertexDistance; }				// 頂点同士の距離の取得
	void set_DrawTime(const float _t) { m_DrawTime = _t; }	// 表示時間の設定
	float get_DrawTime()const { return m_DrawTime; }		// 表示時間の取得

private:
	bool Setup(RendererEngine& renderer);
	bool CreateConstantBuffer(RendererEngine &renderer);
	bool CreateVertexBuffer(RendererEngine &renderer);
	void set_TrailPos(VECTOR3::VEC3 _head,VECTOR3::VEC3 _tail);

	void VertexUpdate(ID3D11DeviceContext* pContext);
	void ConstantBufferUpdate(RendererEngine &renderer);

	/// <summary>
	/// 軌跡をカメラ方向に向かせる
	/// </summary>
	/// <param name="renderer"></param>
	void CalcTrailSegmentToViewDirection(RendererEngine &renderer);

};


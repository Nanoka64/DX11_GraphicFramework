#pragma once
#include "Component_Light.h"

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:DirectionalLight Class --- */
//
//  ★継承：Light ← Component ★
//
// 【?】ディレクションライト
//
// ***************************************************************************************
class DirectionalLight : public Light
{
private:
	std::weak_ptr<class GameObject> m_pLigCamTrackingObj;
	float m_FocusOffsetDistance;	// フォーカスからのオフセット
	float m_OrthographicWidth;		// 横幅
	float m_OrthographicHeigh;		// 縦幅
	float m_ShadowDistance;			// 影の範囲

public:
	DirectionalLight(std::weak_ptr<GameObject> pOwner, int updateRank);
	~DirectionalLight();

	void Init(RendererEngine &renderer) override;	// 初期化
	void Update(RendererEngine &renderer) override;// 更新
	void Draw(RendererEngine &renderer) override;	// 描画

	void set_LightCameraTrackingObj(std::shared_ptr<class GameObject> pObj);

	float get_ShadowFocusOffsetDistance()const { return m_FocusOffsetDistance; };
	void set_ShadowFocusOffsetDistance(float _ofs) { m_FocusOffsetDistance = _ofs; };

	float get_OrthographicWidth()const { return m_OrthographicWidth; };
	void set_OrthographicWidth(float _w) { m_OrthographicWidth = _w; };
	float get_OrthographicHeight()const { return m_OrthographicHeigh; };
	void set_OrthographicHeight(float _h) { m_OrthographicHeigh = _h; };

	float get_ShadowDistance()const { return m_ShadowDistance; };
	void set_ShadowDistance(float _dist) { m_ShadowDistance = _dist; };

};


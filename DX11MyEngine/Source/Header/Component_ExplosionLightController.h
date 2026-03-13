#pragma once
#include "IComponent.h"

struct ExplosionLightData
{
	float _explosionLightRadius = 0.0f;		// 爆発時の発光最大範囲
	float _normalRadius = 0.0f;				// 通常時の発光範囲
	float _explosionDuration = 0.0f;		// 爆発時の発光時間

	/// <summary>
	/// リセット
	/// </summary>
	void Reset()
	{
		*this = ExplosionLightData();
	}
};

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:ExplosionLightController Class --- */
//
//  ★継承：IComponent ★
//
// 【?】爆発弾のポイントライトの挙動
//
// ***************************************************************************************
class ExplosionLightController : public IComponent
{
private:
	ExplosionLightData m_Parameter;
	float m_Timer;
	std::weak_ptr<class PointLight> m_pPointLight;

public:
	ExplosionLightController(std::weak_ptr<GameObject> pOwner, int updateRank = 100);
	~ExplosionLightController();

	void Start(RendererEngine &renderer) override;		// 初期化
	void Update(RendererEngine &renderer) override;		// 更新処理
	void Draw(RendererEngine &renderer)override;		// 描画処理

	/// <summary>
	/// パラメータの設定
	/// </summary>
	/// <param name="_param">パラメータ</param>
	void set_Parameter(const ExplosionLightData &_param) { m_Parameter = _param; };

	/// <summary>
	/// プールへの返却時に実行させる
	/// </summary>
	void Reset();

	/// <summary>
	/// プールからの取り出し時に実行させる
	/// </summary>
	void Setup();
};


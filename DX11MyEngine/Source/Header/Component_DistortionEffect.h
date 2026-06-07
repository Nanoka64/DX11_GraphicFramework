#pragma once
#include "IComponent.h"
#include "MeshInfoFactory.h"

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:DistortionEffect Class --- */
//
//  ★継承：IComponent ★
//
// 【?】ディストーション（陽炎）オブジェクト
//		
// ***************************************************************************************
class DistortionEffect : public IComponent
{
private:
	float m_DistortionPower;   // 歪みの強さ
	float m_DistortionSpeed;   // 歪みの速度
	float m_DistortionScale;   // 歪みのスケール
	CB_DISTORTION m_CBDistortion;	// 定数バッファ用データ
	std::shared_ptr<MeshResourceData> m_pMeshData;	// メッシュ情報

public:
	DistortionEffect(std::weak_ptr<GameObject> pOwner, int updateRank);
	~DistortionEffect();

	void Start(RendererEngine& renderer) override;	// 初期化
	void Update(RendererEngine& renderer) override;	// 更新
	void Draw(RendererEngine& renderer) override;	// 描画
	bool Setup(RendererEngine& renderer);	// 初期化
};


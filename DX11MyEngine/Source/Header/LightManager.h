#pragma once
#include "TransientPointLightPool.h"

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:LightManager Class --- */
//
//  ★★★シングルトン★★★
//
// 【?】ライトの情報をまとめてライティングパス前に送る
//
// ***************************************************************************************
class LightManager
{
private:
	//CB_POINT_LIGHT_SET *m_pCBPointLightSet;
	//CB_DIRECTION_LIGHT_SET *m_pCBDirectionLightSet;
	std::weak_ptr<class MyTransform> m_pCameraTransform;  // カメラの座標取得のために保持
	std::weak_ptr<RendererEngine> m_pRenderer;
	ID3D11DeviceContext* m_pContext;

	std::vector<CB_PointLightData>	m_TemporaryPointLightData;		// ポイントライト情報の一時保持
	std::vector<CB_DirectionLightData> m_TemporaryDirectionLightData;  // ディレクションライト情報の一時保持

	TransientPointLightPool m_TransientPointLightPool;
public:
	LightManager();
	~LightManager();

	bool Init(std::shared_ptr<RendererEngine> pRenderer);
	void Update();
	void Term();

	/// <summary>
	/// カメラのトランスフォームセット
	/// </summary>
	/// <param name="pCam"></param>
	void set_CameraTransform(std::weak_ptr<class MyTransform> pCamTransform);

	void set_PointLightData(const CB_PointLightData& data);
	void set_DirectionLightData(const CB_DirectionLightData& data);

	/// ライトビュープロジェクションの設定（多分ディレクションライト専用）

	/// <summary>
	/// 一時ポイントライトの生成
	/// </summary>
	/// <param name="position">位置</param>
	/// <param name="desc">生成に必要な情報</param>
	/// <returns>ハンドル</returns>
	TransientLightHandle PlayTransientPointLight(
		const VECTOR3::VEC3& position,
		const TransientPointLightDesc& desc);
	void StopTransientPointLight(TransientLightHandle handle);
	void ClearTransientPointLights();

private:
	// コピー禁止用
	LightManager(const LightManager&) = delete;
	LightManager operator=(const LightManager&) = delete;
	// ------------------------------------------------------

	/// <summary>
	/// ディレクションライトを定数バッファにセット
	/// </summary>
	void DirectionLight_SetCBuffer();


	/// <summary>
	/// ポイントライトを定数バッファにセット
	/// </summary>
	void PointLight_SetCBuffer();


	/// <summary>
	/// ポイントライト用の定数バッファを作成
	/// </summary>
	/// <param name="pDevice"></param>
	/// <returns></returns>
	bool CreatePointLightCBuffer(ID3D11Device* pDevice);

	/// <summary>
	/// ディレクションライト用の定数バッファを作成
	/// </summary>
	/// <param name="pDevice"></param>
	/// <returns></returns>
	bool CreateDirectionLightCBuffer(ID3D11Device* pDevice);
};

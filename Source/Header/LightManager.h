#pragma once


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
	CB_POINT_LIGHT_SET *m_pCBPointLightSet;
	CB_DIRECTION_LIGHT_SET *m_pCBDirectionLightSet;
	std::weak_ptr<class Transform> m_pCameraTransform;  // カメラの座標取得のために保持
	std::weak_ptr<RendererEngine> m_pRenderer;
	ID3D11DeviceContext* m_pContext;

	std::vector<CB_POINT_LIGHT>		m_TemporaryPointLightData;		// ポイントライト情報の一時保持
	std::vector<CB_DIRECTION_LIGHT> m_TemporaryDirectionLightData;  // ディレクションライト情報の一時保持

public:
	LightManager();
	~LightManager();

	bool Init(std::shared_ptr<RendererEngine> pRenderer);
	void Update();

	/// <summary>
	/// カメラのトランスフォームセット
	/// </summary>
	/// <param name="pCam"></param>
	void set_CameraTransform(std::weak_ptr<class Transform> pCamTransform);

	void set_PointLightData(const CB_POINT_LIGHT& data);
	void set_DirectionLightData(const CB_DIRECTION_LIGHT& data);

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
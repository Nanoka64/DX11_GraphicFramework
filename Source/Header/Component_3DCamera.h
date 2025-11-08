#pragma once
#include "IComponent.h"

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:Camera3D Class --- */
//
//  ★継承：IComponent ★
//
// 【?】3Dカメラコンポーネント
//
// ***************************************************************************************
class Camera3D : public IComponent
{
private:
	VECTOR3::VEC3 m_EyePos;
	VECTOR3::VEC3 m_FocusPoint;
	VECTOR3::VEC3 m_UpVec;

	float m_Angle_H;	// 水平方向アングル
	float m_Angle_V;	// 垂直方向アングル

	// FPS視点の際、現在位置からの補正値を足した場所を注視場所にする
	// どのくらい離れた位置を注視点とするか
	const float CASE_OF_FPS_FOCUSPOINT = 200.0f;

public:
	Camera3D(std::weak_ptr<GameObject> pOwner, int updateRank = 100);
	~Camera3D();

	void Init(RendererManager& renderer) override;		// 初期化
	void Update(RendererManager& renderer) override;	// 更新処理
	void Draw(RendererManager& renderer)override;		// 描画処理


	/// <summary>
	/// ビュー変換行列の取得
	/// </summary>
	DirectX::XMMATRIX get_ViewMatrix()const;
};


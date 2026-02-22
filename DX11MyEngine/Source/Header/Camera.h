//#pragma once
////--------------------------------------------------------------------------------------
////      * Includes *
////--------------------------------------------------------------------------------------
//#include "GameObject3D.h"
//
//// ***************************************************************************************
//// ---------------------------------------------------------------------------------------
///* --- @:Camera Class --- */
////
////  ★継承：GameObject3D ← Transform ← Object ★
////
//// 【?】3D空間上のカメラ
////
//// ***************************************************************************************
//class Camera : public GameObject3D
//{
//private:
//	VECTOR3::VEC3 m_EyePos;
//	VECTOR3::VEC3 m_FocusPoint;
//	VECTOR3::VEC3 m_UpVec;
//
//	float m_Angle_H;	// 水平方向アングル
//	float m_Angle_V;	// 垂直方向アングル
//
//	// FPS視点の際、現在位置からの補正値を足した場所を注視場所にする
//	// どのくらい離れた位置を注視点とするか
//	const float CASE_OF_FPS_FOCUSPOINT = 200.0f;
//
//public:
//	/// <summary>
//	/// コンストラクタ
//	/// </summary>
//	Camera();
//
//
//	/// <summary>
//	/// デストラクタ
//	/// </summary>
//	~Camera();
//
//	bool Init(RendererManager &renderer) override;	// 初期化
//	void Update(RendererManager &renderer) override;// 更新
//	void Draw(RendererManager &renderer) override;	// 描画
//	bool Term(RendererManager &renderer) override;	// 終了
//	
//	/// <summary>
//	/// ビュー変換行列の取得
//	/// </summary>
//	DirectX::XMMATRIX get_ViewMatrix()const;	
//};
//

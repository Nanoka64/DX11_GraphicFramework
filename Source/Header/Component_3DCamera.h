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
	VECTOR3::VEC3 m_FocusPoint;
	VECTOR3::VEC3 m_UpVec;

	float m_Angle_H;	// 水平方向アングル
	float m_Angle_V;	// 垂直方向アングル

	std::weak_ptr<class GameObject> m_pFocusObject;		// フォーカス対象
	VECTOR3::VEC3 m_PosOffset;							// 対象からどのくらい離れて球面移動するか
	VECTOR3::VEC3 m_FocusOffset;						// フォーカス位置のオフセット
public:
	Camera3D(std::weak_ptr<GameObject> pOwner, int updateRank = 100);
	~Camera3D();

	void Init(RendererEngine& renderer) override;		// 初期化
	void Update(RendererEngine& renderer) override;		// 更新処理
	void Draw(RendererEngine& renderer)override;		// 描画処理

	void set_UpVec(const VECTOR3::VEC3& upVec);
	void set_FocusPoint(const VECTOR3::VEC3& focus);
	void set_FocusObject(std::weak_ptr<class GameObject> pObj);

	VECTOR3::VEC3 get_UpVec()const;
	VECTOR3::VEC3 get_FocusPoint()const;

	/// <summary>
	/// ビュー変換行列の取得
	/// </summary>
	DirectX::XMMATRIX get_ViewMatrix()const;
};


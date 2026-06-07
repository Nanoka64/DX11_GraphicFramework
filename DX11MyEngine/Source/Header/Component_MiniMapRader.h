#pragma once
#include "IComponent.h"


// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:MiniMapRader Class --- */
//
//  ★継承：IComponent ★
//
// 【?】ミニマップ用
//		
// ***************************************************************************************
class MiniMapRader :  public IComponent
{
private:
	const float MINIMAP_X = 1700.0f;    // ミニマップの中心X座標
	const float MINIMAP_Y = 150.0f;     // ミニマップの中心X座標
	const float MINIMAP_RADIUS = 120.0f;// ミニマップの半径
	const float MINIMAP_SIZE = 270.0f;  // ミニマップのサイズ
	const float MINIMAP_ENEMY_ICON_SIZE = 10.0f; // ミニマップ上の敵アイコンのサイズ

	const int ENEMY_ICON_LAYER = 111;	// 敵アイコンのレイヤー
	const int ITEM_ICON_LAYER = 110;	// アイテムアイコンのレイヤー

	class GameObject* m_pRaderBackSpriteObj;		// レーダー背景オブジェクト
	std::vector<class GameObject*> m_pRaderEnemySpriteObjArray;   // レーダーに表示する敵オブジェクト
	std::vector<class GameObject*> m_pRaderItemSpriteObjArray;    // レーダーに表示するアイテムオブジェクト
	std::weak_ptr<class GameObject>m_pPlayerObj;

	int m_MaxEnemyNum;	// レーダーに表示する最大敵数
	int m_CrntEnemyNum;	// 現在レーダーに表示している敵数

public:
	MiniMapRader(std::weak_ptr<GameObject> pOwner, int updateRank);
	~MiniMapRader();

	void Start(RendererEngine& renderer) override;	// 初期化
	void Update(RendererEngine& renderer) override;	// 更新

private:
	/// <summary>
	/// レーダー上のアイコンを更新する
	/// </summary>
	/// <param name="targetObjs">表示対象</param>
	/// <param name="iconSprites">表示するUI</param>
	/// <param name="centerPos">中心点（プレイヤー等）</param>
	/// <param name="cameraAngleH"> 基準となる角度</param>
	void UpdateRadarIcons(const std::vector<std::shared_ptr<GameObject>>& targetObjs, const std::vector<class GameObject*>& iconSprites, const VECTOR3::VEC3& centerPos, float cameraAngleH);

	/// <summary>
	/// ミニマップ内に収める
	/// </summary>
	/// <param name="_Out_X"></param>
	/// <param name="_Out_Y"></param>
	/// <param name="_centerX"></param>
	/// <param name="_centerY"></param>
	/// <param name="_radius"></param>
	void ClampToMinimap(int& _Out_X, int& _Out_Y, int _centerX, int _centerY, float _radius);
	
	/// <summary>
	/// ミニマップに表示するアイコンスプライトの確保
	/// </summary>
	/// <param name="renderer">描画エンジンの参照</param>
	/// <param name="_num">確保する数</param>
	/// <param name="_size">サイズ</param>
	/// <param name="_color">色</param>
	void SecuringMiniMapIconSprite(RendererEngine& renderer, int _num, std::vector<class GameObject*>& iconSprites, const VECTOR2::VEC2& _size, const VECTOR4::VEC4& _color, int _layer);
};


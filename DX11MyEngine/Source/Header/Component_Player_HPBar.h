#pragma once
#include "IComponent.h"
class Player_HPBar :  public IComponent
{
private:
	const float SIZE_X = 554.0f;				// HPバーの横幅
	const float SIZE_Y = 41.0f;					// HPバーの縦幅
	const float SHAKE_HPBAR_DURATION = 1.0f;	// ダメージを受けた際に、HPバーを揺らす時間
	const VECTOR2::VEC2 SHAKE_LENGTH = VECTOR2::VEC2(50.0f, 0.0f);			// 横に揺らすつよさ
	const VECTOR2::VEC2 HPBAR_BACK_POS = VECTOR2::VEC2(50.0f, -30.0f);		// HPバーの背景スプライトの位置
	const VECTOR2::VEC2 HPBAR_BACK_SIZE = VECTOR2::VEC2(600.0f, 330.0f);	// HPバーの背景スプライトのサイズ
	const VECTOR2::VEC2 HPBAR_POS = VECTOR2::VEC2(83.0f, 103.0f);			// HPバー本体スプライトの位置
	const VECTOR2::VEC2 HPBAR_SIZE = VECTOR2::VEC2(700.0f, 103.0f);			// HPバー本体スプライトのサイズ
	const VECTOR2::VEC2 CRNTHP_TEXT_POS = VECTOR2::VEC2(160.0f, 152.0f);	// 現在のHPテキストの位置
	const VECTOR2::VEC2 MAXHP_TEXT_POS = VECTOR2::VEC2(160.0f, 180.0);		// 最大HPテキストの位置
	const float DANGER_COLOR_FADE_SPEED = 20.0f;	// 危険体力の時のフェード速度

	std::weak_ptr<class GameObject>m_pPlayerObj;

	/* スプライト */
	class GameObject* m_pPlayerHPBarBackSpriteObj;	// プレイヤーのHPバー背景オブジェクト
	class GameObject* m_pPlayerHPBarSpriteObj;		// プレイヤーのHPバーオブジェクト
	std::weak_ptr<class SpriteRenderer> m_pPlayerHPBarBackSpriteRenderer;
	std::weak_ptr<class SpriteRenderer> m_pPlayerHPBarSpriteRenderer;

	float m_PrevHP;	// 前回のHPを保持（ダメージを受けたか、確かめるため）
	float m_Timer;
	Tool::VEC2_Shaker m_Shaker;

public:
	Player_HPBar(std::weak_ptr<GameObject> pOwner, int updateRank);
	~Player_HPBar();

	void Start(RendererEngine& renderer) override;	// 初期化
	void Update(RendererEngine& renderer) override;	// 更新

};


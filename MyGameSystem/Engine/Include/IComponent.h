#pragma once
#include "Object.h"

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:IComponent Class --- */
//
//  ★★★抽象クラス★★★
//
// 【?】コンポーネントの基底クラス
//
// ***************************************************************************************
class IComponent : public Object
{
protected:
	std::weak_ptr<class GameObject> m_pOwner;	// このコンポーネントの持ち主
	int m_UpdateRank;	// 更新の順番

public:
	IComponent(std::weak_ptr<class GameObject> pOwner, int updateRank = 100);	// updateRankが低いほど先に更新される
	virtual ~IComponent();

	virtual void Start(RendererEngine &renderer) {};	 // 初期化
	virtual void Update(RendererEngine& renderer) {};	 // 更新処理
	virtual void LateUpdate(RendererEngine &renderer) {};// 更新後の更新
	virtual void Draw(RendererEngine &renderer) {};		 // 描画処理
	
	/* 当たり判定関連 */
	virtual void OnCollisionEnter(const class CollisionInfo &_other) {} // 当たった瞬間
	virtual void OnCollisionStay(const class CollisionInfo &_other) {}	// 当たっている間
	virtual void OnCollisionExit(const class CollisionInfo &_other) {}	// 離れた瞬間

	int get_UpdateRank() const { return m_UpdateRank; }	// 更新の順番取得
	std::weak_ptr<GameObject> get_OwnerObj()const { return m_pOwner; };

	bool get_IsActiveOwnerObj()const { return (m_pOwner.expired() ? false : true); }	// オーナーが存在しているか確認
};


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
	std::weak_ptr<GameObject> m_pOwner;	// このコンポーネントの持ち主
	int m_UpdateRank;	// 更新の順番

public:
	IComponent(std::weak_ptr<class GameObject> pOwner, int updateRank = 100);	// updateRankが低いほど先に更新される
	virtual ~IComponent();

	virtual void Init(RendererEngine &renderer) {};		// 初期化
	virtual void Update(RendererEngine& renderer) {};		// 更新処理
	virtual void Draw(RendererEngine &renderer) {};		// 描画処理

	int get_UpdateRank() const { return m_UpdateRank; }	// 更新の順番取得
};


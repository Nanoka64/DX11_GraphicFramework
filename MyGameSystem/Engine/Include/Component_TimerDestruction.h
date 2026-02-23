#pragma once
#include "IComponent.h"


// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:TimerDestruction Class --- */
//
//  ★継承：IComponent ★
//
// 【?】指定した時間に削除されるようにするコンポーネント
//		一定時間で消えて欲しいオブジェクトにつける（エフェクトとか）
//		
// ***************************************************************************************
class TimerDestruction : public IComponent
{
private:
	float m_LifeTime;		// 消滅までの時間（デフォで10.0）
	float m_Counter;		// 経過時間

public:
	TimerDestruction(std::weak_ptr<GameObject> pOwner, int updateRank = 100);
	~TimerDestruction();

	void Update(RendererEngine& renderer) override;		// 更新処理
	void set_LifeTime(float _t) { m_LifeTime = _t; }	// 消滅時間の設定（必ず呼ぶ）
};


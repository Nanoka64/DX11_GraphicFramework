#pragma once
#include "IComponent.h"


/// <summary>
/// 移動挙動の種類
/// </summary>
enum class MOVE_BEHAVIOUR_TYPE : unsigned char
{
    NONE,

    STRAIGHT,   // 直線移動
    HOMING,     // ホーミング移動
};


// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:MoveLogic Class --- */
//
//  ★継承：Component ★
//
// 【?】移動ロジック
//
// ***************************************************************************************
class MoveLogic : public IComponent
{
private:
    std::unique_ptr<class IMoveBehaviour> m_pMoveBehaviour;	// 移動の挙動クラス


public:
    MoveLogic(std::weak_ptr<GameObject> pOwner, int updateRank = 100);
    ~MoveLogic();

    void Start(RendererEngine &renderer) override;		// 初期化
    void Update(RendererEngine &renderer) override;		// 更新処理


    void set_MoveBehaviour(MOVE_BEHAVIOUR_TYPE _type);
};


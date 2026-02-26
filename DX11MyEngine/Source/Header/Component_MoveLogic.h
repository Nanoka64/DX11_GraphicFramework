#pragma once
#include "IComponent.h"


/// <summary>
/// 移動挙動の種類
/// </summary>
enum class MOVE_BEHAVIOUR_TYPE : unsigned char
{
    NONE,

    LINEAR,     // 直線移動
    HOMING,     // ホーミング移動
};


// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:MoveLogic Class --- */
//
//  ★継承：Component ★
//
// 【?】移動ロジック
//      ビヘイビアを切り替えることで、移動の挙動を変えることができる
//
// ***************************************************************************************
class MoveLogic : public IComponent
{
private:
    std::unordered_map<MOVE_BEHAVIOUR_TYPE, std::unique_ptr<class IMoveBehaviour>> m_pMoveBehaviourMap;    // 移動挙動のマップ
    IMoveBehaviour *m_pMoveBehaviour = nullptr;	// 現在の移動挙動

public:
    MoveLogic(std::weak_ptr<GameObject> pOwner, int updateRank = 100);
    ~MoveLogic();

    void Start(RendererEngine &renderer) override;		// 初期化
    void Calculate(const struct MoveParam& _param);		    // 移動処理


    void Register(MOVE_BEHAVIOUR_TYPE _type);
    void ChangeBehaviour(MOVE_BEHAVIOUR_TYPE _type);
};


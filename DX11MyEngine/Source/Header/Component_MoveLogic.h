#pragma once
#include "IComponent.h"
#include "IMoveBehaviour.h"


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
    std::unordered_map<MOVE_BEHAVIOUR_TYPE, std::unique_ptr<IMoveBehaviour>> m_pMoveBehaviourMap;    // 移動挙動のマップ
    IMoveBehaviour *m_pMoveBehaviour = nullptr;	// 現在の移動挙動

public:
    MoveLogic(std::weak_ptr<GameObject> pOwner, int updateRank = 100);
    ~MoveLogic();

    void Start(RendererEngine &renderer) override;		// 初期化
    void Calculate(const struct MoveParam& _param);		    // 移動処理

    /// <summary>
    /// 移動挙動の登録
    /// </summary>
    /// <param name="_type">移動挙動の種類</param>
    void Register(MOVE_BEHAVIOUR_TYPE _type);

    /// <summary>
    /// 移動挙動の変更
    /// </summary>
    /// <param name="_type">変更する移動挙動の種類</param>
    void ChangeBehaviour(MOVE_BEHAVIOUR_TYPE _type);
};


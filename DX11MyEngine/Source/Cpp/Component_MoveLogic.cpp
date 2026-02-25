#include "pch.h"
#include "Component_MoveLogic.h"
#include "IMoveBehaviour.h"
#include "GameObject.h"

using namespace GIGA_Engine;
using namespace VECTOR3;

//*---------------------------------------------------------------------------------------
//*【?】コンストラクタ
//* [引数]
//* pOwner : オーナーオブジェクト
//* updateRank : 更新レイヤー
//*----------------------------------------------------------------------------------------
MoveLogic::MoveLogic(std::weak_ptr<GameObject> pOwner, int updateRank)
    :IComponent(pOwner, updateRank)
{
    this->set_Tag("MoveLogic");
}

//*---------------------------------------------------------------------------------------
//*【?】デストラクタ
//*----------------------------------------------------------------------------------------
MoveLogic::~MoveLogic()
{

}
//*---------------------------------------------------------------------------------------
//*【?】機能概要
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//*
//* [返値] なし
//*----------------------------------------------------------------------------------------
void MoveLogic::Start(RendererEngine &renderer)
{
    // デフォルトは直線移動
    set_MoveBehaviour(MOVE_BEHAVIOUR_TYPE::STRAIGHT);
}

//*---------------------------------------------------------------------------------------
//*【?】更新
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//*
//* [返値] なし
//*----------------------------------------------------------------------------------------
void MoveLogic::Calculate(const MoveParam& _param)
{
    float deltaTime = Master::m_pTimeManager->get_DeltaTime();

    // 移動挙動クラスがセットされていれば、移動計算をする
    if (auto pTransform = m_pOwner.lock()->get_Transform().lock())
    {
        if (m_pMoveBehaviour)
        {
            ResultMove res;

            // 移動計算を呼び出す
            res = m_pMoveBehaviour->MoveCalculate(deltaTime, _param, *pTransform);

            // 移動ベクトルと回転ベクトルをもとに、新しい位置と回転を計算する
            VEC3 crntPos = pTransform->get_VEC3ToPos();
            VEC3 newPos = crntPos + (res._moveVelocity * deltaTime);

            // 反映
            pTransform->set_Pos(newPos);
        }
    }
}

//*---------------------------------------------------------------------------------------
//*【?】移動挙動の設定
//*
//* [引数]
//* _type : 移動挙動の種類
//* 
//* [返値] なし
//*----------------------------------------------------------------------------------------
void MoveLogic::set_MoveBehaviour(MOVE_BEHAVIOUR_TYPE _type)
{
    switch (_type)
    {
    case MOVE_BEHAVIOUR_TYPE::STRAIGHT:
        break;
    case MOVE_BEHAVIOUR_TYPE::HOMING:
        break;
    default:
        m_pMoveBehaviour = nullptr;
        break;
    }
}

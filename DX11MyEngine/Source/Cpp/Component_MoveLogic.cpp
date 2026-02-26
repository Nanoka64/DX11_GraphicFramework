#include "pch.h"
#include "Component_MoveLogic.h"
#include "IMoveBehaviour.h"
#include "LinearMove_Behaviour.h"
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
	m_pMoveBehaviour = nullptr;
    m_pMoveBehaviourMap.clear();
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
    Register(MOVE_BEHAVIOUR_TYPE::LINEAR);
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
        if (m_pMoveBehaviour != nullptr)
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
//*【?】移動挙動の登録
//*
//* [引数]
//* _type : 移動挙動の種類
//* 
//* [返値] なし
//*----------------------------------------------------------------------------------------
void MoveLogic::Register(MOVE_BEHAVIOUR_TYPE _type)
{
    switch (_type)
    {
    case MOVE_BEHAVIOUR_TYPE::LINEAR:
        m_pMoveBehaviourMap[_type] = std::make_unique<LinearMove_Behaviour>();
        break;
    case MOVE_BEHAVIOUR_TYPE::HOMING:
        break;
    default:
        break;
    }
}

//*---------------------------------------------------------------------------------------
//*【?】移動挙動の変更
//*
//* [引数]
//* _type : 変更する移動挙動の種類（事前に登録されていなかった場合、登録処理を行うため、newが走る）
//* 
//* [返値] なし
//*----------------------------------------------------------------------------------------
void MoveLogic::ChangeBehaviour(MOVE_BEHAVIOUR_TYPE _type)
{
    // もし登録されていなければ登録する
    auto it = m_pMoveBehaviourMap.find(_type);
    if (it == m_pMoveBehaviourMap.end())
    {
        Register(_type);
    }

    m_pMoveBehaviour = m_pMoveBehaviourMap[_type].get();
}

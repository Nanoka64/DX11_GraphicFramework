#include "pch.h"
#include "Component_MoveLogic.h"
#include "IMoveBehaviour.h"
#include "LinearMove_Behaviour.h"
#include "HormingMove_Behaviour.h"
#include "GameObject.h"

using namespace GIGA_Engine;
using namespace VECTOR3;
using namespace UtilityData;

//*---------------------------------------------------------------------------------------
//*【?】コンストラクタ
//* [引数]
//* pOwner : オーナーオブジェクト
//* updateRank : 更新レイヤー
//*----------------------------------------------------------------------------------------
MoveLogic::MoveLogic(std::weak_ptr<GameObject> pOwner, int updateRank)
    :IComponent(pOwner, updateRank),
    m_pMoveBehaviour(nullptr),
    m_GravityVelocity(0.0f),
    m_CrntMoveVelocity(VEC3())
{
    this->set_Tag("MoveLogic");
}

//*---------------------------------------------------------------------------------------
//*【?】デストラクタ
//*----------------------------------------------------------------------------------------
MoveLogic::~MoveLogic()
{
	m_pMoveBehaviour = nullptr;
}
//*---------------------------------------------------------------------------------------
//*【?】初期化
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//*
//* [返値] なし
//*----------------------------------------------------------------------------------------
void MoveLogic::Start(RendererEngine &renderer)
{
    // デフォルトは直線移動
    ChangeBehaviour(MOVE_BEHAVIOUR_TYPE::LINEAR);
}

//*---------------------------------------------------------------------------------------
//*【?】更新
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//*
//* [返値] なし
//*----------------------------------------------------------------------------------------
void MoveLogic::Update(RendererEngine &renderer)
{
    Calculate(m_MoveParam);
}

//*---------------------------------------------------------------------------------------
//*【?】更新
//*
//* [引数]
//* &_param : 移動計算に必要なパラメータ
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

            // 加速度
            //float accelerationSpeed = _param._acceleration * deltaTime;

            // 目標速度に近づける
            //m_CrntMoveVelocity = VEC3::Lerp(m_CrntMoveVelocity, res._moveVelocity, accelerationSpeed);

            // 移動ベクトルと回転ベクトルをもとに、新しい位置と回転を計算する
            VEC3 crntPos = pTransform->get_VEC3ToPos();
            VEC3 newPos = crntPos + (res._moveVelocity * deltaTime);

            //// 重力があるなら、重力処理を行う
            //if (_param._gravity > 0.0f)
            //{
            //    m_GravityVelocity -= _param._gravity * deltaTime;

            //    newPos.y += m_GravityVelocity * deltaTime;
            //    if (newPos.y < -100.0f)
            //    {
            //        newPos.y = 0.0f;
            //        m_GravityVelocity = 0.0f;
            //    }
            //}

            // 反映
            pTransform->set_Pos(newPos);
            pTransform->set_RotationQuaternion(res._RotQ);
        }
    }
}


//*---------------------------------------------------------------------------------------
//*【?】パラメータのリセット
//*
//* [引数] なし
//* [返値] なし
//*----------------------------------------------------------------------------------------
void MoveLogic::ParamReset()
{
    m_GravityVelocity = 0.0f;
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
    int index = static_cast<int>(_type);

    switch (_type)
    {
    case MOVE_BEHAVIOUR_TYPE::LINEAR:
        m_pMoveBehaviourMap[index] = std::make_unique<LinearMove_Behaviour>();
        break;
    case MOVE_BEHAVIOUR_TYPE::HOMING:
        m_pMoveBehaviourMap[index] = std::make_unique<HormingMove_Behaviour>();
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
	// NONEが指定された場合は、移動挙動を解除する
    if (_type == MOVE_BEHAVIOUR_TYPE::NONE)
    {
        m_pMoveBehaviour = nullptr;
		return;
    }

    // もし登録されていなければ登録する
    int index = static_cast<int>(_type);
    if (m_pMoveBehaviourMap[index] == nullptr)
    {
        Register(_type);
    }

    m_pMoveBehaviour = m_pMoveBehaviourMap[index].get();
}

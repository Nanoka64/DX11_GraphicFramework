#include "pch.h"
#include "Component_Health.h"
#include "RendererEngine.h"
#include "GameObject.h"

using namespace GIGA_Engine;
using namespace VECTOR3;

//*---------------------------------------------------------------------------------------
//*【?】コンストラクタ
//* [引数]
//* pOwner : オーナーオブジェクト
//* updateRank : 更新レイヤー
//*----------------------------------------------------------------------------------------
Health::Health(std::weak_ptr<GameObject> pOwner, int updateRank)
    :IComponent(pOwner, updateRank),
    m_CrntHP(0.0f),
    m_MaxHP(0.0f),
    m_IsDead(false)
{
    this->set_Tag("Health");
}

//*---------------------------------------------------------------------------------------
//*【?】デストラクタ
//*----------------------------------------------------------------------------------------
Health::~Health()
{

}

//*---------------------------------------------------------------------------------------
//*【?】初期化
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//* [返値]なし
//*----------------------------------------------------------------------------------------
void Health::Start(RendererEngine& renderer)
{
}


//*---------------------------------------------------------------------------------------
//*【?】更新
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//* [返値]なし
//*----------------------------------------------------------------------------------------
void Health::Update(RendererEngine& renderer)
{

}


void Health::TakeDamage(const float _dmg)
{
    if (m_IsDead){
        return;
    }

    m_CrntHP -= _dmg;
    
    // ダメージ処理を行う
    for (auto& callback : m_DamageTaskArray){
        callback(_dmg);
    }

    // 死亡
    if (m_CrntHP <= 0.0f){
        m_CrntHP = 0.0f;
        m_IsDead = true;


        // ダメージ処理を行う
        for (auto& callback : m_DeathTaskArray) {
            callback();
        }
    }
}


//*---------------------------------------------------------------------------------------
//*【?】死んだ際のコールバックを登録
//*
//* [引数]
//* _callback : 引数なし関数
//*
//* [返値]
//* なし 
//*----------------------------------------------------------------------------------------
void Health::RegisterOnDead(std::function<void()> _callback)
{
    m_DeathTaskArray.push_back(_callback);
}


//*---------------------------------------------------------------------------------------
//*【?】ダメージを受けた際のコールバックを登録
//*
//* [引数]
//* _callback : 引数にfloat（ダメージ値）指定
//*
//* [返値]
//* なし 
//*----------------------------------------------------------------------------------------
void Health::RegisterOnDamage(std::function<void(float)> _callback)
{
    m_DamageTaskArray.push_back(_callback);
}


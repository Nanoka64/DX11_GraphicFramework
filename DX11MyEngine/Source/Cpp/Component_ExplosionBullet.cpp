#include "pch.h"
#include "Component_ExplosionBullet.h"
#include "RendererEngine.h"


using namespace VECTOR3;

//*---------------------------------------------------------------------------------------
//*【?】コンストラクタ
//* [引数]
//* pOwner : オーナーオブジェクト
//* updateRank : 更新レイヤー
//*----------------------------------------------------------------------------------------
ExplosionBullet::ExplosionBullet(std::weak_ptr<GameObject> pOwner, int updateRank) : 
    BulletBase(pOwner, updateRank)
{
	this->set_Tag("ExplosionBullet");
}


//*---------------------------------------------------------------------------------------
//*【?】デストラクタ
//*----------------------------------------------------------------------------------------
ExplosionBullet::~ExplosionBullet()
{

}


//*---------------------------------------------------------------------------------------
//*【?】初期化
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//* [返値]なし
//*----------------------------------------------------------------------------------------
void ExplosionBullet::Start(RendererEngine &renderer)
{

}


//*---------------------------------------------------------------------------------------
//*【?】更新
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//* [返値]なし
//*----------------------------------------------------------------------------------------
void ExplosionBullet::Update(RendererEngine &renderer)
{

}

void ExplosionBullet::OnCollisionEnter(const class CollisionInfo &other)
{
    if (m_CollisionTask)
    {
        m_CollisionTask(other);
    }

    m_pOwner.lock()->clear_StatusFlag(OBJECT_STATUS_BITFLAG::IS_ACTIVE);
}


//*---------------------------------------------------------------------------------------
//*【?】状態のリセット
//* [引数]なし
//* [返値]なし
//*----------------------------------------------------------------------------------------
void ExplosionBullet::Reset()
{
    m_Parameter.Reset();
}
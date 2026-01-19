#include "pch.h"
#include "Component_Bullet.h"
#include "pch.h"
#include "Component_AssultRifle.h"
#include "Component_Transform.h"
#include "RendererEngine.h"
#include "GameObjectManager.h"
#include "GameObject.h"
#include "InputFactory.h"
#include "MeshFactory.h"
#include "ResourceManager.h"

using namespace GIGA_Engine;
using namespace Input;
using namespace VECTOR3;

//*---------------------------------------------------------------------------------------
//*【?】コンストラクタ
//* [引数]
//* pOwner : オーナーオブジェクト
//* updateRank : 更新レイヤー
//*----------------------------------------------------------------------------------------
Bullet::Bullet(std::weak_ptr<GameObject> pOwner, int updateRank) : IComponent(pOwner, updateRank)
{
    this->set_Tag("Bullet");
}



//*---------------------------------------------------------------------------------------
//*【?】デストラクタ
//*----------------------------------------------------------------------------------------
Bullet::~Bullet()
{

}


//*---------------------------------------------------------------------------------------
//*【?】初期化
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//* [返値]なし
//*----------------------------------------------------------------------------------------
void Bullet::Init(RendererEngine &renderer)
{

}


//*---------------------------------------------------------------------------------------
//*【?】更新
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//* [返値]なし
//*----------------------------------------------------------------------------------------
void Bullet::Update(RendererEngine &renderer)
{
    auto transform = m_pOwner.lock()->get_Transform().lock();
    VEC3 crntPos = transform->get_VEC3ToPos();

    VEC3 moveVelocity = VEC3(0.0f,0.0f,0.0f);
    moveVelocity = transform->get_Forward();
    crntPos += moveVelocity.Normalize();

    transform->set_Pos(crntPos);
}


//*---------------------------------------------------------------------------------------
//*【?】描画
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//* [返値]なし
//*----------------------------------------------------------------------------------------
void Bullet::Draw(RendererEngine &renderer)
{

}


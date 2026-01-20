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
    // 開始位置
    m_StartPos = m_pOwner.lock()->get_Transform().lock()->get_VEC3ToPos();
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
    VEC3 moveVelocity = VEC3(0.0f,0.0f,0.0f);   // 速度ベクトル

    VEC3 forward = transform->get_Forward();    // 前方向ベクトル

    forward = forward.Normalize();
    moveVelocity -= forward;
    crntPos += moveVelocity.Normalize() * 20.0f;

    transform->set_Pos(crntPos);

    if (VEC3::Distance(crntPos, m_StartPos) > 2000.0f)
    {
        m_pOwner.lock()->set_StatusFlag(OBJECT_STATUS_BITFLAG::IS_DELETE);
    }
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


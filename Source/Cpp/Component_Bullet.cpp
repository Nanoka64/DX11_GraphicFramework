#include "pch.h"
#include "Component_Bullet.h"
#include "pch.h"
#include "Component_AssultRifle.h"
#include "Component_Transform.h"
#include "Component_DecalRenderer.h"
#include "RendererEngine.h"
#include "GameObjectManager.h"
#include "GameObject.h"
#include "InputFactory.h"
#include "MeshFactory.h"
#include "CollisionInfo.h"
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
void Bullet::Start(RendererEngine &renderer)
{
    auto transform = m_pOwner.lock()->get_Transform().lock();

    // 開始位置
    m_StartPos = transform->get_VEC3ToPos();

    m_MoveVelocity = transform->get_Forward(); // 前方向ベクトル

    m_MoveVelocity.x += Tool::RandRange(-0.03f, 0.03f);
    m_MoveVelocity.y += Tool::RandRange(-0.03f, 0.03f);
    m_MoveVelocity.z += Tool::RandRange(-0.03f, 0.03f);

    m_MoveVelocity = m_MoveVelocity.Normalize();


    m_CollisionTask =
        [this, &renderer](const class CollisionInfo& _other)
        {
            auto matPtr = Master::m_pResourceManager->FindMaterial("Decal_BulletHole");

            SetupMaterialInfo matInfo[1];
            matInfo[0].Index = 0;
            matInfo[0].pMaterialData = matPtr;

            CreateDecalInfo decal;
            decal.pRenderer = &renderer;
            decal.Type = UTILITY_MESH_TYPE::CUBU;
            decal.MatNum = 1;
            decal.MaterialData = matInfo;
            decal.IsActive = false;
            decal.ShaderType = SHADER_TYPE::DEFERRED_STD_DECAL;
            decal.IsNormalMap = false;
            decal.IsDynamic = true;

            auto transform = m_pOwner.lock()->get_Transform().lock();
            VEC3 pos = transform->get_VEC3ToPos();

            VEC3 hitNormal = _other.get_HitNormal();    // 衝突相手の法線


            // 水平方向の向きを求める
            float angleY = atan2(hitNormal.x, hitNormal.z);
            // 水平成分の長さ
            float xzLen = sqrtf(hitNormal.x * hitNormal.x + hitNormal.z * hitNormal.z);
            // 垂直方向の角度を求める
            // 法線の逆を向かせたいのでマイナスを付ける
            float angleX = atan2(-hitNormal.y, xzLen);

            // 法線と弾の移動ベクトルの内積
            float dot_HitNormToMoveVel = abs(VEC3::Dot(hitNormal, m_MoveVelocity));

            // 地面を這うベクトルを求める
            VEC3 tempVec = m_MoveVelocity - (hitNormal * dot_HitNormToMoveVel);
            tempVec = tempVec.Normalize();

            VEC3 side = VEC3::Cross(VEC3(0.0f, 1.0f, 0.0f), hitNormal).Normalize();
            VEC3 up = VEC3::Cross(hitNormal, side);

            float dotSide = VEC3::Dot(tempVec, side);
            float dotUp = VEC3::Dot(tempVec, up);

            float angleZ = atan2f(dotSide, dotUp);


            VEC3 scale;
            scale.x = 30.0f;
            scale.y = 30.0f + (1.0f - dot_HitNormToMoveVel) * 150.0f;
            scale.z = 10.0f;

            auto obj = MeshFactory::CreateDecal(decal);
            obj->get_Component<DecalRenderer>()->Start(renderer);
            obj->get_Transform().lock()->set_Pos(pos);
            obj->get_Transform().lock()->set_Scale(scale);
            obj->get_Transform().lock()->set_RotateToRad(angleX, angleY, angleZ);
            obj->set_Tag("BulletHole");
        };
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
    
    crntPos -= m_MoveVelocity * 20.0f;

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


void Bullet::OnCollisionEnter(const class CollisionInfo &other)
{
    m_CollisionTask(other);
    m_pOwner.lock()->set_StatusFlag(OBJECT_STATUS_BITFLAG::IS_DELETE);
}
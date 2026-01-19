#include "pch.h"
#include "Component_AssultRifle.h"
#include "Component_Transform.h"
#include "Component_Bullet.h"
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
AssultRifle::AssultRifle(std::weak_ptr<GameObject> pOwner, int updateRank) : IComponent(pOwner, updateRank)
{
	this->set_Tag("AssultRifle");
}



//*---------------------------------------------------------------------------------------
//*【?】デストラクタ
//*----------------------------------------------------------------------------------------
AssultRifle::~AssultRifle()
{

}


//*---------------------------------------------------------------------------------------
//*【?】初期化
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//* [返値]なし
//*----------------------------------------------------------------------------------------
void AssultRifle::Init(RendererEngine &renderer)
{

}


//*---------------------------------------------------------------------------------------
//*【?】更新
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//* [返値]なし
//*----------------------------------------------------------------------------------------
void AssultRifle::Update(RendererEngine &renderer)
{
	auto transform = m_pOwner.lock()->get_Transform().lock();

	if (GetInputHoldRepeat(CONFIG_INPUT::C,10,20))
	{
        // マテリアル取得
        auto matPtr = Master::m_pResourceManager->LoadMaterial("Wall");
        SetupMaterialInfo matInfo[1];
        matInfo[0].Index = 0;
        matInfo[0].pMaterialData = matPtr;
        CreateUtilityMeshInfo mesh;
        mesh.pRenderer = &renderer;
        mesh.Type = UTILITY_MESH_TYPE::SPHERE;
        mesh.MatNum = 1;
        mesh.MaterialData = matInfo;
        mesh.IsActive = true;
        mesh.ShaderType = SHADER_TYPE::DEFERRED_STD_STATIC_N;
        mesh.IsNormalMap = true;
        mesh.ObjTag = "Bullet";
        auto obj = MeshFactory::CreateUtilityMesh(mesh);

        obj->add_Component<Bullet>();
        obj->get_Component<Bullet>()->Init(renderer);

        // 親の向きと位置を参照
        obj->get_Transform().lock()->set_Pos(transform->get_Parent().lock()->get_VEC3ToPos());
        obj->get_Transform().lock()->set_RotateToRad(transform->get_Parent().lock()->get_VEC3ToRotateToRad());
        obj->get_Transform().lock()->set_Scale(VEC3(10.0f, 10.0f, 50.0f));
	}
}


//*---------------------------------------------------------------------------------------
//*【?】描画
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//* [返値]なし
//*----------------------------------------------------------------------------------------
void AssultRifle::Draw(RendererEngine &renderer)
{

}

//*---------------------------------------------------------------------------------------
//*【?】弾になるオブジェクトの設定
//*
//* [引数]
//* pObj : 弾オブジェクト
//* [返値]なし
//*----------------------------------------------------------------------------------------
void AssultRifle::set_BulletObject(std::shared_ptr<GameObject> pObj)
{
	m_pBulletObject = pObj;
}


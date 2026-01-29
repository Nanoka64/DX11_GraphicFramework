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
#include "Component_BoxCollider.h"
#include "Component_TrailRenderer.h"

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
void AssultRifle::Start(RendererEngine &renderer)
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

	//if (GetInputHoldRepeat(CONFIG_INPUT::C,2,2))
	if(GetMouseClick(MOUSE_BUTTON_STATE::LEFT))
    {
        // マテリアル取得
        auto matPtr1 = Master::m_pResourceManager->FindMaterial("Bullet");

        SetupMaterialInfo matInfo[1];
        matInfo[0].Index = 0;
        matInfo[0].pMaterialData = matPtr1; // 頭

        CreateModelInfo model;
        model.pRenderer = &renderer;
        model.Path = "Resource/Model/Weapon/bullet.fbx";
        model.ObjTag = "Bullet";
        model.IsAnim = false;
        model.MatNum = 1;
        model.SetupMaterial = matInfo;
        model.ShaderType = SHADER_TYPE::DEFERRED_STD_STATIC;
        auto obj = MeshFactory::CreateModel(model);
        if (obj == nullptr)
        {
            assert(false);
            return;
        }

        // バレットコンポーネントの追加
        auto bulletComp = obj->add_Component<Bullet>();
        auto bullet_transform = obj->get_Transform().lock();

        VEC3 pos = transform->get_WorldVEC3ToPos();
        VEC3 rad = transform->get_Parent().lock()->get_VEC3ToRotateToRad();
        
        // 親の向きと位置を参照
        bullet_transform->set_Pos(pos);
        bullet_transform->set_RotateToRad(rad);
        bullet_transform->set_Scale(VEC3(0.01f, 0.01f, 0.01f));

        // コライダーの追加
        auto collider = obj->add_Component<BoxCollider>();
        collider->set_Size(VEC3(2.0f, 2.0f, 2.0f));
        collider->set_Center(VEC3(0.0f, 2.0f, 0.0f));

        //auto trail = obj->add_Component<TrailRenderer>();
        //trail->set_Width(1.0f);
        //trail->set_MinVertexDistance(10.0f);

        // コライダーの登録
        Master::m_pCollisionManager->RegisterCollider(collider);


        // 初期化
        bulletComp->Start(renderer);
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


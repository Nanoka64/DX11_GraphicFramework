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
#include "Component_3DCamera.h"
#include "Component_LineRenderer.h"
#include "Component_PointLight.h"

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
    // 照準レーザー用
    m_pLineRendererComp = m_pOwner.lock()->get_Component<LineRenderer>();
    m_pFlashPointLight = m_pOwner.lock()->get_Component<PointLight>();
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
    float c_AngleH = renderer.get_CameraComponent()->get_Angle_H();
    float c_AngleV = renderer.get_CameraComponent()->get_Angle_V();

	auto transform = m_pOwner.lock()->get_Transform().lock();
    VEC3 pos = transform->get_WorldVEC3ToPos();

    // 武器を回転させる
    // 水平方向はプレイヤーに合わせているので垂直方向のみ、カメラの回転を使う。
    transform->set_RotateToRad(VEC3(c_AngleV * -1,0.0f, 0.0f));


    // ワールド変換行列から方向をとる
    XMMATRIX worldMtx = transform->get_WorldMtx();
    XMVECTOR forward =  worldMtx.r[2];  // Z
    forward *= -1;  // プレイヤーが-Z前になってしまっているので

    // 弾を発射してないときはフラッシュライトをオフ
    m_pFlashPointLight.lock()->set_Intensity(0.0f);

    // レーザーサイトの始点と方向
    m_pLineRendererComp.lock()->set_Dir(VEC3::FromXMVECTOR(XMVector3Normalize(forward)));
    m_pLineRendererComp.lock()->set_StartPos(pos);

    // 右クリックでズーム
    renderer.get_CameraComponent()->set_Fov(90.0f);
    if (GetMouseClick(MOUSE_BUTTON_STATE::RIGHT))
    {
        renderer.get_CameraComponent()->set_Fov(40.0f);
    }
    // 左クリックで発射
	if(GetMouseClickHoldRepeat(MOUSE_BUTTON_STATE::LEFT, 4, 4))
    {
        // ****************************************************
        //				 発射音再生
        // ****************************************************
        Master::m_pSoundManager->Play_RandPitch(SOUND_TYPE::SE, SOUND_ID_TO_INT(SOUND_ID::GUN_FIRE02), 300);

        // フラッシュライト
        m_pFlashPointLight.lock()->set_Range(30.0f);
        m_pFlashPointLight.lock()->set_Intensity(1.5f);
        m_pFlashPointLight.lock()->set_LightColor(VEC3(1.0f, 1.0f, 1.0f));

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
        

        VEC3 rad;
        rad.x = (c_AngleV) * -1;
        rad.y = (c_AngleH - 1.57f) * -1;
        rad.z = 0.0f;

        // 親の向きと位置を参照
        bullet_transform->set_Pos(pos);
        bullet_transform->set_RotateToRad(rad);
        bullet_transform->set_Scale(VEC3(0.01f, 0.01f, 0.01f));

        // コライダーの追加
        auto collider = obj->add_Component<BoxCollider>();
        collider->set_Size(VEC3(5.0f, 5.0f, 5.0f));
        collider->set_Center(VEC3(0.0f, 2.0f, 0.0f));

        // 軌跡
        auto trail = obj->add_Component<TrailRenderer>();
        trail->set_Width(2.0f);
        trail->set_MinVertexDistance(5.0f);
        trail->set_DrawTime(5);
        trail->set_EmissivePower(10.0f);
        trail->set_Color(VECTOR4::VEC4(0.0f, 1.0f, 0.0f, 1.0f));


        auto lig = obj->add_Component<PointLight>();
        lig->set_LightColor(VEC3(0.0f, 0.5f, 1.0f));
        lig->set_Intensity(10.0f);

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


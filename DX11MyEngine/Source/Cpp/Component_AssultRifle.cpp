#include "pch.h"
#include "Component_AssultRifle.h"
#include "Component_Transform.h"
#include "Component_NormalBullet.h"
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
#include "GameManager.h"

using namespace GIGA_Engine;
using namespace Input;
using namespace VECTOR3;
using namespace BulletData;

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
	if(GetMouseClickHoldRepeat(MOUSE_BUTTON_STATE::LEFT, 5, 5))
    {
        // ****************************************************
        //				 発射音再生
        // ****************************************************
        Master::m_pSoundManager->Play_RandPitch(SOUND_TYPE::SE, SOUND_ID_TO_INT(SOUND_ID::GUN_FIRE02), 300);


        // 親の向き等を参照
        VEC3 rad;
        rad.x = (c_AngleV) * -1;
        rad.y = (c_AngleH - 1.57f) * -1;
        rad.z = 0.0f;

        // トランスフォームパラメータ
        BulletTransformData bulletTransform;
        bulletTransform._pos = pos;
        bulletTransform._rotRad = rad;
        bulletTransform._scale = VEC3(0.01f, 0.01f, 0.01f);

        // 弾自身のパラメータ
        //NormalBulletData param;
        //param._range = 2000.0f;
        //param._speed = 400.0f;

        ExplosionBulletData param;
        param._range = 2000.0f;
        param._speed = 300.0f;
        param._explosionRadius = 50.0f;
        param._explosionEffectHandleTag = "Explosion_01";

        GameManager::get_BulletManager()->Shot(renderer, bulletTransform, param);

        // フラッシュ
        m_pFlashPointLight.lock()->set_Range(30.0f);
        m_pFlashPointLight.lock()->set_Intensity(1.5f);
        m_pFlashPointLight.lock()->set_LightColor(VEC3(1.0f, 1.0f, 1.0f));
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


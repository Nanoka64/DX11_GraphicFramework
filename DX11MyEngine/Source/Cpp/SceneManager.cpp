#include "pch.h"
#include "SceneManager.h"
#include "SceneStateEnums.h"
#include "RendererEngine.h"
#include "DirectWriteManager.h"
#include "ResourceManager.h"
#include "GameObjectManager.h"
#include "GameManager.h"
#include "InputFactory.h"
#include "SceneFactory.h"
#include "MeshFactory.h"
#include "Component_3DCamera.h"
#include "Component_PlayerController.h"
#include "Component_SkinnedMeshAnimator.h"
#include "Component_ModelMeshResource.h"
#include "Component_ModelMeshRenderer.h"
#include "Component_MeshRenderer.h"
#include "Component_DirectionalLight.h"
#include "Component_PointLight.h"
#include "Component_SpriteRenderer.h"
#include "Component_BillboardRenderer.h"
#include "Component_SkyRenderer.h"
#include "Component_AssultRifle.h"
#include "Component_BoxCollider.h"
#include "Component_SphereCollider.h"
#include "Component_TrailRenderer.h"
#include "Component_DecalRenderer.h"


using namespace VECTOR4;
using namespace VECTOR3;
using namespace VECTOR2;
using namespace Tool::UV;
using namespace Tool;
using namespace Input;

using namespace RenderData;
using namespace GIGA_Engine;

using namespace SceneStateEnums;

//*---------------------------------------------------------------------------------------
//* @:SceneManager Class 
//*【?】コンストラクタ
//* 引数：なし
//*----------------------------------------------------------------------------------------
SceneManager::SceneManager():
    m_pPlayer(),
    m_StateMachine(this),
    m_CrntSceneState(0),
    m_IsClose(false)
{

}


//*---------------------------------------------------------------------------------------
//* @:SceneManager Class 
//*【?】デストラクタ
//* 引数：なし
//*----------------------------------------------------------------------------------------
SceneManager::~SceneManager()
{
}


//*---------------------------------------------------------------------------------------
//* @:SceneManager Class 
//*【?】初期化
//* 引数：1.RendererEngine
//* 返値：成功したか
//*----------------------------------------------------------------------------------------
bool SceneManager::Init(RendererEngine &renderer)
{

    // パイプラインの作成
    if (!renderer.CreateRendererPipeline(RENDER_PIPELINE_STATE::DEFAULT))
    {
        return false;
    }

    // ステートマシンの作成
    SceneFactory::Create(m_StateMachine, SCENE_STATE::SCENE_STATE_TITLE, renderer);
    SceneFactory::Create(m_StateMachine, SCENE_STATE::SCENE_STATE_GAME, renderer);
    SceneFactory::Create(m_StateMachine, SCENE_STATE::SCENE_STATE_RESULT, renderer);
    m_StateMachine.SetStartState(SCENE_STATE::SCENE_STATE_TITLE);

    return true;
}


//*---------------------------------------------------------------------------------------
//* @:SceneManager Class 
//*【?】更新
//* 引数：1.RendererEngine
//* 返値：void
//*----------------------------------------------------------------------------------------
void SceneManager::Update(RendererEngine& renderer)
{
    static float counter = 0.0f;
    counter += 0.01f;

    // シーンステートの実行
    m_StateMachine.Update();

    // オブジェクト更新
    Master::m_pGameObjectManager->ObjectUpdate(renderer);

    // 衝突判定
    Master::m_pCollisionManager->CollisionProcess();

    // 遅延更新
    Master::m_pGameObjectManager->ObjectLateUpdate(renderer);

}


//*---------------------------------------------------------------------------------------
//* @:SceneManager Class 
//*【?】描画
//* 引数：1.RendererEngine
//* 返値：void
//*----------------------------------------------------------------------------------------
void SceneManager::Draw(RendererEngine& renderer)
{
    // レンダリングパイプラインの実行
    if (auto camera = Master::m_pDataManager->get_CameraComponent().lock()) {
        renderer.ExecuteDefaultRendererPipeline(RENDER_PIPELINE_STATE::DEFAULT, camera.get());
    }

    // シーンステートの描画
    m_StateMachine.Draw();

    //Master::m_pDirectWriteManager->DrawString("こんにちは",VEC2(940, 540));
}


//*---------------------------------------------------------------------------------------
//* @:SceneManager Class 
//*【?】終了
//* 引数：1.RendererEngine
//* 返値：void
//*----------------------------------------------------------------------------------------
void SceneManager::Term(RendererEngine &renderer)
{
}


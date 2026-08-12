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


using namespace VECTOR4;
using namespace VECTOR3;
using namespace VECTOR2;
using namespace Tool::UV;
using namespace Tool;
using namespace Input;

using namespace GIGA_Engine;

using namespace SceneStateEnums;

//*---------------------------------------------------------------------------------------
//* @:SceneManager Class 
//*【?】コンストラクタ
//* 引数：なし
//*----------------------------------------------------------------------------------------
SceneManager::SceneManager():
    m_StateMachine(this),
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
    // シーンステートの実行
    m_StateMachine.Update();
}


//*---------------------------------------------------------------------------------------
//* @:SceneManager Class 
//*【?】描画
//* 引数：1.RendererEngine
//* 返値：void
//*----------------------------------------------------------------------------------------
void SceneManager::Draw(RendererEngine& renderer)
{
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


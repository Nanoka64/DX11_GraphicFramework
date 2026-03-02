#include "pch.h"
#include "GameManager.h"
#include "SceneManager.h"
#include "RendererEngine.h"
#include "DirectWriteManager.h"


BulletManager *GameManager::m_pBulletManager = nullptr;  // 弾管理クラス


//*---------------------------------------------------------------------------------------
//* @:GameManager Class 
//*【?】コンストラクタ
//* 引数：なし
//*----------------------------------------------------------------------------------------
GameManager::GameManager() :
	m_pSceneManager(nullptr),
	m_IsClose(false)
{
}


//*---------------------------------------------------------------------------------------
//* @:GameManager Class 
//*【?】デストラクタ
//* 引数：なし
//*----------------------------------------------------------------------------------------
GameManager::~GameManager()
{

}


//*---------------------------------------------------------------------------------------
//* @:GameManager Class 
//*【?】
//* 引数：1.RendererEngine
//* 返値：bool
//*----------------------------------------------------------------------------------------
bool GameManager::Init(RendererEngine& renderer)
{
	// シーン管理クラスの生成
	m_pSceneManager = new SceneManager();

	if (!m_pSceneManager->Init(renderer))
	{
		return false;
	}

	// 弾管理クラスの生成
	GameManager::m_pBulletManager = new BulletManager();

	return true;
}


//*---------------------------------------------------------------------------------------
//* @:GameManager Class 
//*【?】更新
//* 引数：1.RendererEngine
//* 返値：void
//*----------------------------------------------------------------------------------------
void GameManager::Update(RendererEngine& renderer)
{
	// シーンの更新
	m_pSceneManager->Update(renderer);

	// 弾の更新
	m_pBulletManager->Update(renderer);

	// シーンが終了していたら終わらせる
	if (m_pSceneManager->get_IsSceneClose())
	{
		m_IsClose = true;
	}
}


//*---------------------------------------------------------------------------------------
//* @:GameManager Class 
//*【?】描画
//* 引数：1.RendererEngine
//* 返値：void
//*----------------------------------------------------------------------------------------
void GameManager::Draw(RendererEngine& renderer)
{
	m_pSceneManager->Draw(renderer);
}



//*---------------------------------------------------------------------------------------
//* @:GameManager Class 
//*【?】終了
//* 引数：1.RendererEngine
//* 返値：void
//*----------------------------------------------------------------------------------------
void GameManager::Term(RendererEngine &renderer)
{
	m_pSceneManager->Term(renderer);
	delete m_pSceneManager;
}


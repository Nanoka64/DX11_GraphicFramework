#include "pch.h"
#include "GameManager.h"
#include "SceneManager.h"
#include "RendererEngine.h"
#include "DirectWriteManager.h"

//*---------------------------------------------------------------------------------------
//* @:GameManager Class 
//*y?zƒRƒ“ƒXƒgƒ‰ƒNƒ^
//* ˆø”F‚È‚µ
//*----------------------------------------------------------------------------------------
GameManager::GameManager() :
	m_pSceneManager(nullptr)
{
}


//*---------------------------------------------------------------------------------------
//* @:GameManager Class 
//*y?zƒfƒXƒgƒ‰ƒNƒ^
//* ˆø”F‚È‚µ
//*----------------------------------------------------------------------------------------
GameManager::~GameManager()
{

}


//*---------------------------------------------------------------------------------------
//* @:GameManager Class 
//*y?z
//* ˆø”F1.RendererEngine
//* •Ô’lFbool
//*----------------------------------------------------------------------------------------
bool GameManager::Init(RendererEngine& renderer)
{
	m_pSceneManager = new SceneManager();

	if (!m_pSceneManager->Init(renderer))
	{
		return false;
	}

	return true;
}


//*---------------------------------------------------------------------------------------
//* @:GameManager Class 
//*y?zXV
//* ˆø”F1.RendererEngine
//* •Ô’lFvoid
//*----------------------------------------------------------------------------------------
void GameManager::Update(RendererEngine& renderer)
{
	m_pSceneManager->Update(renderer);
}


//*---------------------------------------------------------------------------------------
//* @:GameManager Class 
//*y?z•`‰æ
//* ˆø”F1.RendererEngine
//* •Ô’lFvoid
//*----------------------------------------------------------------------------------------
void GameManager::Draw(RendererEngine& renderer)
{
	m_pSceneManager->Draw(renderer);
}



//*---------------------------------------------------------------------------------------
//* @:GameManager Class 
//*y?zI—¹
//* ˆø”F1.RendererEngine
//* •Ô’lFvoid
//*----------------------------------------------------------------------------------------
void GameManager::Term(RendererEngine &renderer)
{
	m_pSceneManager->Term(renderer);
	delete m_pSceneManager;
}


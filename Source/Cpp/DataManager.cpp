#include "pch.h"
#include "DataManager.h"
#include "RendererEngine.h"
#include "ResourceManager.h"
#include "SceneStateEnums.h"
#include "Component_SpriteRenderer.h"
#include "Component_SkinnedMeshAnimator.h"
#include "Component_PlayerController.h"
#include "Component_BoxCollider.h"
#include "Component_3DCamera.h"
#include "Component_Transform.h"
#include "Component_TrailRenderer.h"
#include "Component_LineRenderer.h"
#include "Component_DirectionalLight.h"
#include "Component_AssultRifle.h"
#include "Component_Health.h"
#include "Component_PointLight.h"
#include "GameObject.h"
#include "MeshFactory.h"
#include "InputFactory.h"

//*---------------------------------------------------------------------------------------
//*【?】コンストラクタ
//*----------------------------------------------------------------------------------------
DataManager::DataManager() 
{
}

//*---------------------------------------------------------------------------------------
//*【?】デストラクタ
//*----------------------------------------------------------------------------------------
DataManager::~DataManager()
{
}

//*---------------------------------------------------------------------------------------
//*【?】初期化
//*
//* [引数]なし
//* [返値]
//* true  : 成功 
//* false : 失敗 
//*----------------------------------------------------------------------------------------
bool DataManager::Init()
{
	return true;
}

//*---------------------------------------------------------------------------------------
//*【?】タイトルロードの開始時にセットするデータ
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//* [返値]
//* true  : 成功 
//* false : 失敗 
//*----------------------------------------------------------------------------------------
bool DataManager::SettingsData_TitleLoadOnEnter(RendererEngine& renderer)
{
	return true;
}

//*---------------------------------------------------------------------------------------
//*【?】タイトルロードの終了時にセットするデータ
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//* [返値]
//* true  : 成功 
//* false : 失敗 
//*----------------------------------------------------------------------------------------
bool DataManager::SettingsData_TitleLoadOnExit(RendererEngine& renderer)
{
	return true;
}

//*---------------------------------------------------------------------------------------
//*【?】ミッション開始時のデータセット
//*
//* [引数]
//* &renderer : 描画エンジンの参照 
//* _missionNumber : ミッション番号（0から）
//* [返値]
//* true  : 成功 
//* false : 失敗 
//*----------------------------------------------------------------------------------------
bool DataManager::SettingsData_MissionLoad(RendererEngine& renderer, UINT _missionNumber)
{
	return true;
}

//*---------------------------------------------------------------------------------------
//*【?】ミッション終了時のデータセット
//*
//* [引数]
//* &renderer : 描画エンジンの参照 
//* _missionNumber : ミッション番号（0から）
//* [返値]
//* true  : 成功 
//* false : 失敗 
//*----------------------------------------------------------------------------------------
bool DataManager::SettingsData_MissionTermination(RendererEngine& renderer, UINT _missionNumber)
{
	return true;
}

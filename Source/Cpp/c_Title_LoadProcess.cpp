#include "pch.h"
#include "c_Title_LoadProcess.h"
#include "ResourceManager.h"
#include "SceneStateEnums.h"
#include "Component_SpriteRenderer.h"
#include "GameObject.h"
#include "MeshFactory.h"
#include "InputFactory.h"

using namespace SceneStateEnums;


//*---------------------------------------------------------------------------------------
//* @:c_Title_LoadProcess Class 
//*【?】開始
//* 引数：1.SceneManager
//* 返値：void
//*----------------------------------------------------------------------------------------
void c_Title_LoadProcess::OnEnter(SceneManager *pOwner)
{
	// すでにロードされているなら返す
	if (m_IsLoad) {
		return;
	}

	CreateSpriteInfo sprite;
	sprite.pTextureMap[0] = Master::m_pResourceManager->LoadWIC_Texture(L"Resource/Texture/Title/Title_Description_01.png");
	sprite.IsActive = true;
	sprite.ObjTag = "TitleLoad_Back_Sp";
	sprite.pRenderer = m_pRenderer;
	sprite.ShaderType = SHADER_TYPE::FORWARD_UNLIT_UI_SPRITE;
	sprite.Type = SPRITE_USAGE_TYPE::NORMAL;
	sprite.Width = 500.1f;
	sprite.Height = 500.1f;
	sprite.IsActive = true;
	sprite.IsTransparent = true;
	auto obj = MeshFactory::CreateSprite(sprite);
	if (obj) {
		m_pLoadBackSprite = obj->get_Component<SpriteRenderer>();
	}

	m_IsLoad = true;
}


//*---------------------------------------------------------------------------------------
//* @:c_Title_LoadProcess Class 
//*【?】終了
//* 引数：1.SceneManager
//* 返値：void
//*----------------------------------------------------------------------------------------
void c_Title_LoadProcess::OnExit(SceneManager *pOwner)
{
	//m_pLoadBackSprite->get_OwnerObj().lock()->set_StatusFlag(OBJECT_STATUS_BITFLAG::IS_ACTIVE);
}


//*---------------------------------------------------------------------------------------
//* @:c_Title_LoadProcess Class 
//*【?】更新
//* 引数：1.SceneManager
//* 返値：void
//*----------------------------------------------------------------------------------------
int c_Title_LoadProcess::Update(SceneManager *pOwner)
{
	// ゲームシーンへ
	if (GetInputDown(GAME_CONFIG::MOVE_JUMP))
	{
		return c_TITLE::c_GO_GAME_SCENE;
	}

	return c_TITLE::c_TITLE_LOAD_PROCESS;
}


//*---------------------------------------------------------------------------------------
//* @:c_Title_LoadProcess Class 
//*【?】描画
//* 引数：1.SceneManager
//* 返値：void
//*----------------------------------------------------------------------------------------
void c_Title_LoadProcess::Draw(SceneManager *pOwner)
{
	Master::m_pDirectWriteManager->DrawString("☆ロード中", VECTOR2::VEC2(940, 500));
}

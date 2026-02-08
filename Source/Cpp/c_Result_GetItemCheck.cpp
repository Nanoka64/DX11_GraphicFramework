#include "pch.h"
#include "c_Result_GetItemCheck.h"
#include "ResourceManager.h"
#include "SceneStateEnums.h"
#include "InputFactory.h"

using namespace SceneStateEnums;
//*---------------------------------------------------------------------------------------
//* @:c_Result_GetItemCheck Class 
//*【?】開始
//* 引数：1.SceneManager
//* 返値：void
//*----------------------------------------------------------------------------------------
void c_Result_GetItemCheck::OnEnter(SceneManager *pOwner)
{

}


//*---------------------------------------------------------------------------------------
//* @:c_Result_GetItemCheck Class 
//*【?】終了
//* 引数：1.SceneManager
//* 返値：void
//*----------------------------------------------------------------------------------------
void c_Result_GetItemCheck::OnExit(SceneManager *pOwner)
{

}


//*---------------------------------------------------------------------------------------
//* @:c_Result_GetItemCheck Class 
//*【?】更新
//* 引数：1.SceneManager
//* 返値：void
//*----------------------------------------------------------------------------------------
int c_Result_GetItemCheck::Update(SceneManager *pOwner)
{
	// タイトルシーンへ
	if (GetInputHold(GAME_CONFIG::MOVE_JUMP, 60))
	{
		return c_RESULT::c_GO_TITLE_SCENE;
	}

	return c_RESULT::c_GETITEM_CHECK;
}


//*---------------------------------------------------------------------------------------
//* @:c_Result_GetItemCheck Class 
//*【?】描画
//* 引数：1.SceneManager
//* 返値：void
//*----------------------------------------------------------------------------------------
void c_Result_GetItemCheck::Draw(SceneManager *pOwner)
{
	Master::m_pDirectWriteManager->DrawString("クリアおめでとう！", VECTOR2::VEC2(740, 540), "White_50_STD");
	Master::m_pDirectWriteManager->DrawString("Spaceを1秒以上長押しでタイトルに戻ります", VECTOR2::VEC2(540, 840), "White_40_STD");
}

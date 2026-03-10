#include "pch.h"
#include "c_Title_SoldierSelect.h"
#include "Root_TitleSceneState.h"
#include "GameObjectManager.h"
#include "ResourceManager.h"
#include "SceneStateEnums.h"
#include "InputFactory.h"
#include "GameObject.h"

using namespace VECTOR2;
using namespace VECTOR3;

// 項目の衝突判定用サイズ
static const VECTOR2::VEC2 g_ItemSize = VECTOR2::VEC2(400.0f, 70.0f);

// 項目の位置
static const VECTOR2::VEC2 g_ItemPosArray[UINT_CAST(SOLDIER_TYPE::NUM)] =
{
	VEC2(400.0f,600.0f),
	VEC2(1000.0f,600.0f),
	VEC2(400.0f,800.0f),
	VEC2(1000.0f,800.0f),
};

/// <summary>
/// 兵科名
/// </summary>
static constexpr const char* g_SoldierNames[UINT_CAST(SOLDIER_TYPE::NUM)] =
{
	"レンジャー",
	"ウイングダイバー",
	"エアレイダー",
	"フェンサー",
};

/// <summary>
/// マウスカーソルが項目の上に乗った際に、項目をどれくらいずらすか
/// </summary>
static const float g_MouseHoveredItemSlideOffset = 20.0f;


using namespace SceneStateEnums;

//*---------------------------------------------------------------------------------------
//* @:c_Title_SoldierSelect Class 
//*【?】開始
//* 引数：1.SceneManager
//* 返値：void
//*----------------------------------------------------------------------------------------
void c_Title_SoldierSelect::OnEnter(SceneManager *pOwner)
{
	// すでに初期化済みなら返す
	if (m_IsInit)
	{
		// 背景画像をアクティブに
		for (int i = 0; i < UINT_CAST(SOLDIER_TYPE::NUM); i++)
		{
			auto obj = m_pMenuItemBackSpriteTransform[i]->get_OwnerObj();
			if (obj.expired())
			{
				assert(false);
				continue;
			}
			obj.lock()->set_StatusFlag(OBJECT_STATUS_BITFLAG::IS_ACTIVE);
		}
		return;
	}

	m_PrevHoveredMIssionItem = 0;

	// アクティブにしてトランスフォームを取得
	for (int i = 0; i < UINT_CAST(SOLDIER_TYPE::NUM); i++)
	{
		auto obj = Master::m_pGameObjectManager->get_ObjectByTag("MenuItemBack_Sp" + std::to_string(i + 1));
		if (obj)
		{
			obj->set_StatusFlag(OBJECT_STATUS_BITFLAG::IS_ACTIVE);
			m_pMenuItemBackSpriteTransform[i] = obj->get_Transform().lock();
		}

		m_Items[i]._pos = g_ItemPosArray[i];
		m_Items[i]._name = g_SoldierNames[i];
	}
	m_IsInit = true;
}


//*---------------------------------------------------------------------------------------
//* @:c_Title_SoldierSelect Class 
//*【?】終了
//* 引数：1.SceneManager
//* 返値：void
//*----------------------------------------------------------------------------------------
void c_Title_SoldierSelect::OnExit(SceneManager *pOwner)
{
	// オブジェクトを非アクティブに
	for (int i = 0; i < UINT_CAST(SOLDIER_TYPE::NUM); i++)
	{
		auto obj = m_pMenuItemBackSpriteTransform[i]->get_OwnerObj();
		if (obj.expired())
		{
			assert(false);
			continue;
		}
		obj.lock()->clear_StatusFlag(OBJECT_STATUS_BITFLAG::IS_ACTIVE);
	}

}


//*---------------------------------------------------------------------------------------
//* @:c_Title_SoldierSelect Class 
//*【?】更新
//* 引数：1.SceneManager
//* 返値：void
//*----------------------------------------------------------------------------------------
int c_Title_SoldierSelect::Update(SceneManager *pOwner)
{
	// 右クリックでメインメニューへ戻る
	if (GetMouseClickDown(MOUSE_BUTTON_STATE::RIGHT))
	{
		return c_TITLE::c_TITLE_MAIN_MENU;
	}

	POINT mousePos = Master::m_pInputManager->GetMousePos();	// マウス座標

	m_NextState = c_TITLE_SOLDIER_SELECT;

	for (auto& item : m_Items)
	{
		// 項目の位置
		VEC2 menuItemPos = item._pos;

		// 衝突判定情報
		CollInData2D_AABB colData;
		colData._min = menuItemPos;
		colData._max = menuItemPos;
		colData._max += g_ItemSize;	// サイズ反映

		// マウスと項目の判定
		if (Master::m_pCollisionManager->HitCheck2D_BoxVsPoint(colData, VEC2(mousePos.x, mousePos.y)))
		{
			item._isHovered = true;	// マウスが乗ってる

			// 左クリックで選択
			if (GetMouseClickDown(MOUSE_BUTTON_STATE::LEFT))
			{
				//m_NextState = item._nextState;
			}
		}
		else
		{
			item._isHovered = false;
		}
	}

	//return m_NextState;

	return c_TITLE::c_TITLE_SOLDIER_SELECT;
}


//*---------------------------------------------------------------------------------------
//* @:c_Title_SoldierSelect Class 
//*【?】描画
//* 引数：1.SceneManager
//* 返値：void
//*----------------------------------------------------------------------------------------
void c_Title_SoldierSelect::Draw(SceneManager *pOwner)
{
	int counter = 0;

	for (auto& item : m_Items)
	{
		// 項目の位置
		VEC2 menuItemPos = item._pos;

		// スプライトの位置（アンカー座標を画面の中心にしてしまっているため、補正している）
		VEC2 spritePos;
		spritePos.x = -menuItemPos.x;
		spritePos.y = -menuItemPos.y + 480;

		// マウスが乗ってるならずらす
		if (item._isHovered)
		{
			if (m_PrevHoveredMIssionItem)
			{
				// ****************************************************
				//				カーソルが載った時のSE再生
				// ****************************************************
				Master::m_pSoundManager->Play(SOUND_TYPE::SE, SOUND_ID_TO_INT(SOUND_ID::SYSTEM_MOVING_CURSOR01));
			}

			m_PrevHoveredMIssionItem = 0;	// なんの項目に乗ったか保持

			// ずらす
			menuItemPos.x += g_MouseHoveredItemSlideOffset;
			spritePos.x += g_MouseHoveredItemSlideOffset;
		}


		// TODO:UI用もソートされてしまっているため描画順が崩れる
		// なので一旦オフセットのほうに位置を入れる
		m_pMenuItemBackSpriteTransform[counter]->set_VEC3ToLocal_Pos(VEC3(spritePos.x, spritePos.y, 0.0f));
		m_pMenuItemBackSpriteTransform[counter]->set_Scale(500.0f, 100.0f, 1.0f);

		// 文字表示
		Master::m_pDirectWriteManager->DrawString(item._name, menuItemPos, "White_40_STD");

		counter++;
	}

	Master::m_pDirectWriteManager->DrawString("☆兵科選択", VECTOR2::VEC2(40.0f, 500.0f), "White_40_STD");
}

#include "pch.h"
#include "Component_ConfigSetting.h"
#include "GameObjectManager.h"
#include "ResourceManager.h"
#include "SceneStateEnums.h"
#include "InputFactory.h"
#include "Component_SpriteRenderer.h"
#include "Component_ButtonUI.h"

using namespace SceneStateEnums;
using namespace UtilityData;
using namespace VECTOR2;
using namespace VECTOR3;
using namespace VECTOR4;


//*---------------------------------------------------------------------------------------
//*【?】コンストラクタ
//*----------------------------------------------------------------------------------------
ConfigSetting::ConfigSetting(std::weak_ptr<GameObject> pOwner, int updateRank) :
	IComponent(pOwner, updateRank),
	m_Timer(0.0f),
	m_pConfigBackSpriteObj(nullptr),
	m_pConfigSelectItemSpriteObj(nullptr),
	m_pConfigAdjustmentButtonObjArray(),
	m_ItemStartPos(CONFIG_ITEM_START_POS)
{

}

//*---------------------------------------------------------------------------------------
//*【?】デストラクタ
//*----------------------------------------------------------------------------------------
ConfigSetting::~ConfigSetting()
{
	ReleaseToUIPool();
}


//*---------------------------------------------------------------------------------------
//*【?】初期化
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//* [返値]なし
//*----------------------------------------------------------------------------------------
void ConfigSetting::Start(RendererEngine& renderer) 
{
	// 現在の設定情報を入れる
	const auto& config = Master::m_pDataManager->get_UserConfigData();
	m_DefaultConfigItemInfoArray[0] = ConfigMenuItemInfo({ 100.0f, 100.0f }, "", CONFIG_ITEM::BGM_VOLUME, VALUE_TYPE::INT, config._BGMVolume);
	m_DefaultConfigItemInfoArray[1] = ConfigMenuItemInfo({ 100.0f, 150.0f }, "", CONFIG_ITEM::SE_VOLUME, VALUE_TYPE::INT, config._SEVolume);
	m_DefaultConfigItemInfoArray[2] = ConfigMenuItemInfo({ 100.0f, 200.0f }, "", CONFIG_ITEM::MOUSE_SENSITIVITY, VALUE_TYPE::FLOAT, config._mouseSensitivity);
	m_DefaultConfigItemInfoArray[3] = ConfigMenuItemInfo({ 100.0f, 250.0f }, "", CONFIG_ITEM::INVERT_Y, VALUE_TYPE::BOOL, config._isInvertY);
	m_DefaultConfigItemInfoArray[4] = ConfigMenuItemInfo({ 100.0f, 300.0f }, "", CONFIG_ITEM::CAMERA_SHAKE, VALUE_TYPE::BOOL, config._isCameraShake);
	m_DefaultConfigItemInfoArray[5] = ConfigMenuItemInfo({ 100.0f, 350.0f }, "", CONFIG_ITEM::SHADOW_ENABLED, VALUE_TYPE::BOOL, config._isShadowEnabled);

	// 使用するスプライトのセットアップ
	BorrowFromUIPoolAndSetup(renderer);


	m_Timer = 0.0f;
}


//*---------------------------------------------------------------------------------------
//*【?】更新
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//* [返値]なし
//*----------------------------------------------------------------------------------------
void ConfigSetting::Update(RendererEngine& renderer)
{
	m_Timer += Master::m_pTimeManager->get_DeltaTime();

	for (int i = 0; i < INT_CAST(CONFIG_ITEM::NUM); i++)
	{
		VEC2 itemPos = m_ItemStartPos;
		itemPos.y += ITEM_POS_Y_BETWEEN_DIST * i;

		POINT mousePos = Master::m_pInputManager->GetMousePos();	// マウス座標

		// スプライトの衝突判定情報
		CollInData2D_AABB colData;
		colData._min = itemPos;
		colData._max = itemPos + CONFIG_ITEM_SIZE;

		auto selectItemRect = m_pConfigSelectItemSpriteObj->get_RectTransform().lock();
		auto selectItemSpriteComp = m_pConfigSelectItemSpriteObj->get_Component<SpriteRenderer>();
		selectItemSpriteComp->set_UVOffset(VEC2(0.0f, m_Timer));

		float t = sinf(m_Timer * 5.0f) * 0.5f + 0.5f;
		float ease = Tool::Easing::EaseOutSin(t);
		selectItemSpriteComp->set_Color(VEC4(1.0f * ease, 10.0f, 10.0f, 1.0f));


		// マウスとボタンの判定
		if (Master::m_pCollisionManager->HitCheck2D_BoxVsPoint(colData, VEC2(FLOAT_CAST(mousePos.x), FLOAT_CAST(mousePos.y))))
		{
			// 衝突時、黄色くする
			//spriteComp->set_Color(VEC4(1.0f, 1.0f, 1.0f, 1.0f));

			selectItemRect->set_RectPosition(itemPos);


			m_pConfigAdjustmentButtonObjArray[i][0]->get_Component<ButtonUI>()->set_IsInteractable(true);	// 左
			m_pConfigAdjustmentButtonObjArray[i][1]->get_Component<ButtonUI>()->set_IsInteractable(true);	// 右
		}
		// 何もないときは、無効状態に
		else
		{
			m_pConfigAdjustmentButtonObjArray[i][0]->get_Component<ButtonUI>()->set_IsInteractable(false);	
			m_pConfigAdjustmentButtonObjArray[i][1]->get_Component<ButtonUI>()->set_IsInteractable(false);
		}
	}
}


//*---------------------------------------------------------------------------------------
//*【?】描画
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//* [返値]なし
//*----------------------------------------------------------------------------------------
void ConfigSetting::Draw(RendererEngine& renderer)
{

	for (int i = 0; i < INT_CAST(CONFIG_ITEM::NUM); i++)
	{
		VEC2 itemPos = m_ItemStartPos;
		itemPos.y += ITEM_POS_Y_BETWEEN_DIST * i;

		// 項目名
		Master::m_pDirectWriteManager->SetOutLine(2.0f, D2D1::ColorF(D2D1::ColorF::Black));	 // 縁取り
		Master::m_pDirectWriteManager->SetColor(D2D1::ColorF(D2D1::ColorF::White));			 // 白
		Master::m_pDirectWriteManager->DrawStringToAligment(g_ConfigItemNames[i], itemPos, "White_20_STD", H_ALIGNMENT::LEADING, V_ALIGNMENT::CENTER, CONFIG_ITEM_SIZE);

		// 項目の値
		Master::m_pDirectWriteManager->SetColor(D2D1::ColorF(D2D1::ColorF::Aqua));	// 水色
		Master::m_pDirectWriteManager->DrawStringToAligment(m_ItemInfoArray[i].ConvertValueAsString(), m_ItemInfoArray[i]._pos, "White_20_STD", H_ALIGNMENT::CENTER, V_ALIGNMENT::CENTER, CONFIG_VALUE_OFFSET_SIZE);
	}
	Master::m_pDirectWriteManager->SetColor(D2D1::ColorF(D2D1::ColorF::White));
}


//*---------------------------------------------------------------------------------------
//*【?】値の変更
//*
//* [引数]
//* _item : 何の項目か
//*_isLeftIndex : 左ボタンからの入力か（trueなら左、falseなら右）
//* 
//* [返値] なし
//*----------------------------------------------------------------------------------------
void ConfigSetting::ChangeConfigValue(UtilityData::CONFIG_ITEM _item, bool _isLeftIndex)
{
	int index = INT_CAST(_item);
	if (index < 0 || index >= INT_CAST(CONFIG_ITEM::NUM))
	{
		return;
	}

	switch (_item)
	{
		//=========================================================================================
		//						BGM調整
		//=========================================================================================
	case UtilityData::CONFIG_ITEM::BGM_VOLUME:
	{
		int crntValue = std::get<int>(m_ItemInfoArray[index]._value);
		crntValue += (_isLeftIndex ? -1 : 1);
		crntValue = std::clamp(crntValue, MIN_BGM_VOLUME, MAX_BGM_VOLUME);	// クランプして範囲内に収める
		m_ItemInfoArray[index].SetValue(crntValue);
		Master::m_pDataManager->set_BGMVolume(crntValue);
	}
	break;
	//=========================================================================================
	//						SE調整
	//=========================================================================================
	case UtilityData::CONFIG_ITEM::SE_VOLUME:
	{
		int crntValue = std::get<int>(m_ItemInfoArray[index]._value);
		crntValue += (_isLeftIndex ? -1 : 1);
		crntValue = std::clamp(crntValue, MIN_SE_VOLUME, MAX_SE_VOLUME);
		m_ItemInfoArray[index].SetValue(crntValue);
		Master::m_pDataManager->set_SEVolume(crntValue);
	}
	break;
	//=========================================================================================
	//						マウス感度調整
	//=========================================================================================
	case UtilityData::CONFIG_ITEM::MOUSE_SENSITIVITY:
	{
		float crntValue = std::get<float>(m_ItemInfoArray[index]._value);
		crntValue += (_isLeftIndex ? -0.5f : 0.5f);
		crntValue = std::clamp(crntValue, MIN_MOUSE_SENSITIVITY, MAX_MOUSE_SENSITIVITY);
		m_ItemInfoArray[index].SetValue(crntValue);
		Master::m_pDataManager->set_MouseSensitivity(crntValue);
	}
	break;
	//=========================================================================================
	//						Y反転の有無
	//=========================================================================================
	case UtilityData::CONFIG_ITEM::INVERT_Y:
	{
		bool crntValue = std::get<bool>(m_ItemInfoArray[index]._value);
		crntValue = (_isLeftIndex ? false : true);
		m_ItemInfoArray[index].SetValue(crntValue);
		Master::m_pDataManager->set_IsInvertY(crntValue);
	}
	break;
	//=========================================================================================
	//						カメラシェイクの有無
	//=========================================================================================
	case UtilityData::CONFIG_ITEM::CAMERA_SHAKE:
	{
		bool crntValue = std::get<bool>(m_ItemInfoArray[index]._value);
		crntValue = (_isLeftIndex ? false : true);
		m_ItemInfoArray[index].SetValue(crntValue);
		Master::m_pDataManager->set_IsCameraShake(crntValue);
	}
	break;
	//=========================================================================================
	//						シャドウの有無
	//=========================================================================================
	case UtilityData::CONFIG_ITEM::SHADOW_ENABLED:
	{
		bool crntValue = std::get<bool>(m_ItemInfoArray[index]._value);
		crntValue = (_isLeftIndex ? false : true);
		m_ItemInfoArray[index].SetValue(crntValue);
		Master::m_pDataManager->set_IsShadowEnabled(crntValue);
	}
	break;
	case UtilityData::CONFIG_ITEM::NUM:
		break;
	default:
		break;
	}
};

//*---------------------------------------------------------------------------------------
//*【?】使用するスプライトを初期化する
//*
//* [引数]
//* *pOwner : シーンの親
//*
//* [返値] なし
//*----------------------------------------------------------------------------------------
void ConfigSetting::BorrowFromUIPoolAndSetup(RendererEngine& renderer)
{
	// 項目のオブジェクトとコンポーネントの取得と設定
	for (int i = 0; i < INT_CAST(CONFIG_ITEM::NUM); i++)
	{
		UIData::SpriteUIData spriteData;
		UIData::RectTransformData rectTrans;

		VEC2 itemPos = m_ItemStartPos;
		itemPos.y += ITEM_POS_Y_BETWEEN_DIST * i;


		//*****************************************************************************************
		//						メニュー項目
		//*****************************************************************************************
		m_ItemInfoArray[i]._name = g_ConfigItemNames[i];
		m_ItemInfoArray[i]._pos = itemPos + VEC2(BUTTON_OFFSET_POS_X - 50.0f, -15.0f);	// TODO:マジックナンバー（項目の値の位置がそのままではずれてしまうため）
		m_ItemInfoArray[i]._type = static_cast<CONFIG_ITEM>(i);
		m_ItemInfoArray[i]._value = m_DefaultConfigItemInfoArray[i]._value;
		m_ItemInfoArray[i]._valueType = m_DefaultConfigItemInfoArray[i]._valueType;

		//*****************************************************************************************
		//						ボタン
		//*****************************************************************************************

		/* 左側 */
		UIData::ButtonUIData buttonData;
		buttonData._imagePath = "Resource/Texture/Title/Cursor_L.png";
		buttonData._layerRank = 111;
		buttonData._repeatInputInterval = 5;	// ボタンを押し続けたときの連続入力の間隔
		buttonData._inputWaitFrame = 10;		// ボタンを押してから次の入力を受け付けるまでのフレーム数
		buttonData._tag = "AdjustmentButton" + std::to_string(i) + "_1";
		buttonData._inputValidationState = UIData::STATE::PRESSED;
		buttonData._onClicFunction = [this, i]() {this->ChangeConfigValue(static_cast<CONFIG_ITEM>(i), true); };	// クリックされたときに設定値を増やす関数を呼ぶ
		buttonData._inputSoundID = SOUND_ID_TO_INT(SOUND_ID::SYSTEM_MOVING_CURSOR01);
		rectTrans._size = CONFIG_ITEM_BUTTON_SIZE;
		rectTrans._pos = itemPos;
		rectTrans._pos.x += BUTTON_OFFSET_POS_X;	// 項目の右側に配置するようX座標をずらす
		m_pConfigAdjustmentButtonObjArray[i][0] = Master::m_pUIManager->GetButton(renderer, rectTrans, buttonData);
		auto leftButton = m_pConfigAdjustmentButtonObjArray[i][0]->get_Component<ButtonUI>();
		leftButton->set_Color(VEC4(0.0f), UIData::STATE::DISABLED);		// 無効の際は表示しない
		leftButton->set_Color(VEC4(5.0f, 5.0f, 0.0f, 1.0f), UIData::STATE::PRESSED);
		leftButton->set_IsInteractable(false);
		leftButton->set_AllowRepeatInput(true);		// 押し続けたときの連続入力を許可する
		leftButton->set_RepeatInputInterval(2);		// 入力間隔
		leftButton->set_InputWaitFrame(1);

		/* 右側 */
		buttonData._imagePath = "Resource/Texture/Title/Cursor_R.png";
		buttonData._tag = "AdjustmentButton" + std::to_string(i) + "_2";
		buttonData._inputValidationState = UIData::STATE::PRESSED;
		buttonData._onClicFunction = [this, i]() {this->ChangeConfigValue(static_cast<CONFIG_ITEM>(i), false); };	// クリックされたときに設定値を増やす関数を呼ぶ
		rectTrans._size = CONFIG_ITEM_BUTTON_SIZE;
		rectTrans._pos.x += BUTTON_POS_X_BETWEEM_DIST;					// ボタン同士の横方向の距離を空ける
		m_pConfigAdjustmentButtonObjArray[i][1] = Master::m_pUIManager->GetButton(renderer, rectTrans, buttonData);
		auto rightButton = m_pConfigAdjustmentButtonObjArray[i][1]->get_Component<ButtonUI>();
		rightButton->set_Color(VEC4(0.0f), UIData::STATE::DISABLED);	// 無効の際は表示しない
		rightButton->set_Color(VEC4(5.0f, 5.0f, 0.0f, 1.0f), UIData::STATE::PRESSED);
		rightButton->set_IsInteractable(false);
		rightButton->set_AllowRepeatInput(true);		// 押し続けたときの連続入力を許可する
		rightButton->set_RepeatInputInterval(2);		// 入力間隔
		rightButton->set_InputWaitFrame(1);

		//m_pButtonArray[i] = m_pConfigAdjustmentButtonObj[i]->get_Component<ButtonUI>();
		//m_pMenuItemRectTransformArray[i] = m_pConfigAdjustmentButtonObj[i]->get_RectTransform();
	}

	// 最初は一番上の項目のみ、有効状態
	m_pConfigAdjustmentButtonObjArray[0][0]->get_Component<ButtonUI>()->set_IsInteractable(true);
	m_pConfigAdjustmentButtonObjArray[0][1]->get_Component<ButtonUI>()->set_IsInteractable(true);

	//*****************************************************************************************
	//						選択した項目をハイライトするスプライト
	//*****************************************************************************************
	UIData::SpriteUIData selectItemSpriteData;
	UIData::RectTransformData selectItemRectTrans;
	selectItemSpriteData._imagePath = "Resource/Texture/Title/Decoration01.png";
	selectItemSpriteData._tag = "ConfigMenuBackSprite";
	selectItemSpriteData._color = VEC4(1.0f, 10.0f, 10.0f, 1.0f);
	//selectItemSpriteData._shaderType = SHADER_TYPE::FORWARD_UNLIT_UI_NOTEXTURE_SPRITE;
	selectItemSpriteData._layerRank = 110;
	selectItemRectTrans._size = CONFIG_ITEM_SIZE;
	selectItemRectTrans._pos = m_ItemStartPos;
	m_pConfigSelectItemSpriteObj = Master::m_pUIManager->GetSprite(renderer, selectItemRectTrans, selectItemSpriteData);


	//*****************************************************************************************
	//						最奥背景スプライト
	//*****************************************************************************************
	VEC2 backSpritePos = m_ItemStartPos;	// 項目の開始位置を基準にする
	backSpritePos.x -= 100.0f;
	backSpritePos.y -= 50.0f;
	UIData::SpriteUIData backSpriteData;
	UIData::RectTransformData backRectTrans;
	backSpriteData._imagePath = "Resource/Texture/Title/ConfigBackGround.png";
	backSpriteData._layerRank = 100;
	backSpriteData._tag = "ConfigSceneBackSprite";
	backRectTrans._size = BACK_SPRITE_SIZE;
	backRectTrans._pos = backSpritePos;
	m_pConfigBackSpriteObj = Master::m_pUIManager->GetSprite(renderer, backRectTrans, backSpriteData);

}


//*---------------------------------------------------------------------------------------
//*【?】スプライトを開放する（プールへ返す）
//*
//* [引数 なし
//* [返値] なし
//*----------------------------------------------------------------------------------------
void ConfigSetting::ReleaseToUIPool()
{
	// *****************************************************************************************
	// 	// オブジェクトを非アクティブに（プールに返す）
	// *****************************************************************************************
	for (int i = 0; i < INT_CAST(CONFIG_ITEM::NUM); i++)
	{
		// ボタン 左右
		for (int j = 0; j < 2; j++) {
			if (m_pConfigAdjustmentButtonObjArray[i][j])
			{
				m_pConfigAdjustmentButtonObjArray[i][j]->clear_StatusFlag(OBJECT_STATUS_BITFLAG::IS_ACTIVE);
			}
		}
	}
	if (m_pConfigSelectItemSpriteObj != nullptr) {
		m_pConfigSelectItemSpriteObj->clear_StatusFlag(OBJECT_STATUS_BITFLAG::IS_ACTIVE);
	}

	if (m_pConfigBackSpriteObj != nullptr) {
		m_pConfigBackSpriteObj->clear_StatusFlag(OBJECT_STATUS_BITFLAG::IS_ACTIVE);
	}
}


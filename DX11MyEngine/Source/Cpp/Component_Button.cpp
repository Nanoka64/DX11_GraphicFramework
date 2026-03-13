#include "pch.h"
#include "Component_Button.h"
#include "RendererEngine.h"


using namespace VECTOR2;
using namespace VECTOR3;
using namespace VECTOR4;

//*---------------------------------------------------------------------------------------
//*【?】コンストラクタ
//* [引数]
//* pOwner : オーナーオブジェクト
//* updateRank : 更新レイヤー
//*----------------------------------------------------------------------------------------
Button::Button(std::weak_ptr<GameObject> pOwner, int updateRank)
	: IComponent(pOwner, updateRank),
	m_StateColor(VEC4()),
	m_CrntState(STATE::NORMAL),
	m_FadeDuration(0.1f),
	m_IsInteractable(true),
	m_Text("Button")
{
	this->set_Tag("Button");
}


//*---------------------------------------------------------------------------------------
//*【?】デストラクタ
//*----------------------------------------------------------------------------------------
Button::~Button()
{

}


//*---------------------------------------------------------------------------------------
//*【?】初期化
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//* [返値]なし
//*----------------------------------------------------------------------------------------
void Button::Start(RendererEngine &renderer)
{
	if (m_pOwner.expired())
	{
		assert(false);
		return;
	}
	m_pMyTransform = m_pOwner.lock()->get_RectTransform();
}


//*---------------------------------------------------------------------------------------
//*【?】更新
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//* [返値]なし
//*----------------------------------------------------------------------------------------
void Button::Update(RendererEngine &renderer)
{
	POINT mousePos = Master::m_pInputManager->GetMousePos();	// マウス座標
	auto transform = m_pMyTransform.lock();
	VEC3 pos = transform->get_VEC3ToPos();
	VEC3 scale = transform->get_VEC3ToScale();

	// ボタンの衝突判定情報
	CollInData2D_AABB colData;
	colData._min = VEC2(pos.x, pos.y);
	colData._max = VEC2(pos.x, pos.y);

	// マウスとボタンの判定
	if (Master::m_pCollisionManager->HitCheck2D_BoxVsPoint(colData, VEC2(mousePos.x, mousePos.y)))
	{
		// 左クリックで選択
		if (GetMouseClickDown(MOUSE_BUTTON_STATE::LEFT))
		{
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
void Button::Draw(RendererEngine &renderer)
{
	Master::m_pDirectWriteManager->DrawString(m_Text, VECTOR2::VEC2(40.0f, 500.0f), "White_40_STD");
}

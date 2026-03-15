#include "pch.h"
#include "Component_RectTransform.h"
#include "RendererEngine.h"


using namespace VECTOR2;
using namespace VECTOR3;

//*---------------------------------------------------------------------------------------
//*【?】コンストラクタ
//* [引数]
//* pOwner : オーナーオブジェクト
//* updateRank : 更新レイヤー
//*----------------------------------------------------------------------------------------
RectTransform::RectTransform(std::weak_ptr<GameObject> pOwner, int updateRank) : 
	MyTransform(pOwner, updateRank),
	m_AnchoredPosition(VEC2()),
	m_SizeDelta(VEC2(100.0f,100.0f)),
	m_RectSize(VEC2(1.0f, 1.0f)),
	m_AnchorMin(VEC2(0.5f, 0.5f)),
	m_AnchorMax(VEC2(0.5f, 0.5f)),
	m_Pivot(VEC2()),
	m_WorldMatrix( XMMatrixIdentity()),
	m_CalculatedWidth(0.0f),
	m_CalculatedHeight(0.0f)
{
	this->set_Tag("RectTransform");
}


//*---------------------------------------------------------------------------------------
//*【?】デストラクタ
//*----------------------------------------------------------------------------------------
RectTransform::~RectTransform()
{

}


//*---------------------------------------------------------------------------------------
//*【?】ワールド変換行列の計算
//* TODO:計算ロジック部分をほとんどAIに頼んでしまったので、復習して理解できるようにする！
//* 
//* [引数] なし
//*
//* [返値]
//* ワールド変換行列 
//*----------------------------------------------------------------------------------------
void RectTransform::UpdateUILocalMatrix()
{
    // スクリーンの大きさをまずは親とする
	float parentWidth =  Master::m_pDataManager->get_ScreenWidth();
	float parentHeight = Master::m_pDataManager->get_ScreenHeight();
	VEC2 parentPivot = { 0.5f, 0.5f };

    // 親が設定されていればそっちに入れ替え
	if (auto parent = m_pParentRect.lock())
	{
		parentWidth = parent->get_Width();
		parentHeight = parent->get_Height();
		parentPivot = parent->get_Pivot();
	}

	// アンカー間の距離（親のサイズに対する割合）＋ SizeDelta
	float anchorWidth = parentWidth * (m_AnchorMax.x - m_AnchorMin.x);
	float anchorHeight = parentHeight * (m_AnchorMax.y - m_AnchorMin.y);

	m_CalculatedWidth = anchorWidth + m_SizeDelta.x;
	m_CalculatedHeight = anchorHeight + m_SizeDelta.y;

    // ローカル座標の計算（親のピボット位置を原点(0,0)としたときの自身のピボットの相対座標）
    // 親の左上(0,0)から見た、自身のアンカーの基準位置
    float anchorBaseX = parentWidth * m_AnchorMin.x + anchorWidth * m_Pivot.x;
    float anchorBaseY = parentHeight * m_AnchorMin.y + anchorHeight * m_Pivot.y;

    // 親の左上(0,0)から見た、親のピボット位置
    float parentPivotPosX = parentWidth * parentPivot.x;
    float parentPivotPosY = parentHeight * parentPivot.y;

    // 親のピボットからの相対距離 ＋ AnchoredPosition
    float localPosX = (anchorBaseX - parentPivotPosX) + m_AnchoredPosition.x;
    float localPosY = (anchorBaseY - parentPivotPosY) + m_AnchoredPosition.y;

    // UI用ローカル変換行列の構築 (DirectX 11用)
    // 1x1のメッシュを想定しているため、ピボット位置を基準にメッシュをずらす
    XMMATRIX offsetMtx = XMMatrixTranslation(
        -m_CalculatedWidth * m_Pivot.x,
        -m_CalculatedHeight * m_Pivot.y,
        0.0f
    );

    // スケール（UI自体の幅・高さ × TransformのScale）
    XMMATRIX scaleMtx = XMMatrixScaling(
        m_CalculatedWidth * m_RectSize.x,
        m_CalculatedHeight * m_RectSize.y,
        1.0f
    );

    // 回転
    XMVECTOR rot = XMQuaternionMultiply(m_Local_RotationQ, m_RotationQ);
    XMMATRIX rotMtx = XMMatrixRotationQuaternion(rot);

    // 移動（親のピボットからの相対位置）
    XMMATRIX transMtx = XMMatrixTranslation(localPosX, localPosY, 0.0f);

    // ローカル行列の合成（Offset -> Scale -> Rotation -> Translation）
    XMMATRIX localMatrix = offsetMtx * scaleMtx * rotMtx * transMtx;

    //ワールド行列の計算（親のワールド行列と掛け合わせる）
    XMMATRIX parentWorldMtx = XMMatrixIdentity();
    if (auto parent = m_pParentRect.lock())
    {
        parentWorldMtx = parent->get_WorldMtx();
    }

    m_WorldMatrix = localMatrix * parentWorldMtx;
}

// -----------------------------------------------------------------------------
/// <summary>
/// ワールド行列取得
/// </summary>
/// <returns></returns>
// -----------------------------------------------------------------------------
XMMATRIX RectTransform::get_WorldMtx()const {
    return m_WorldMatrix;;
}

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
RectTransform::RectTransform(std::weak_ptr<GameObject> pOwner, int updateRank) : MyTransform(pOwner, updateRank)
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
//*【?】ワールド変換行列の取得
//*
//* [引数] なし
//*
//* [返値]
//* ワールド変換行列 
//*----------------------------------------------------------------------------------------
XMMATRIX RectTransform::get_WorldMtx()const
{
	XMMATRIX world;
	return world;
}

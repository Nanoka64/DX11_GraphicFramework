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
RectTransform::RectTransform(std::weak_ptr<GameObject> pOwner, int updateRank) : IComponent(pOwner, updateRank)
{
	this->set_Tag("RectTransform");
}


//*---------------------------------------------------------------------------------------
//*【?】デストラクタ
//*----------------------------------------------------------------------------------------
RectTransform::~RectTransform()
{

}


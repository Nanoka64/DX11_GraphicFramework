#include "pch.h"
#include "Component_ModelMeshResource.h"

//*---------------------------------------------------------------------------------------
//* @:ModelMeshResource Class 
//*【?】コンストラクタ
//* 引数：1.
//*----------------------------------------------------------------------------------------
ModelMeshResource::ModelMeshResource(std::weak_ptr<GameObject> pOwner, int updateRank) : IComponent(pOwner, updateRank)
{
    this->set_Tag("ModelMeshResource");
}


//*---------------------------------------------------------------------------------------
//* @:ModelMeshResource Class 
//*【?】デストラクタ
//*----------------------------------------------------------------------------------------
ModelMeshResource::~ModelMeshResource()
{
}



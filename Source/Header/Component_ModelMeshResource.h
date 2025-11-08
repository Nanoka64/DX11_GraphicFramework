#pragma once
#include "IComponent.h"
#include "ModelData.h"

class ModelMeshResource : public IComponent
{
private:
    std::weak_ptr<class ModelData> m_pModelData;

public:
    ModelMeshResource(std::weak_ptr<GameObject> pOwner, int updateRank = 100);
    ~ModelMeshResource();

    const std::weak_ptr<class ModelData> get_ModelData()const { return m_pModelData; }
    void set_ModelData(std::weak_ptr<class ModelData> pData){ m_pModelData = pData; };
};


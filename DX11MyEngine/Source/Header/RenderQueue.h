#pragma once
#include "RenderCommand.h"


class RenderQueue
{
private:
    std::vector<RenderData::RenderCommandData> m_ShadowItems;
    std::vector<RenderData::RenderCommandData> m_GBufferItems;
    std::vector<RenderData::RenderCommandData> m_DecalItems;
    std::vector<RenderData::RenderCommandData> m_ForwardOpaqueItems;
    std::vector<RenderData::RenderCommandData> m_TransparentItems;

    //std::vector<RenderData::ObjectRenderData> m_ObjectData;
};


#include "pch.h"
#include "EditorManager.h"
#include "EditWindowBase.h"
#include "HierarchyWindow.h"
#include "InspectorWindow.h"
#include "GameObject.h"

EditorManager::EditorManager()
{
}

EditorManager::~EditorManager()
{
}

bool EditorManager::Init(RendererEngine &renderer)
{
    // Hierarchyウィンドウの生成
    m_EditWindowMap["Hierarchy"] = std::make_unique<HierarchyWindow>();
    m_EditWindowMap["Inspector"] = std::make_unique<InspectorWindow>();

    m_EditWindowMap["Hierarchy"]->Init(renderer);
    m_EditWindowMap["Inspector"]->Init(renderer);

    return true;
}

void EditorManager::Update(RendererEngine &renderer)
{
    // 各ウィンドウの更新
    for (auto& window : m_EditWindowMap)
    {
        window.second->Update(renderer);
    }
}

void EditorManager::set_crntSelectedObject(std::shared_ptr<class GameObject> pObj)
{
    m_pCrntSelectedObject = pObj;
}

std::shared_ptr<class GameObject> EditorManager::get_crntSelectedObject()
{
    return m_pCrntSelectedObject;
}
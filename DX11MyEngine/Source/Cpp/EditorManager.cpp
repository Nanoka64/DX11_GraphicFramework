#include "pch.h"
#include "EditorManager.h"
#include "EditWindowBase.h"
#include "HierarchyWindow.h"
#include "InspectorWindow.h"
#include "GameObject.h"
#include "RendererEngine.h"

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
    // エディタが閉じてる
    if (Master::m_pDataManager->get_IsDebugMode() == false)
    {
        return;
    }

    // ここではImGuiの描画命令だけを登録する。実際の描画はDebugger::EndFrameで行われるため、
    // この後にゲームを描画してSRVを更新しても、同じフレームの完成画像を表示できる。
    // パディングをなくし、Gameパネルの表示領域を最大限使う。
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    const ImGuiWindowFlags gameWindowFlags =
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
    if (ImGui::Begin("Game", nullptr, gameWindowFlags))
    {
        const ImVec2 availableSize = ImGui::GetContentRegionAvail();
        const float sourceWidth = static_cast<float>(renderer.get_ScreenWidth());
        const float sourceHeight = static_cast<float>(renderer.get_ScreenHeight());

        if (availableSize.x > 0.0f && availableSize.y > 0.0f &&
            sourceWidth > 0.0f && sourceHeight > 0.0f)
        {
            // 縦横それぞれの倍率の小さい方を採用し、元のアスペクト比を保ったまま収める。
            const float imageScale = std::min(
                availableSize.x / sourceWidth,
                availableSize.y / sourceHeight);
            const ImVec2 imageSize(sourceWidth * imageScale, sourceHeight * imageScale);
            const ImVec2 cursorPos = ImGui::GetCursorPos();

            // 余った領域を左右または上下へ均等に配分し、ゲーム画面を中央へ配置する。
            ImGui::SetCursorPos(ImVec2(
                cursorPos.x + (availableSize.x - imageSize.x) * 0.5f,
                cursorPos.y + (availableSize.y - imageSize.y) * 0.5f));

            // Rendererが保存した「ゲームだけの完成フレーム」をImGuiの画像として表示する。
            if (auto gameViewSrv = renderer.get_FrameBufferSRV_ComPtr())
            {
                ImGui::Image((ImTextureID)gameViewSrv.Get(), imageSize);
            }
        }
    }
    ImGui::End();
    ImGui::PopStyleVar();

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

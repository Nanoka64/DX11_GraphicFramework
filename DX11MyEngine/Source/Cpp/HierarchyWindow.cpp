#include "pch.h"
#include "HierarchyWindow.h"
#include "EditorBase.h"
#include "RendererEngine.h"
#include "GameObject.h"

//*---------------------------------------------------------------------------------------
//*【?】コンストラクタ
//*----------------------------------------------------------------------------------------
HierarchyWindow::HierarchyWindow() : EditWindowBase()
{
}

//*---------------------------------------------------------------------------------------
//*【?】デストラクタ
//*----------------------------------------------------------------------------------------
HierarchyWindow::~HierarchyWindow()
{
}

//*---------------------------------------------------------------------------------------
//*【?】初期化
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//*
//* [返値]
//* true : 成功
//* false : 失敗
//*----------------------------------------------------------------------------------------
bool HierarchyWindow::Init(RendererEngine &renderer)
{
    m_EditorMap[""];

    return true;
}

//*---------------------------------------------------------------------------------------
//*【?】更新
//*
//* [引数]
//* &renderer : 描画エンジンの参照
//*
//* [返値] なし
//*----------------------------------------------------------------------------------------
void HierarchyWindow::Update(RendererEngine &renderer)
{
    using namespace Tool;

    std::shared_ptr<GameObject> selectedObject = nullptr;   // 選択されたオブジェクトのポインタ
    std::vector<std::shared_ptr<GameObject>> opawueObjects3D = Master::m_pGameObjectManager->get_3DOpaque_ObjectList();
    std::vector<std::shared_ptr<GameObject>> transparentObjects3D = Master::m_pGameObjectManager->get_3DTransparent_ObjectList();
    
    std::vector<std::shared_ptr<GameObject>> objects;

    // 合体 vectorバージョン
    objects.insert(objects.begin(),opawueObjects3D.begin(), opawueObjects3D.end());
    objects.insert(objects.end(), transparentObjects3D.begin(), transparentObjects3D.end());

    // 透明/不透明オブジェクトリスト合体
    //objects.splice(objects.end(), transparentObjList);
    //objects.splice(objects.end(), opawueObjList); 

    Master::m_pDebugger->BeginDebugWindow(U8ToChar(u8"オブジェクトリスト"));
    Master::m_pDebugger->DG_BulletText(U8ToChar(u8"数 : %d"), objects.size());

    int id = 0;

    for (auto& obj : objects)
    {
        if (!obj->get_IsStatusFlag(OBJECT_STATUS_BITFLAG::IS_ACTIVE))
        {
            continue;
        }
        
        bool isSelected = false;
        id++;
        // IDを名前に入れて一意にする（##ID は表示されず、内部的な識別子になる）
        std::string label = obj->get_Tag() + "##" + std::to_string(id);

        // 選択可能なリストアイテムを作成
        if (Master::m_pDebugger->DG_Selectable(label, &isSelected, ImGuiSelectableFlags_None, VECTOR2::VEC2(300, 20)))
        {
            // 選択されたオブジェクトを保存
            selectedObject = obj;

            // 選択されたオブジェクトをマネージャに伝える
            Master::m_pEditorManager->set_crntSelectedObject(selectedObject);
        }
    }
    Master::m_pDebugger->EndDebugWindow();
}


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
void HierarchyWindow::Update(RendererEngine& renderer)
{
    using namespace Tool;

    std::shared_ptr<GameObject> selectedObject = nullptr;   // 選択されたオブジェクトのポインタ
    std::vector<std::shared_ptr<GameObject>> opawueObjects3D = Master::m_pGameObjectManager->get_3DOpaque_Objects();
    std::vector<std::shared_ptr<GameObject>> transparentObjects3D = Master::m_pGameObjectManager->get_3DTransparent_Objects();
    std::vector<std::shared_ptr<GameObject>> transparentObjects2D = Master::m_pGameObjectManager->get_2DTransparent_Objects();

    std::vector<std::shared_ptr<GameObject>> objects;

    // 合体 vectorバージョン
    objects.insert(objects.begin(), opawueObjects3D.begin(), opawueObjects3D.end());
    objects.insert(objects.end(), transparentObjects3D.begin(), transparentObjects3D.end());
    objects.insert(objects.end(), transparentObjects2D.begin(), transparentObjects2D.end());

    // 透明/不透明オブジェクトリスト合体
    //objects.splice(objects.end(), transparentObjList);
    //objects.splice(objects.end(), opawueObjList); 

    Master::m_pDebugger->BeginDebugWindow(U8ToChar(u8"オブジェクトリスト"),0);
    Master::m_pDebugger->DG_BulletText(U8ToChar(u8"数 : %d"), objects.size());

    int id = 0;

    // 重複しているタグでオブジェクトをまとめる
    std::map<std::string, std::vector<std::shared_ptr<GameObject>>> nameByObjectsMap;
    for (auto& obj : objects)
    {
        if (!obj->get_IsStatusFlag(OBJECT_STATUS_BITFLAG::IS_ACTIVE))
        {
            continue;
        }

        nameByObjectsMap[obj->get_Tag()].push_back(obj);
    }
    // ノードで表示するオブジェクト
    std::vector<std::pair<std::string, std::vector<std::shared_ptr<GameObject>>>> nodeObjects(nameByObjectsMap.begin(), nameByObjectsMap.end());  
    // 単体で表示するオブジェクト
    std::vector<std::shared_ptr<GameObject>> singleObjects;         

    // ノード用mapのvectorをサイズが大きい順にソート
    std::sort(nodeObjects.begin(), nodeObjects.end(), [](const auto& a, const auto& b)
        {
            return a.second.size() > b.second.size();
        });

    // ノード用のmapから要素が一つのみのvectorを削除し、単一オブジェクト用のvectorに移す
    std::erase_if(nodeObjects,
        [&singleObjects](const auto& _val) {
            // 一つしか存在しないので、これで削除
            if (_val.second.size() == 1)
            {
                singleObjects.push_back(_val.second.front());
                return true;
            }
            return false;
        }
    );

    // 単一オブジェクトvecrtorをレイヤー順にソート
    std::sort(singleObjects.begin(), singleObjects.end(),
        [](const auto& a, const auto& b)
        {
            return a->get_LayerRank() < b->get_LayerRank();
        });


    // タグごとにノードを分けて表示（1つのみの場合は、ノードに分けない）
    for (auto& pair : nodeObjects)
    {
        if (Master::m_pDebugger->DG_TreeNode(pair.first))
        {
            SelectObjectsDraw(pair.second);
            Master::m_pDebugger->DG_TreePop();
        }
    }
 
    // 単一オブジェクトvectorを表示
    SelectObjectsDraw(singleObjects);

    Master::m_pDebugger->EndDebugWindow();
}

//*---------------------------------------------------------------------------------------
//*【?】選択可能オブジェクトを一覧表示
//*
//* [引数]
//* &_objects : 表示するオブジェクトリスト
//*
//* [返値] なし
//*----------------------------------------------------------------------------------------
void HierarchyWindow::SelectObjectsDraw(std::vector<std::shared_ptr<GameObject>>& _objects)const
{
    int id = 0;

    for (auto& obj : _objects)
    {
        if (!obj->get_IsStatusFlag(OBJECT_STATUS_BITFLAG::IS_ACTIVE))
        {
            continue;
        }

        bool isSelected = false;
        id++;
        // IDを名前に入れて一意にする（##ID は表示されず、内部的な識別子になる）
        std::string label = std::to_string(obj->get_LayerRank()) + obj->get_Tag() + "##" + std::to_string(id);

        // 選択可能なリストアイテムを作成
        if (Master::m_pDebugger->DG_Selectable(label, &isSelected, ImGuiSelectableFlags_None, VECTOR2::VEC2(300, 20)))
        {
            // 選択されたオブジェクトを保存
            //selectedObject = obj;

            // 選択されたオブジェクトをマネージャに伝える
            Master::m_pEditorManager->set_crntSelectedObject(obj);
        }
    }
}


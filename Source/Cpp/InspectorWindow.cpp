#include "pch.h"
#include "InspectorWindow.h"
#include "EditorBase.h"
#include "RendererEngine.h"
#include "InspectorEditors.h"
#include "GameObject.h"

//*---------------------------------------------------------------------------------------
//*【?】コンストラクタ
//*----------------------------------------------------------------------------------------
InspectorWindow::InspectorWindow(): EditWindowBase()
{
}

//*---------------------------------------------------------------------------------------
//*【?】デストラクタ
//*----------------------------------------------------------------------------------------
InspectorWindow::~InspectorWindow()
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
bool InspectorWindow::Init(RendererEngine &renderer)
{
    // 各エディタの生成
    m_EditorMap["Transform"]            = std::make_unique<TransformEditor>();
    m_EditorMap["DirectionalLight"]     = std::make_unique<DirectionLightEditor>();
    m_EditorMap["PointLight"]           = std::make_unique<PointLightEditor>();
    m_EditorMap["PlayerController"]     = std::make_unique<PlayerControllerEditor>();
    m_EditorMap["Camera3D"]             = std::make_unique<Camera3DEditor>();
    m_EditorMap["SkinnedMeshAnimator"]  = std::make_unique<SkinnedMeshAnimatorEditor>();

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
void InspectorWindow::Update(RendererEngine &renderer)
{
    using namespace Tool;

    // 現在選択されているオブジェクトの取得
    m_pCrntSelectedObject = Master::m_pEditorManager->get_crntSelectedObject();

    Master::m_pDebugger->BeginDebugWindow(U8ToChar(u8"インスペクター"));

    // 各エディタの更新
    // 今はTransformエディタのみ実装
    for (auto &editor : m_EditorMap)
    {
        // 更新対象のオブジェクトが存在しているか
        if (auto obj = m_pCrntSelectedObject.lock())
        {
            editor.second->OnEditorGUI(renderer, *obj);
        }
    }
    Master::m_pDebugger->EndDebugWindow();
}


//*---------------------------------------------------------------------------------------
//*【?】現在選択されているオブジェクトの設定
//*
//* [引数]
//* pObj : 選択されたオブジェクトのポインタ
//*
//* [返値] なし
//*----------------------------------------------------------------------------------------
void InspectorWindow::set_CrntSelectedObject(std::shared_ptr<class GameObject> pObj)
{
    m_pCrntSelectedObject = pObj; 
}

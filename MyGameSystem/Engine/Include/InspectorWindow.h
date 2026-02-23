#pragma once
#include "EditWindowBase.h"

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:InspectorWindow Class --- */
//
//  ★継承：EditWindowBase ★
//
// 【?】Unityのインスペクターウィンドウのような機能を実装するクラス
//      各エディタのインスタンスを保持し、編集ウィンドウとして描画する
//
// ***************************************************************************************
class InspectorWindow : public EditWindowBase
{
private:
    std::unordered_map<std::string, std::unique_ptr<class EditorBase>> m_EditorMap; 
    std::weak_ptr<class GameObject> m_pCrntSelectedObject; // 現在選択されているオブジェクトのポインタ

public:
    InspectorWindow();
    ~InspectorWindow() override;
    bool Init(RendererEngine& renderer) override;
    void Update(RendererEngine &renderer)override;

    /// <summary>
    /// 現在選択されているオブジェクトを設定
    /// </summary>
    /// <param name="pObj"></param>
    void set_CrntSelectedObject(std::shared_ptr<class GameObject> pObj);
};


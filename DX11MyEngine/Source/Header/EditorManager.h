#pragma once


// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:EditorManager Class --- */
//
//  ★★★シングルトン★★★
//
// 【?】 エディタ関連の管理を行うクラス
//      
//
// ***************************************************************************************
class EditorManager
{
private:
    std::unordered_map<std::string, std::unique_ptr<class EditWindowBase>> m_EditWindowMap;
    std::shared_ptr<class GameObject> m_pCrntSelectedObject; // 現在選択されているオブジェクトのポインタ

public:
    EditorManager();
    ~EditorManager();

    bool Init(RendererEngine &renderer);                  // 初期化
    void Update(RendererEngine &renderer);                // 更新

    void set_crntSelectedObject(std::shared_ptr<class GameObject> pObj); // 現在選択されているオブジェクトを設定
    std::shared_ptr<class GameObject> get_crntSelectedObject(); // 現在選択されているオブジェクトを取得
private:
    // コピー禁止
    EditorManager(const EditorManager &) = delete;
    EditorManager &operator=(const EditorManager &) = delete;
    // ------------------------------------------------------
};


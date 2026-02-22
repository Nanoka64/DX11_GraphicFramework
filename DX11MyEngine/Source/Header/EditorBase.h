#pragma once

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:EditorBase Class --- */
//
//  ★★★抽象クラス★★★
//
// 【?】Unityベースのエディタ機能を実装するための基底クラス
//
// ***************************************************************************************
class EditorBase
{
protected:

public:
    EditorBase() = default;
    virtual ~EditorBase() = default;

    virtual bool Init(RendererEngine &renderer) = 0;
    virtual void OnEditorGUI(RendererEngine &renderer, class GameObject &pObj) = 0;

private:


};


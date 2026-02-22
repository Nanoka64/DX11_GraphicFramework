#pragma once
#include "EditorBase.h"



// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:ObjectSelect Class --- */
//
//  ★継承：EditorBase ★
//
// 【?】オブジェクト選択用エディタ
//
// ***************************************************************************************
class ObjectSelect : public EditorBase
{
private:


public:
    ObjectSelect();
    ~ObjectSelect();
    bool Init(RendererEngine &renderer) override;
    void OnEditorGUI(RendererEngine &renderer, class GameObject &pObj) override;
private:

};


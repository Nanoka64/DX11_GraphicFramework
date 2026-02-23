#pragma once

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:EditWindowBase Class --- */
//
//  ★★★抽象クラス★★★
//
// 【?】編集ウィンドウの基底クラス
//
// ***************************************************************************************
class EditWindowBase
{
protected:

public:
    EditWindowBase() = default;
    virtual ~EditWindowBase() = default;

    virtual bool Init(RendererEngine &renderer) = 0;
    virtual void Update(RendererEngine &renderer) = 0;
private:


};


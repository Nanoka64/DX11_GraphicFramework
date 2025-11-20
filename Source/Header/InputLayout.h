#pragma once
#include "IDXResource.h"

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:InputLayout Class --- */
//
//  ★継承：IDXResource ★
//
// 【?】入力レイアウトをラップする
//      ShaderManagerのみがこのクラスの所有権を持つ
//
// ***************************************************************************************

class InputLayout : public IDXResource<ID3D11InputLayout>
{
private:
    //ID3D11InputLayout *m_pVertexLayout;   // 頂点フォーマット


public:
    InputLayout();
    ~InputLayout();

    bool Create();
    void Release() override;
};


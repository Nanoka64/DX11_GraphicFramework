#pragma once
#include "IDXResource.h"


// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:IndexBuffer Class --- */
//
//  ★継承：IDXResource ★
//
// 【?】インデックスバッファをラップする
//
// ***************************************************************************************
class IndexBuffer : public IDXResource<void>
{
private:


public:
    IndexBuffer();
    ~IndexBuffer();

    bool Setup() override;
    void Term() override;
};


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
class IndexBuffer : public IDXResource<ID3D11Buffer>
{
private:


public:
    IndexBuffer();
    ~IndexBuffer();

    bool Create();
    void Release() override;
};


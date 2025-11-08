#pragma once
#include "IDXResource.h"

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:VertexBuffer Class --- */
//
//  ★継承：IDXResource ★
//
// 【?】頂点バッファをラップする
//
// ***************************************************************************************
class VertexBuffer : public IDXResource<void>
{
private:


public:
    VertexBuffer();
    ~VertexBuffer();

    bool Setup() override;
    void Term() override;
};


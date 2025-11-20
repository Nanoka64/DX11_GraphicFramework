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
class VertexBuffer : public IDXResource<ID3D11Buffer>
{
private:


public:
    VertexBuffer();
    ~VertexBuffer();
    bool Create();
    void Release() override;
};


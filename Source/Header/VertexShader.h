#pragma once
#include "IDXResource.h"

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:VertexShader Class --- */
//
//  ★継承：IDXResource ★
//
// 【?】頂点シェーダデータをラップする
//      ShaderManagerのみがこのクラスの所有権を持つ
//
// ***************************************************************************************

class VertexShader : public IDXResource<ID3D11VertexShader>
{
private:
    //ID3D11VertexShader *m_pVertexShader;  // 頂点シェーダ


public:
    VertexShader();
    ~VertexShader();


    bool Create();
    void Release() override;
};


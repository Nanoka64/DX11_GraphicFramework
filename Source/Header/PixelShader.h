#pragma once
#include "IDXResource.h"

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:PixelShader Class --- */
//
//  ★継承：IDXResource ★
//
// 【?】ピクセルシェーダデータをラップする
//      ShaderManagerのみがこのクラスの所有権を持つ
//
// ***************************************************************************************
class PixelShader : public IDXResource<ID3D11PixelShader>
{
private:
    //ID3D11PixelShader *m_pPixelShader;   // ピクセルシェーダ

public:
    PixelShader();
    ~PixelShader();

    bool Create();
    void Release() override;
};


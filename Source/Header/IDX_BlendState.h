#pragma once
#include "IDXResource.h"


// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:IDX_BlendState Class --- */
//  
// šŒp³FIDXResource š
//
// y?zID3D11BlendState‚ğƒ‰ƒbƒv‚·‚é
//
// ***************************************************************************************
class IDX_BlendState : public IDXResource<ID3D11BlendState>
{
private:


public:
    IDX_BlendState();
    ~IDX_BlendState();

    bool Setup() override;
    void Term() override;
};


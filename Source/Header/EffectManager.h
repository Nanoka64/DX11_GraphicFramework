#pragma once

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:EffectManager Class --- */
//
//  ★★★シングルトン★★★
//
// 【?】エフェクトの管理
//      Effekseerのエフェクトの読み込み、再生等を行う
// 
// ***************************************************************************************
class EffectManager
{
private:


public:
    EffectManager();
    ~EffectManager();

    bool Setup();

private:
    // コピー禁止
    EffectManager(const EffectManager &) = delete;
    EffectManager &operator=(const EffectManager &) = delete;

};


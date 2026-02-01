#pragma once

//#include <Effekseer.h>
//#include <EffekseerRendererDX11.h>


// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:EffectManager Class --- */
//
//  ★★★シングルトン★★★
//
// 【?】エフェクトの管理
//      Effekseerのエフェクトの読み込み、再生等を行う
// 
// 参考サイト：https://github.com/effekseer/Effekseer/blob/master/docs/Help_Cpp/18x/Guide_Cpp_Ja.md
//
// ***************************************************************************************
class EffectManager
{
private:
    //::Effekseer::ManagerRef m_EfkManager;
    //std::unordered_map<std::string, Effekseer::EffectRef> m_EffectMap;

public:
    EffectManager();
    ~EffectManager();

    bool Setup(RendererEngine& renderer);
    void Update(RendererEngine& renderer);

private:
    // コピー禁止
    EffectManager(const EffectManager &) = delete;
    EffectManager &operator=(const EffectManager &) = delete;

};


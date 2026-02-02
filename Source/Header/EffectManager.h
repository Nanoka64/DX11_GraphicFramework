#pragma once

#include <Effekseer.h>
#include <EffekseerRendererDX11.h>

#define EFFECT_MAX 100
#define PERTICLE_BUFFER 8000

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
    ::Effekseer::ManagerRef m_EfkManager;
    ::EffekseerRenderer::RendererRef m_EfkRenderer;
    Effekseer::EffectRef m_EffectArray[EFFECT_MAX];
    Effekseer::Handle m_HandleArray[EFFECT_MAX];
    int m_EffectIndex;

public:
    EffectManager();
    ~EffectManager();

    bool Setup(RendererEngine& renderer);           // 初期化
    void UpdateEffect(RendererEngine& renderer);    // エフェクトの更新
    void DrawEffect();                              // エフェクトの描画
    int LoadEffect(const char16_t *name);           // エフェクトの読み込み
    void PlayEffect(int handle);                    // エフェクトの再生
    void StopEffect(int handle);                    // エフェクトを止める
    void SetPositionEffect(int handle, float x, float y, float z);  // エフェクトの位置を設定
    void SetScaleEffect(int handle, float x, float y, float z);     // エフェクトの大きさを設定
    void SetRotationEffect(int handle, float x, float y, float z);  // エフェクトの回転地を設定
    bool IsPlayingEffect(int handle);                               // エフェクトの再生状態を取得
private:
    // コピー禁止
    EffectManager(const EffectManager &) = delete;
    EffectManager &operator=(const EffectManager &) = delete;

};


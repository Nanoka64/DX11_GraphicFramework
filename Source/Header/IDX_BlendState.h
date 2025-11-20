#pragma once
#include "IDXResource.h"


//* =========================================================================
//* - @:BlendSetupDataInfo構造体 - */
//* 【?】ブレンドステートの作成データ
//       D3D11_BLEND_DESCのラップデータ
//* =========================================================================
struct BlendSetupDataInfo
{
    signed char independentBlendEnable;             // レンダリングターゲット番号(-1ならターゲットごとに設定を独立させない)
    BOOL        alphaToCoverageEnable;              // α値をカバレッジマスクに変換するか(高密度の葉のようなものに使用するといいらしい？)

    /// <summary>
    /// レンダリングターゲットごとの設定
    /// 独立設定にしていなければrenderTarget[0]のみ有効
    /// </summary>
    struct RT_BlendDataInfo {
        BOOL            blendEnable;                // ブレンドするかどうか
        D3D11_BLEND     srcBlend;                   // ソース             新しいピクセルの色に適用するブレンド係数
        D3D11_BLEND     destBlend;                  // デスティネーション 既存のピクセルの色に適用するブレンド係数
        D3D11_BLEND_OP  blendOp;                    // srcとdestの色を結合する演算方法
        D3D11_BLEND     srcBlendAlpha;              // アルファブレンドでソース            のアルファ値に適用するブレンド係数
        D3D11_BLEND     destBlendAlpha;             // アルファブレンドでデスティネーションのアルファ値に適用するブレンド係数
        D3D11_BLEND_OP  blendOpAlpha;               // アルファブレンドのsrcとdestの色を結合する演算方法
        UINT8           renderTargetWriteMask;      // RTに対する色の書き込みに関する情報（？）
    }renderTargets[8];
};

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:IDX_BlendState Class --- */
//  
// ★継承：IDXResource ★
//
// 【?】ID3D11BlendStateをラップする
//
// ***************************************************************************************
class IDX_BlendState : public IDXResource<ID3D11BlendState>
{
private:
    bool m_IsIndependentBlendEnable;    // レンダリングターゲットごとに独立させるかどうか

public:
    IDX_BlendState();
    ~IDX_BlendState();

    bool Create(RendererManager& renderer,const BlendSetupDataInfo& _InData);
    void Release() override;
};


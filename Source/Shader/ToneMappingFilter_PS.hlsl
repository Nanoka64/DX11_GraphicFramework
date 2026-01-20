// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//
/* - @:ピクセルシェーダ -*/
//
//  【?】HDR対応のためのトーンマッピングを行う
//      一旦、ACES (Academy Color Encoding System)というのを使用してみる
//      映画風のシネマティックな感じらしい（ 見てみたらEDF4みたいな感じだった ）
//
// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
#pragma once
SamplerState g_sSampler : register(s0);
Texture2D g_tTexture0 : register(t0); 


/* =========================================================================
/* - @:入力構造体 - */
/* =========================================================================*/
struct PS_IN
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR;
    float2 UV : TEXCOORD;
};

// ACESトーンマッピングの近似式
float3 ACESToneMapping(float3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return saturate((x * (a * x + b)) / (x * (c * x + d) + e));
}

// **************************************************************************
/* - @:エントリーポイント - */
// **************************************************************************
float4 PSMain(PS_IN input) : SV_TARGET
{
    float3 hdrColor = g_tTexture0.Sample(g_sSampler, input.UV).rgb;
    float3 finalColor = float3(0, 0, 0);
    
    // トーンマッピング処理
    finalColor = ACESToneMapping(hdrColor);
    
    
    // リニア空間：現実的な正しい世界
    // ガンマ空間：モニタが表示する少し暗い世界
    
    // ガンマ補正
    // ガンマ値 2.2というものがあり、モニタの特性として、
    // そのままだと本来よりも暗くなってしまう。
    // 1.0 / 2.2をすることで0.4545...ガンマの逆数となり、
    // この累乗を計算することで正しい明るさで表示される......らしい。
    finalColor = pow(finalColor, 1.0f / 2.2f);
    
    return float4(finalColor, 1.0f);;
}
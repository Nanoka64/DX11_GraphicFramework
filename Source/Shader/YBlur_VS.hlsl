// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//
/* - @:頂点シェーダ -*/
//
//  【?】ガウシアンブラー
//
// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

SamplerState g_sSampler : register(s0);
Texture2D g_tTexture : register(t0);

/* =========================================================================
/* - @:スプライト入力構造体 - */
/* =========================================================================*/
struct VS_IN
{
    float3 Pos : POSITION;  // 頂点座標
    float2 UV : TEXCOORD;   // テクスチャ座標
    float4 Color : COLOR;   // 頂点色
    float3 Normal : NORMAL; // 法線（cpp側の都合で入れているが基本使わない）
};

/* =========================================================================
/* - @:スプライト出力構造体 - */
/* =========================================================================*/
struct VS_OUT
{
    float4 pos : SV_POSITION;
    float4 tex0 : TEXCOORD0;
    float4 tex1 : TEXCOORD1;
    float4 tex2 : TEXCOORD2;
    float4 tex3 : TEXCOORD3;
    float4 tex4 : TEXCOORD4;
    float4 tex5 : TEXCOORD5;
    float4 tex6 : TEXCOORD6;
    float4 tex7 : TEXCOORD7;
};


// **************************************************************************
/* - @:エントリーポイント - */
// **************************************************************************
VS_OUT VSMain(VS_IN input)
{
    VS_OUT output;
    
    output.pos.xyz = float4(input.Pos.xyz, 1.0f);
    output.pos.w = 1.0f;
    
    // テクスチャのサイズを取得
    float2 texSize;
    float level;
    g_tTexture.GetDimensions(0, texSize.x, texSize.y, level);

    // 基準テクセルのUV座標を記録
    float2 tex = input.UV;
    
    // 基準テクセルからV座標を+1テクセルずらすためのオフセットを計算する
    output.tex0.xy = float2(0.0f, 1.0f / texSize.y);
    
    // 基準テクセルからV座標を+3テクセルずらすためのオフセットを計算する
    output.tex1.xy = float2(0.0f, 3.0f / texSize.y);
    
    // 基準テクセルからV座標を+5テクセルずらすためのオフセットを計算する
    output.tex2.xy = float2(0.0f, 5.0f / texSize.y);
    
    // 基準テクセルからV座標を+7テクセルずらすためのオフセットを計算する
    output.tex3.xy = float2(0.0f, 7.0f / texSize.y);
    
    // 基準テクセルからV座標を+9テクセルずらすためのオフセットを計算する
    output.tex4.xy = float2(0.0f, 9.0f / texSize.y);
    
    // 基準テクセルからV座標を+11テクセルずらすためのオフセットを計算する
    output.tex5.xy = float2(0.0f, 11.0f / texSize.y);
    
    // 基準テクセルからV座標を+13テクセルずらすためのオフセットを計算する
    output.tex6.xy = float2(0.0f, 13.0f / texSize.y);
    
    // 基準テクセルからV座標を+15テクセルずらすためのオフセットを計算する
    output.tex7.xy = float2(0.0f, 15.0f / texSize.y);
    
    // オフセットに-1を掛けてマイナス方向のオフセットを計算する
    output.tex0.zw = output.tex0.xy * -1.0f;
    output.tex1.zw = output.tex1.xy * -1.0f;
    output.tex2.zw = output.tex2.xy * -1.0f;
    output.tex3.zw = output.tex3.xy * -1.0f;
    output.tex4.zw = output.tex4.xy * -1.0f;
    output.tex5.zw = output.tex5.xy * -1.0f;
    output.tex6.zw = output.tex6.xy * -1.0f;
    output.tex7.zw = output.tex7.xy * -1.0f;
    
    // オフセットに基準テクセルのUV座標を足し算して、
    // 実際にサンプリングするUV座標を変換する
    output.tex0 += float4(tex,tex);
    output.tex1 += float4(tex,tex);
    output.tex2 += float4(tex,tex);
    output.tex3 += float4(tex,tex);
    output.tex4 += float4(tex,tex);
    output.tex5 += float4(tex,tex);
    output.tex6 += float4(tex,tex);
    output.tex7 += float4(tex,tex);
    
    return output;
}
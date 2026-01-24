#pragma once


struct SimpleVertex     // 頂点フォーマット
{
    DirectX::XMFLOAT3 Pos;       // x, y, z 座標
    DirectX::XMFLOAT2 Tex;       // x, y    UV値
};

// *******************************************************************
/* 透視投影用定数バッファ */
// *******************************************************************
struct CB_PROJECTION
{
    DirectX::XMFLOAT4X4 Projection;	    // 透視投影行列
    DirectX::XMFLOAT4X4 InvProjection;  // 逆行列
};

struct CB_PROJECTION_SET
{
    CB_PROJECTION Data={};
    ID3D11Buffer* pBuff = nullptr;
};

// *******************************************************************
/* ビュー変換用定数バッファ */
// *******************************************************************
struct CB_VIEW
{
    DirectX::XMFLOAT4X4 View;

    // ビュープロジェクション行列の逆行列
    DirectX::XMFLOAT4X4 ViewProjInvMatrix;
};

struct CB_VIEW_SET
{
    CB_VIEW Data = {};
    ID3D11Buffer* pBuff = nullptr;
};

// *******************************************************************
/* ワールド変換用定数バッファ */
// *******************************************************************
struct CB_TRANSFORM{
    DirectX::XMFLOAT4X4 WorldMtx;  // ワールド変換行列
};

struct CB_TRANSFORM_SET{
    CB_TRANSFORM Data = {};
    ID3D11Buffer* pBuff = nullptr;
};


// *******************************************************************
/* ボーン変換用定数バッファ */
// *******************************************************************
struct CB_BONES_DATA
{
    DirectX::XMFLOAT4X4 BonesMatrices[128];   // 最大8本のボーンに対応 シェーダ側と同じに
};

struct CB_BONES_DATA_SET
{
    CB_BONES_DATA Data = {};
    ID3D11Buffer* pBuff = nullptr;
};

// *******************************************************************
/* ディレクションライト用定数バッファ */
// *******************************************************************

/// <summary>
/// ディレクションライト情報
/// </summary>
struct CB_DIRECTION_LIGHT
{
    DirectX::XMFLOAT3 Direction;    // 方向
    float Padding1;
    
    DirectX::XMFLOAT3 DiffuseColor; // ディフューズ色
    float DiffuseIntensity;         // ディフューズ光度

    DirectX::XMFLOAT3 SpecularColor;// スペキュラ色
    float SpecularIntensity;        // スペキュラ強度

    // ライトから見た画面座標
    DirectX::XMFLOAT4X4 LightViewProj;

    // TODO:ここに置くと色々ずれるので場所変える
    DirectX::XMFLOAT3 EyePos;       // 視点の位置
    float Padding4;

};


/// <summary>
/// ポイントライト
/// </summary>
struct CB_POINT_LIGHT
{
    DirectX::XMFLOAT3 Pos;          // 座標
    float Range;                    // 影響範囲
    
    DirectX::XMFLOAT3 DiffuseColor; // ディフューズ色
    float DiffuseIntensity;         // ディフューズ光度
    
    DirectX::XMFLOAT3 SpecularColor;// スペキュラ色
    float SpecularIntensity;        // スペキュラ強度
};


/// <summary>
/// ディレクションライト 
/// バッファセット用
/// </summary>
struct CB_DIRECTION_LIGHT_SET
{
    CB_DIRECTION_LIGHT Data[DIRECTIONLIGHT_MAX_NUM] {};
    ID3D11Buffer *pBuff = nullptr;
};

/// <summary>
/// ポイントライト 
/// バッファセット用
/// </summary>
struct CB_POINT_LIGHT_SET
{
    CB_POINT_LIGHT Data[POINTLIGHT_MAX_NUM]{}; // 500個
    ID3D11Buffer *pBuff = nullptr;
};


// *******************************************************************
/* 定数バッファに渡すためのマテリアル  */
// *******************************************************************
struct CB_MATERIAL{
    DirectX::XMFLOAT4 Diffuse;    // ディフューズ反射光

	DirectX::XMFLOAT4 Specular;   // スペキュラ反射光
	
    float SpecularPower;          // スペキュラの強さ
    float EmissivePower;          // 自己発光の強さ（ブルーム強度）
    DirectX::XMFLOAT2 OffsetUV;   // UVオフセット（エネルギーの流れる感じとかの表現に使えそう ヘクトルみたいな）

    DirectX::XMFLOAT3 EmissiveColor;    // 発光カラー
    float pad;

	//float Padding[2];             // パディング(16バイトアラインメント ※詳細はUtilityHeader.hlsli側を見て)
};

struct CB_MATERIAL_SET{
    CB_MATERIAL Data = {};
    ID3D11Buffer* pBuff = nullptr;
};

// *******************************************************************
/* ワールド変換とマテリアル情報をまとめた定数バッファ 
    ※使わないかも？ */
// *******************************************************************
struct CB_OBJECT3D_DATA{
    CB_TRANSFORM Transform;      // ワールド変換行列
    CB_MATERIAL Material;        // マテリアル情報
};

struct CB_OBJECT3D_DATA_SET{
	CB_OBJECT3D_DATA Data = {};
	ID3D11Buffer* pBuff = nullptr;
};


// *******************************************************************
/* ユーザー拡張 */
// *******************************************************************
struct CB_USER_EXPAND_SET{
    void * pData = nullptr;
    ID3D11Buffer* pBuff = nullptr;
    int Size = 0;
    int Slot = -1;
};

// *******************************************************************
/* ＣＰＵ用 ３Ｄ情報 */
// *******************************************************************

// ディフューズマップ
struct DIFFUSE_MAP_DATA
{
    std::weak_ptr <class Texture> Texture;
    int index = -1;
};

// スペキュラマップ
struct SPECULAR_MAP_DATA
{
    std::weak_ptr <class Texture> Texture;
    int index = -1;
};

// ノーマルマップ
struct NORMAL_MAP_DATA
{
    std::weak_ptr <class Texture> Texture;
    int index = -1;
};


//* =========================================================================
//* - @:BLEND_MODE列挙体 - */
//* 【?】ブレンドモード
//* =========================================================================
enum class BLEND_MODE
{
    NONE = 0,       // 何もしない
    ALPHA,          // 半透明
    ALPHA_TRANS,    // 半透明合成
    ADD,            // 加算
    SUB,            // 減算

    NUM,
};



// テクスチャマップ
enum TEXTURE_MAP
{
    TEXTURE_MAP_NONE,
    TEXTURE_MAP_DIFFUSE,	// ディフューズ 拡散光
    TEXTURE_MAP_NORMAL,		// ノーマル		凹凸（法線）
    TEXTURE_MAP_SPECULAR,	// スペキュラ	反射光
};


struct InstanceData
{
    DirectX::XMMATRIX World;
    VECTOR4::VEC4 Color;
    VECTOR2::VEC2 UV;
};


#pragma once
#include "Object.h"
#include "Component_IMeshResource.h"
#include "Component_SpriteRenderer.h"
#include "Component_BillboardResource.h"


// 入力マテリアル情報
struct InputMaterial
{
    MATERIAL* pMat; // マテリアル（MatNum分）
    int MatIndex;   // マテリアル番号
};

/// <summary>
/// 定数バッファ情報
/// </summary>
struct ConstantBufferInfo
{
    int SetSlot = -1; 						    // 定数バッファ番号
    void* pUserExpandConstantBuffer = nullptr;  // VSユーザー拡張用定数バッファ
    int UserExpandConstantBufferSize = 0;       // VSユーザー拡張用定数バッファサイズ
};


/// <summary>
/// モデルの生成情報
/// </summary>
struct CreateModelInfo
{
    RendererEngine *pRenderer;      // 描画
    std::string Path;               // モデルパス
    std::string ObjTag;             // オブジェクトのタグ
    bool IsActive;                  // 生成時にオブジェクトをアクティブにするか
    bool IsAnim;                    // アニメーションするかどうか
    int InitAnimIndex;              // 最初に再生するアニメーション
    SHADER_TYPE ShaderType;         // 使用するシェーダの種類

    InputMaterial* MaterialData;    // マテリアル情報
    UINT MatNum;                    // マテリアル数


    // コンストラクタ
    CreateModelInfo() :
        pRenderer(nullptr),
        IsActive(true),
        IsAnim(false),
        InitAnimIndex(0),
        MaterialData(nullptr),
        MatNum(0),
        ShaderType(SHADER_TYPE::DEFFERD_STANDARD_SIMPLE)
    {
    }
};


/// <summary>
/// 汎用メッシュ生成情報
/// </summary>
struct CreateUtilityMeshInfo
{
    RendererEngine* pRenderer;  // 描画
    
    UTILITY_MESH_TYPE Type;      // メッシュタイプ
    SHADER_TYPE ShaderType;      // 使用するシェーダの種類

    std::string ObjTag;          // オブジェクトのタグ
    bool IsActive;               // 生成時にオブジェクトをアクティブにするか

    InputMaterial* MaterialData; // マテリアル情報
    UINT MatNum;                 // マテリアル数

    // コンストラクタ
    CreateUtilityMeshInfo():
        pRenderer(nullptr),
        IsActive(true),
        MaterialData(nullptr),
        MatNum(0),
        Type(UTILITY_MESH_TYPE::NONE),
        ShaderType(SHADER_TYPE::DEFFERD_STANDARD_SIMPLE)
    {};
};


/// <summary>
/// スプライト生成情報
/// </summary>
struct CreateSpriteInfo
{
    RendererEngine *pRenderer;          // 描画
    std::string ObjTag;                 // オブジェクトのタグ
    bool IsActive;                      // 生成時にオブジェクトをアクティブにするか
    SPRITE_USAGE_TYPE Type;             // スプライトの使用方法
    SHADER_TYPE ShaderType;             // 使用するシェーダの種類

    ConstantBufferInfo *pPSConstantBuffers;  // ピクセルシェーダ用定数バッファ
    ConstantBufferInfo *pVSConstantBuffers;  // 頂点シェーダ用定数バッファ
    int PSConstBufferNum;               // ピクセルシェーダ用定数バッファ数
    int VSConstBufferNum;               // 頂点シェーダ用定数バッファ数

    std::map<int, std::weak_ptr<class Texture>> pTextureMap;    // テクスチャマップ

    // 幅
    float Width;
    float Height;

    CreateSpriteInfo():
        pRenderer(nullptr),
        IsActive(true),
        Width(0.0f),
        Height(0.0f),
        Type(SPRITE_USAGE_TYPE::NORMAL),
        ShaderType(SHADER_TYPE::FOWARD_STANDARD_UI_SPRITE),
        pPSConstantBuffers(nullptr),
        pVSConstantBuffers(nullptr),
        PSConstBufferNum(0),
        VSConstBufferNum(0)
    {};
};


/// <summary>
/// ビルボード生成情報
/// </summary>
struct CreateBillboradInfo
{
    RendererEngine* pRenderer;   // 描画

    BILLBOARD_USAGE_TYPE Type;    // 使用方法
    FIXED_AXIS_BITFLAG FixedAxis; // 固定軸ビットフラグ（指定しない場合は固定軸なし）
    SHADER_TYPE ShaderType;       // 使用するシェーダの種類
    
    std::string ObjTag;           // オブジェクトのタグ
    bool IsActive;                // 生成時にオブジェクトをアクティブにするか
    
    InputMaterial* MaterialData;  // マテリアル情報
    UINT MatNum;                  // マテリアル数

    // コンストラクタ
    CreateBillboradInfo() :
        pRenderer(nullptr),
        IsActive(true),
        MaterialData(nullptr),
        MatNum(0),
        Type(BILLBOARD_USAGE_TYPE::SIMPLE),
        FixedAxis()
    {};
};


class MeshFactory
{
private:


public:
    static std::weak_ptr<class GameObject> CreateModel(const CreateModelInfo& info);
    static std::weak_ptr<class GameObject> CreateUtilityMesh(const CreateUtilityMeshInfo& info);
    static std::weak_ptr<class GameObject> CreateSprite(const CreateSpriteInfo &info);
    static std::weak_ptr<class GameObject> CreateBillboard(const CreateBillboradInfo& info);
}; 


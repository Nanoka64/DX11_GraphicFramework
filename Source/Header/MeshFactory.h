#pragma once
#include "Object.h"
#include "Component_IMeshResource.h"


// 入力マテリアル情報
struct InputMaterial
{
    MATERIAL* pMat; // マテリアル（MatNum分）
    int MatIndex;         // マテリアル番号
};


/// <summary>
/// モデルの生成情報
/// </summary>
struct CreateModelInfo
{
    RendererManager *pRenderer;     // 描画
    std::string Path;               // モデルパス
    std::string ObjTag;             // オブジェクトのタグ
    bool IsActive;                  // 生成時にオブジェクトをアクティブにするか
    bool IsAnim;                    // アニメーションするかどうか
    int InitAnimIndex;              // 最初に再生するアニメーション

    InputMaterial* MaterialData;    // マテリアル情報
    UINT MatNum;                    // マテリアル数


    // コンストラクタ
    CreateModelInfo() :
        pRenderer(nullptr),
        IsActive(true),
        IsAnim(false),
        InitAnimIndex(0),
        MaterialData(nullptr),
        MatNum(0)
    {
    }
};


/// <summary>
/// 汎用メッシュ生成情報
/// </summary>
struct CreateUtilityMeshInfo
{
    RendererManager* pRenderer;  // 描画
    
    UTILITY_MESH_TYPE Type;      // メッシュタイプ
    
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
        Type(UTILITY_MESH_TYPE::NONE)
    {};
};

class MeshFactory : public Object
{
private:


public:
    static std::weak_ptr<class GameObject> CreateModel(const CreateModelInfo& info);
    static std::weak_ptr<class GameObject> CreateUtilityMesh(const CreateUtilityMeshInfo& info);
};


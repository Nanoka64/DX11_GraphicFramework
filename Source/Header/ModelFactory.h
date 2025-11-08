#pragma once
#include "Object.h"

struct CreateModelInfo
{
    RendererManager *pRenderer;  // 描画
    std::string Path;    // モデルパス
    std::string ObjTag;  // オブジェクトのタグ
    bool IsActive;       // 生成時にオブジェクトをアクティブにするか
    bool IsAnim;         // アニメーションするかどうか

    // 入力マテリアル情報
    struct InputMaterial
    {
        MATERIAL *pMat; // マテリアル（MatNum分）
        int MatIndex;         // マテリアル番号
    } *MaterialData;

    UINT MatNum;   // マテリアル数


    // コンストラクタ
    CreateModelInfo() :
        pRenderer(nullptr),
        IsActive(true),
        IsAnim(false),
        MaterialData(nullptr),
        MatNum(0)
    {
    }
};


class ModelFactory : public Object
{
private:


public:
    static std::weak_ptr<class GameObject> CreateModel(const CreateModelInfo& info);

};


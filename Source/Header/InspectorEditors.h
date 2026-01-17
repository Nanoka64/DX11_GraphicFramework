#pragma once
#include "EditorBase.h"


// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:TransformEditor Class --- */
//
//  ★継承：EditorBase ★
//
// 【?】トランスフォーム編集用エディタ
//
// ***************************************************************************************
class TransformEditor : public EditorBase
{
private:
    float m_SlideAccuRate; // 位置の入力精度

public:
    TransformEditor() 
        : EditorBase(),
        m_SlideAccuRate(1.0f) 
    {};
    ~TransformEditor()
    {
    };

    bool Init(RendererEngine& renderer) override;
    void OnEditorGUI(RendererEngine &renderer, class GameObject &pObj) override;

};

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:DirectionLightEditor Class --- */
//
//  ★継承：EditorBase ★
//
// 【?】ディレクションライト編集用エディタ
//
// ***************************************************************************************
class DirectionLightEditor : public EditorBase
{
private:
    float m_SlideAccuRate; // 位置の入力精度

public:
    DirectionLightEditor()
        : EditorBase(),
        m_SlideAccuRate(1.0f)
    {
    };
    ~DirectionLightEditor()
    {
    };
    bool Init(RendererEngine &renderer) override;
    void OnEditorGUI(RendererEngine &renderer, class GameObject &pObj) override;

};



// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:PointLightEditor Class --- */
//
//  ★継承：EditorBase ★
//
// 【?】ポイントライト編集用エディタ
//
// ***************************************************************************************
class PointLightEditor : public EditorBase
{
private:
    float m_SlideAccuRate; // 位置の入力精度

public:
    PointLightEditor()
        : EditorBase(),
        m_SlideAccuRate(1.0f)
    {
    };
    ~PointLightEditor()
    {
    };
    bool Init(RendererEngine &renderer) override;
    void OnEditorGUI(RendererEngine &renderer, class GameObject &pObj) override;
};


// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:PlayerControllerEditor Class --- */
//
//  ★継承：EditorBase ★
//
// 【?】プレイヤーコントローラー編集用エディタ
//
// ***************************************************************************************
class PlayerControllerEditor : public EditorBase
{
private:
    float m_SlideAccuRate; // 位置の入力精度

public:
    PlayerControllerEditor()
        : EditorBase(),
        m_SlideAccuRate(1.0f)
    {
    };
    ~PlayerControllerEditor()
    {
    };
    bool Init(RendererEngine &renderer) override;
    void OnEditorGUI(RendererEngine &renderer, class GameObject &pObj) override;

};

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:Camera3DEditor Class --- */
//
//  ★継承：EditorBase ★
//
// 【?】カメラ3D編集用エディタ
//
// ***************************************************************************************
class Camera3DEditor : public EditorBase
{
private:

public:
    Camera3DEditor()
        : EditorBase()
    {
    };
    ~Camera3DEditor()
    {
    };
    bool Init(RendererEngine &renderer) override;
    void OnEditorGUI(RendererEngine &renderer, class GameObject &pObj) override;

};

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:SkinnedMeshAnimator Class --- */
//
//  ★継承：EditorBase ★
//
// 【?】スキンメッシュアニメーション編集用エディタ
//
// ***************************************************************************************
class SkinnedMeshAnimatorEditor : public EditorBase
{
private:

public:
    SkinnedMeshAnimatorEditor()
        : EditorBase()
    {
    };
    ~SkinnedMeshAnimatorEditor()
    {
    };
    bool Init(RendererEngine &renderer) override;
    void OnEditorGUI(RendererEngine &renderer, class GameObject &pObj) override;

};

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:ModelMeshResourceEditor Class --- */
//
//  ★継承：EditorBase ★
//
// 【?】スキンメッシュアニメーション編集用エディタ
//
// ***************************************************************************************
class ModelMeshResourceEditor : public EditorBase
{
private:

public:
    ModelMeshResourceEditor()
        : EditorBase()
    {
    };
    ~ModelMeshResourceEditor()
    {
    };
    bool Init(RendererEngine &renderer) override;
    void OnEditorGUI(RendererEngine &renderer, class GameObject &pObj) override;

};

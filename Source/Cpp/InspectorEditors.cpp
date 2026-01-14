#include "pch.h"
#include "InspectorEditors.h"
#include "RendererEngine.h"
#include "GameObject.h"
#include "Component_Transform.h"
#include "Component_DirectionalLight.h"
#include "Component_PointLight.h"
#include "Component_PlayerController.h"
#include "Component_3DCamera.h"

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:TransformEditor Class --- */
//
// ***************************************************************************************
bool TransformEditor::Init(RendererEngine &renderer)
{
    return true;
}

void TransformEditor::OnEditorGUI(RendererEngine &renderer, GameObject &pObj)
{
    using namespace VECTOR3;
    using namespace Tool;

    auto pTransform = pObj.get_Component<Transform>();

    if (pTransform == nullptr)
    {
        MessageBoxA(NULL, "Transformコンポーネントがアタッチされていません。", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    VEC3 pos = pTransform->get_VEC3ToPos();
    VEC3 rot = pTransform->get_VEC3ToRotateToDeg();
    VEC3 scl = pTransform->get_VEC3ToScale();

    // beginはInspectorWindowで行っている

    // ノード
    if (Master::m_pDebugger->DG_TreeNode(U8ToChar(u8"トランスフォーム")))
    {
        Master::m_pDebugger->DG_Separator();    // 区切り線
        Master::m_pDebugger->DG_BulletText(U8ToChar(u8"座標"));
        Master::m_pDebugger->DG_SameLine();
        Master::m_pDebugger->DG_DragVec3("##Pos", &pos, m_SlideAccuRate, -100000.0f, 100000.0f);


        Master::m_pDebugger->DG_BulletText(U8ToChar(u8"回転"));
        Master::m_pDebugger->DG_SameLine();
        Master::m_pDebugger->DG_DragVec3("##Rot", &rot, m_SlideAccuRate, -100000.0f, 100000.0f);


        Master::m_pDebugger->DG_BulletText(U8ToChar(u8"拡縮"));
        Master::m_pDebugger->DG_SameLine();
        Master::m_pDebugger->DG_DragVec3("##Scale", &scl, m_SlideAccuRate, -100000.0f, 100000.0f);

        Master::m_pDebugger->DG_SliderFloat(U8ToChar(u8"編集スライド精度"), 1, &m_SlideAccuRate, 0.001f, 3.0f);

        Master::m_pDebugger->DG_TreePop();
    }

    // Transformに反映
    pTransform->set_Pos(pos);
    pTransform->set_RotateToDeg(rot);
    pTransform->set_Scale(scl);
}



// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:DirectionLightEditor Class --- */
//
// ***************************************************************************************
bool DirectionLightEditor::Init(RendererEngine &renderer)
{
    return true;
}

void DirectionLightEditor::OnEditorGUI(RendererEngine &renderer, GameObject &pObj)
{
    using namespace VECTOR3;
    using namespace Tool;

    // ディレクショナルライトコンポーネントの取得
    auto pDirectionLig = pObj.get_Component<DirectionalLight>();

    if (pDirectionLig == nullptr)
    {
        return;
    }

    // beginはInspectorWindowで行っている
    float intensity = pDirectionLig->get_Intensity();
    VEC3 lightColor = pDirectionLig->get_LightColor();

    // ノード
    if (Master::m_pDebugger->DG_TreeNode(U8ToChar(u8"ディレクションライト")))
    {
        Master::m_pDebugger->DG_Separator();    // 区切り線
        Master::m_pDebugger->DG_BulletText(U8ToChar(u8"強さ"));
        Master::m_pDebugger->DG_SameLine();
        Master::m_pDebugger->DG_SliderFloat("##Insensity", 1, &intensity, 0.0f, 100.0f);

        Master::m_pDebugger->DG_BulletText(U8ToChar(u8"カラー"));
        Master::m_pDebugger->DG_SameLine();
        Master::m_pDebugger->DG_ColorEdit3("##DirLigColor", &lightColor);

        Master::m_pDebugger->DG_TreePop();
    }

    // 反映
    pDirectionLig->set_Intensity(intensity);
    pDirectionLig->set_LightColor(lightColor);
}


// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:PointLightEditor Class --- */
//
// ***************************************************************************************
bool PointLightEditor::Init(RendererEngine &renderer)
{
    return true;
}

void PointLightEditor::OnEditorGUI(RendererEngine &renderer, GameObject &pObj)
{
    using namespace VECTOR3;
    using namespace Tool;

    // ディレクショナルライトコンポーネントの取得
    auto pPointLig = pObj.get_Component<PointLight>();

    if (pPointLig == nullptr)
    {
        return;
    }

    // beginはInspectorWindowで行っている
    float intensity = pPointLig->get_Intensity();
    float range = pPointLig->get_Range();
    VEC3 lightColor = pPointLig->get_LightColor();

    // ノード
    if (Master::m_pDebugger->DG_TreeNode(U8ToChar(u8"ポイントライト")))
    {
        Master::m_pDebugger->DG_Separator();    // 区切り線
        Master::m_pDebugger->DG_BulletText(U8ToChar(u8"強さ"));
        Master::m_pDebugger->DG_SameLine();
        Master::m_pDebugger->DG_SliderFloat("##Insensity", 1, &intensity, 0.0f, 100.0f);

        Master::m_pDebugger->DG_BulletText(U8ToChar(u8"範囲"));
        Master::m_pDebugger->DG_SameLine();
        Master::m_pDebugger->DG_SliderFloat("##Range", 1, &range, 0.0f, 10000.0f);

        Master::m_pDebugger->DG_BulletText(U8ToChar(u8"カラー"));
        Master::m_pDebugger->DG_SameLine();
        Master::m_pDebugger->DG_ColorEdit3("##DirLigColor", &lightColor);

        Master::m_pDebugger->DG_TreePop();
    }

    // 反映
    pPointLig->set_Intensity(intensity);
    pPointLig->set_LightColor(lightColor);
    pPointLig->set_Range(range);
}



// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:PlayerControllerEditor Class --- */
//
// ***************************************************************************************
bool PlayerControllerEditor::Init(RendererEngine &renderer)
{
    return true;
}

void PlayerControllerEditor::OnEditorGUI(RendererEngine &renderer, GameObject &pObj)
{
    using namespace VECTOR3;
    using namespace Tool;

    // ディレクショナルライトコンポーネントの取得
    auto pComp = pObj.get_Component<PlayerController>();

    if (pComp == nullptr)
    {
        return;
    }

    // beginはInspectorWindowで行っている
    float moveSpeed             = pComp->get_MoveSpeed();
    PLAYER_ANIMATION_ID animId  = pComp->get_AnimID();
    VEC3 moveVelocity           = pComp->get_MoveVelocity();
    bool isJump                 = pComp->get_IsJump();
    float jumpFoce              = pComp->get_JumpFoce();
    float gravity               = pComp->get_Gravity();

    // ノード
    if (Master::m_pDebugger->DG_TreeNode(U8ToChar(u8"プレイヤーコントローラー")))
    {
        Master::m_pDebugger->DG_Separator();    // 区切り線

        Master::m_pDebugger->DG_BulletText(U8ToChar(u8"移動速度"));
        Master::m_pDebugger->DG_SameLine();
        Master::m_pDebugger->DG_SliderFloat("##MoveSpeed", 1, &moveSpeed, 0.0f, 100.0f);

        Master::m_pDebugger->DG_BulletText(U8ToChar(u8"速度ベクトル"));
        Master::m_pDebugger->DG_SameLine();
        Master::m_pDebugger->DG_DragVec3("##Velocity", &moveVelocity, 0.0f, -10000, 10000);

        Master::m_pDebugger->DG_BulletText(U8ToChar(u8"再生中のアニメーション"));
        Master::m_pDebugger->DG_TextValue("ID : %d",static_cast<int>(animId));
        Master::m_pDebugger->DG_TextValue(U8ToChar(u8"名前 : %s"), g_PlayerAnimationNames[static_cast<int>(animId) + 1].c_str());

        Master::m_pDebugger->DG_BulletText(U8ToChar(u8"ジャンプフラグ"));
        Master::m_pDebugger->DG_SameLine();
        Master::m_pDebugger->DG_RadioButton("##JumpFlag", isJump);

        Master::m_pDebugger->DG_BulletText(U8ToChar(u8"ジャンプ力"));
        Master::m_pDebugger->DG_SameLine();
        Master::m_pDebugger->DG_DragFloat("##JumpFoce", 1, &jumpFoce,0.01f, 0.0f, 100.0f);

        Master::m_pDebugger->DG_BulletText(U8ToChar(u8"重力"));
        Master::m_pDebugger->DG_Spacing();
        Master::m_pDebugger->DG_Spacing();
        Master::m_pDebugger->DG_SameLine();
        Master::m_pDebugger->DG_DragFloat("##Gravity", 1, &gravity, 0.01f, 0.0f, 10.0f);

        Master::m_pDebugger->DG_TreePop();
    }

    // 反映
    pComp->set_MoveSpeed(moveSpeed);
    pComp->set_AnimID(animId);
    pComp->set_JumpFoce(jumpFoce);
    pComp->set_Gravity(gravity);
}


// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:PlayerControllerEditor Class --- */
//
// ***************************************************************************************
bool Camera3DEditor::Init(RendererEngine &renderer)
{
    return true;
}

void Camera3DEditor::OnEditorGUI(RendererEngine &renderer, GameObject &pObj)
{
    using namespace VECTOR3;
    using namespace Tool;

    // ディレクショナルライトコンポーネントの取得
    auto pComp = pObj.get_Component<Camera3D>();

    if (pComp == nullptr)
    {
        return;
    }

    // beginはInspectorWindowで行っている

    float angle_H = pComp->get_Angle_H();
    float angle_V = pComp->get_Angle_V();
    float fov = pComp->get_Fov();
    float nearClip = pComp->get_Near();
    float farClip = pComp->get_Far();
    std::string focusObjTag = pComp->get_FocusObjectTag();
    VEC3 focusPoint = pComp->get_FocusPoint();
    VEC3 posOffset = pComp->get_PosOffset();
    VEC3 focusOffset = pComp->get_FocusOffset();

    // ノード
    if (Master::m_pDebugger->DG_TreeNode(U8ToChar(u8"3Dカメラ")))
    {
        Master::m_pDebugger->DG_Separator();    // 区切り線

        Master::m_pDebugger->DG_BulletText(U8ToChar(u8"視野"));
        Master::m_pDebugger->DG_SameLine();
        Master::m_pDebugger->DG_SliderFloat("##Fov", 1, &fov, 0.1f, 180.0f);

        Master::m_pDebugger->DG_BulletText(U8ToChar(u8"手前クリップ"));
        Master::m_pDebugger->DG_SameLine();
        Master::m_pDebugger->DG_SliderFloat("##NearClip", 1, &nearClip, 0.0f, 600.0f);
        Master::m_pDebugger->DG_BulletText(U8ToChar(u8"奥側クリップ"));
        Master::m_pDebugger->DG_SameLine();
        Master::m_pDebugger->DG_SliderFloat("##FarClip", 1, &farClip,  0.0f, 30000.0f);

        Master::m_pDebugger->DG_BulletText(U8ToChar(u8"水平アングル"));
        Master::m_pDebugger->DG_SameLine();
        Master::m_pDebugger->DG_SliderFloat("##AngleH", 1, &angle_H, -5.0f, 5.0f);
        Master::m_pDebugger->DG_BulletText(U8ToChar(u8"垂直アングル"));
        Master::m_pDebugger->DG_SameLine();
        Master::m_pDebugger->DG_SliderFloat("##AngleV", 1, &angle_V, -5.0f, 5.0f);

        Master::m_pDebugger->DG_BulletText(U8ToChar(u8"注視点"));
        Master::m_pDebugger->DG_SameLine();
        Master::m_pDebugger->DG_DragVec3("##Focus:", &focusPoint, 0.1f, -10000.0f, 10000.0f);

        Master::m_pDebugger->DG_BulletText(U8ToChar(u8"フォーカスしているオブジェクトのタグ："));
        Master::m_pDebugger->DG_SameLine();
        Master::m_pDebugger->DG_Text(focusObjTag.c_str());

        Master::m_pDebugger->DG_BulletText(U8ToChar(u8"オフセット座標"));
        Master::m_pDebugger->DG_DragVec3("##ofsP", &posOffset, 1.0f, 10.0f, 1000.0f);

        Master::m_pDebugger->DG_BulletText(U8ToChar(u8"オフセット注視点"));
        Master::m_pDebugger->DG_DragVec3("##ofsF", &focusOffset, 1.0f, -1000.0f, 1000.0f);

        Master::m_pDebugger->DG_TreePop();
    }

    /*
    * 補正
    */
    if (nearClip < 0.0f)
    {
        nearClip = 1.0f;
    }
    if (farClip < nearClip)
    {
        farClip = nearClip + 200.0f;    // 同じだとダメなので補正値足す
    }
    if (fov < 0.0f)
    {
        fov = 1.0f;
    }

    // 反映
    pComp->set_Angle_H(angle_H);
    pComp->set_Angle_V(angle_V);
    pComp->set_FocusPoint(focusPoint);
    pComp->set_PosOffset(posOffset);
    pComp->set_FocusOffset(focusOffset);
    pComp->set_Fov(fov);
    pComp->set_Near(nearClip);
    pComp->set_Far(farClip);
}

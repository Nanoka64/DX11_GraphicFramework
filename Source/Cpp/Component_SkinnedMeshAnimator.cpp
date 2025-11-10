#include "pch.h"
#include "Component_SkinnedMeshAnimator.h"
#include "Component_ModelMeshResource.h"
#include "RendererManager.h"

using namespace DirectX;
using namespace BASE_VERTEX;
using namespace Tool::UV;

SkinnedMeshAnimator::SkinnedMeshAnimator(std::weak_ptr<GameObject> pOwner, int updateRank) : IComponent(pOwner, updateRank),
m_AnimationTime(0.0),
m_CurrentAnimIndex(0),
m_IsAnimationFlag(false),
m_AnimProcTime(0.0),
m_ConstanrBufferBonesData(nullptr)
{
    this->set_Tag("SkinnedMeshAnimator");
}


SkinnedMeshAnimator::~SkinnedMeshAnimator()
{

}


//*---------------------------------------------------------------------------------------
//* @:SkinnedMeshAnimator Class 
//*【?】初期化
//* 引数：1.RendererManager 
//* 返値：void
//*----------------------------------------------------------------------------------------
void SkinnedMeshAnimator::Init(RendererManager &renderer)
{
    m_NodeList      = m_pMeshResource.lock()->get_ModelData().lock()->get_NodeList();
    m_BoneList      = m_pMeshResource.lock()->get_ModelData().lock()->get_BoneList();
    m_BoneIndexMap  = m_pMeshResource.lock()->get_ModelData().lock()->get_BoneIndexMap();
    m_Animations    = m_pMeshResource.lock()->get_ModelData().lock()->get_Animations();
    m_ConstanrBufferBonesData = m_pMeshResource.lock()->get_ModelData().lock()->GetConstantBufferBonesData();
}


//*---------------------------------------------------------------------------------------
//* @:SkinnedMeshAnimator Class 
//*【?】更新
//* 引数：1.RendererManager 
//* 返値：void
//*----------------------------------------------------------------------------------------
void SkinnedMeshAnimator::Draw(RendererManager& renderer)
{
    Debugger::Instance().BeginDebugWindow(m_pOwner.lock()->get_Tag() + " ---AnimInfo");
    Debugger::Instance().DG_TextValue("BoneNum: %d", m_BoneList.size());    // ボーン数
    Debugger::Instance().DG_TextValue("NodeNum: %d", m_NodeList.size());    // ノード数
    Debugger::Instance().DG_TextValue("AnimNum: %d", m_Animations.size());  // アニメーション数
    Debugger::Instance().DG_CheckBox("IsAnimation##" + m_pOwner.lock()->get_Tag(), &m_IsAnimationFlag);    // アニメーション再生するか
    Debugger::Instance().DG_SliderInt("AnimationIndex##" + m_pOwner.lock()->get_Tag(), 1, &m_CurrentAnimIndex, 0, m_Animations.size() - 1);    // アニメーションインデックス
    Debugger::Instance().EndDebugWindow();

    m_AnimProcTime += 0.023f;
    BoneTransformsUpdate(renderer, m_AnimProcTime);
}


//*---------------------------------------------------------------------------------------
//* @:MeshRenderer Class 
//*【?】IMeshResource参照用のポインタ設定
//* 引数：1.IMeshResource
//* 返値：void
//*----------------------------------------------------------------------------------------
void SkinnedMeshAnimator::set_MeshResource(std::weak_ptr<class ModelMeshResource> meshResource)
{
    m_pMeshResource = meshResource;
}


//*---------------------------------------------------------------------------------------
//* @:SkinnedMeshAnimator Class 
//*【?】ボーンの更新
// 
//* 引数：1.RendererManager
//* 引数：2.秒単位の時間
//* 返値：void
//*----------------------------------------------------------------------------------------
void SkinnedMeshAnimator::BoneTransformsUpdate(RendererManager &renderer, float timeInSeconds)
{
    float tickPerSecond = 0.0f;
    float timeInTicks = 0.0f;
    float animTimeTicks = 0.0f;

    // アニメーションがあるなら ***********************************
    if (!m_Animations.empty()) {
        tickPerSecond =
            static_cast<float>(
                m_Animations[m_CurrentAnimIndex]->TicksPerSecond != 0 ?     // ティック数が0じゃないならそのまま代入
                m_Animations[m_CurrentAnimIndex]->TicksPerSecond : 25.0f    // 0なら25.0を仮として入れる
                );

        // ティック単位の時間を求める
        /*  ティック単位時間 ＝ 秒単位時間 ｘ ティック数 */
        timeInTicks = timeInSeconds * tickPerSecond;


        // アニメーションの現在の再生時間を剰余演算で求める
        /* 現在の再生時間 ＝ ティック単位時間 ％ アニメーションの持続時間 */
        animTimeTicks = static_cast<float>(fmod(timeInTicks, m_Animations[m_CurrentAnimIndex]->Duration));
    }

    // ボーン変換
    TransformBone(animTimeTicks, 0, XMMatrixIdentity());

    auto pContext = renderer.get_DeviceContext();

    // 定数バッファに詰め込む
    for (size_t i = 0; i < ARRAYSIZE(m_ConstanrBufferBonesData->Data.BonesMatrices); i++)
    {
        if (m_BoneList.size() <= i)break;
        m_ConstanrBufferBonesData->Data.BonesMatrices[i] = m_BoneList[i].FinalTransformation;
    }


    // モデルシェーダに切り替え
    ShaderManager::Instance().DeviceToSetShader(SHADER_TYPE::MODEL);


    // 定数バッファ更新
    pContext->UpdateSubresource(
        m_ConstanrBufferBonesData->pBuff,
        0,
        nullptr,
        &m_ConstanrBufferBonesData->Data,
        0,
        0
    );

    // ボーン変換用の定数バッファの更新＆セット
    pContext->VSSetConstantBuffers(3, 1, &m_ConstanrBufferBonesData->pBuff);
}


//*---------------------------------------------------------------------------------------
//* @:SkinnedMeshAnimator Class 
//*【?】ボーン変換行列の計算    ※再帰関数
// 
//* 引数：1.アニメーション単位のティック数
//* 引数：2.処理するノードインデックス
//* 引数：3.親ボーンのワールド変換行列
//* 返値：void
//*----------------------------------------------------------------------------------------
void SkinnedMeshAnimator::TransformBone(float animTimeTicks, UINT nodeIdx, const DirectX::XMMATRIX &parent)
{
    XMMATRIX localTransformMtx = XMMatrixIdentity(); // ローカル行列
    XMMATRIX worldTransformMtx = XMMatrixIdentity(); // ボーンの最終ワールド座標変換行列
    localTransformMtx = m_NodeList[nodeIdx]->Transformation; // 初期姿勢で初期化
    std::string nodeName = m_NodeList[nodeIdx]->Name;

    // アニメーションがあるなら
    if (!m_Animations.empty()) {
        const AnimationData *pAnim = m_Animations[m_CurrentAnimIndex];

        const NodeAnimChannel *pNodeAnim = FindNodeAnim(pAnim, nodeName);  // アニメーション確認

        // アニメーションがあるなら、キーフレーム間の値を補間させる（無かったら初期姿勢のまま）
        if (pNodeAnim != nullptr)
        {
            if (m_IsAnimationFlag == true)
            {
                // スケール補間 ======================================
                XMMATRIX scalingMtx = XMMatrixIdentity();
                CalcInterpolatedScaling(scalingMtx, animTimeTicks, pNodeAnim);

                // 回転補間 ======================================
                XMMATRIX RotationMtx = XMMatrixIdentity();
                CalcInterpolatedRotation(RotationMtx, animTimeTicks, pNodeAnim);

                // 位置補間 ======================================
                XMMATRIX TranslationMtx = XMMatrixIdentity();
                CalcInterpolatedPosition(TranslationMtx, animTimeTicks, pNodeAnim);

                // それぞれ合成する （スケール * 回転 * 移動）
                localTransformMtx = scalingMtx * RotationMtx * TranslationMtx;
            }
        }
    }

    /* ローカル変換 x 親のワールド行列 （サイトによって掛ける順番が変わったりする）*/
    // 親の変換行列をかけて、ワールド空間に配置するための変換行列を求める
    worldTransformMtx = localTransformMtx * parent;


    // ボーンの最終行列を計算
    if (m_BoneIndexMap.find(nodeName) != m_BoneIndexMap.end())
    {
        UINT boneIndex = m_BoneIndexMap[nodeName];

        // 
        /* スキニング用変換行列　＝　オフセット行列（初期姿勢の逆行列）ｘ ワールド行列 */
        //
        XMMATRIX mat = m_BoneList[boneIndex].OffsetMtx * worldTransformMtx;
        XMStoreFloat4x4(&m_BoneList[boneIndex].FinalTransformation, mat);   // 4*4に格納


        // デバッグ用
        m_BoneList[boneIndex].DebugWorldMatrix = worldTransformMtx;
    }

    // 再帰処理で子ノードも変換させる
    for (size_t i = 0; i < m_NodeList[nodeIdx]->ChildrenIndices.size(); i++)
    {
        UINT childIndex = m_NodeList[nodeIdx]->ChildrenIndices[i];
        TransformBone(animTimeTicks, childIndex, worldTransformMtx);
    }
}


//*---------------------------------------------------------------------------------------
//* @:SkinnedMeshAnimator Class 
//*【?】指定のアニメーション情報があるか確認する
// 
//* 引数：1.AnimationData
//* 引数：2.確認するノード名
//* 返値：NodeAnimChannel*
//*----------------------------------------------------------------------------------------
const NodeAnimChannel *SkinnedMeshAnimator::FindNodeAnim(const AnimationData *pAnim, const std::string &nodeName)
{
    for (size_t i = 0; i < pAnim->Channels.size(); i++) {
        const NodeAnimChannel *pNodeAnim = &pAnim->Channels[i];
        if (pNodeAnim->NodeName == nodeName)
        {
            return pNodeAnim;
        }
    }
    return nullptr;
}


//*---------------------------------------------------------------------------------------
//* @:SkinnedMeshAnimator Class 
//*【?】対応するキーインデックスを探す 
//      ・Position
//      ・Rotation
//      ・Scaling
// 
//* 引数：1.ティック単位アニメーション時間
//* 引数：2.確認するノードに含まれるアニメーション情報
//* 返値：UINT インデックス
//*----------------------------------------------------------------------------------------
/* Positionのキーフレームインデックスを探す */
UINT SkinnedMeshAnimator::FindPosition(float animTimeTicks, const NodeAnimChannel *pNodeAnimChannel)
{
    assert(pNodeAnimChannel->PositionKeys.size() > 0);

    for (UINT i = 0; i < pNodeAnimChannel->PositionKeys.size() - 1; i++)
    {
        double t = pNodeAnimChannel->PositionKeys[i + 1].Time;
        if (animTimeTicks < t) {
            return i;
        }
    }
    return 0;
}

/* Rotationのキーフレームインデックスを探す */
UINT SkinnedMeshAnimator::FindRotation(float animTimeTicks, const NodeAnimChannel *pNodeAnimChannel)
{
    assert(pNodeAnimChannel->RotationKeys.size() > 0);

    for (UINT i = 0; i < pNodeAnimChannel->RotationKeys.size() - 1; i++)
    {
        double t = pNodeAnimChannel->RotationKeys[i + 1].Time;
        if (animTimeTicks < t) {
            return i;
        }
    }
    return 0;
}

/* Scalingのキーフレームインデックスを探す */
UINT SkinnedMeshAnimator::FindScaling(float animTimeTicks, const NodeAnimChannel *pNodeAnimChannel)
{
    assert(pNodeAnimChannel->ScalingKeys.size() > 0);

    for (UINT i = 0; i < pNodeAnimChannel->ScalingKeys.size() - 1; i++)
    {
        double t = pNodeAnimChannel->ScalingKeys[i + 1].Time;
        if (animTimeTicks < t) {
            return i;
        }
    }
    return 0;
}


//*---------------------------------------------------------------------------------------
//* @:SkinnedMeshAnimator Class 
//*【?】現在のアニメーション時間からキーフレーム同士の補間をする
//      ・Position
//      ・Rotation
//      ・Scaling
// 
//* 引数：1.行列出力用の参照
//* 引数：2.ティック単位アニメーション時間
//* 引数：3.確認するノードに含まれるアニメーション情報
//*----------------------------------------------------------------------------------------
/* 位置のキーフレーム補間 */
void SkinnedMeshAnimator::CalcInterpolatedPosition(DirectX::XMMATRIX &out, float animTimeTicks, const NodeAnimChannel *pNodeAnimChannel)
{
    if ((pNodeAnimChannel->PositionKeys.size() < 2)) {
        VEC3 res = pNodeAnimChannel->PositionKeys[0].Value;
        out = XMMatrixTranslation(res.x, res.y, res.z);
        return;
    }

    VEC3 startPos{};
    VEC3 endPos{};
    UINT posIndex = FindPosition(animTimeTicks, pNodeAnimChannel);
    UINT nextPosIndex = posIndex + 1;
    startPos = pNodeAnimChannel->PositionKeys[posIndex].Value;          // 現在位置
    endPos = pNodeAnimChannel->PositionKeys[nextPosIndex].Value;      // 次の位置
    double t1 = pNodeAnimChannel->PositionKeys[posIndex].Time;
    double t2 = pNodeAnimChannel->PositionKeys[nextPosIndex].Time;
    double delta = t2 - t1;
    double factor = (animTimeTicks - t1) / delta;
    assert(factor >= 0.0f && factor <= 1.0f);

    // 線形補間
    VEC3 tempVec = VEC3::Lerp(startPos, endPos, factor);

    //平行移動行列を作成する
    out = XMMatrixTranslation(tempVec.x, tempVec.y, tempVec.z);
}

/* 回転のキーフレーム補間 */
void SkinnedMeshAnimator::CalcInterpolatedRotation(DirectX::XMMATRIX &out, float animTimeTicks, const NodeAnimChannel *pNodeAnimChannel)
{
    if ((pNodeAnimChannel->RotationKeys.size() < 2)) {
        aiQuaternion res = pNodeAnimChannel->RotationKeys[0].Value;
        XMVECTOR q = XMVectorSet(res.x, res.y, res.z, res.w);        // XMVECTORへ変換
        out = XMMatrixRotationQuaternion(q);        // 回転行列の作成
        return;
    }

    aiQuaternion startRot{};
    aiQuaternion endRot{};
    UINT rotIndex = FindRotation(animTimeTicks, pNodeAnimChannel);
    UINT nextRotIndex = rotIndex + 1;
    startRot = pNodeAnimChannel->RotationKeys[rotIndex].Value;      // 現在位置
    endRot = pNodeAnimChannel->RotationKeys[nextRotIndex].Value;    // 次の位置
    double t1 = pNodeAnimChannel->RotationKeys[rotIndex].Time;
    double t2 = pNodeAnimChannel->RotationKeys[nextRotIndex].Time;
    double delta = t2 - t1;
    double factor = (animTimeTicks - t1) / delta;
    assert(factor >= 0.0f && factor <= 1.0f);

    aiQuaternion res{};
    aiQuaternion::Interpolate(res, startRot, endRot, factor);
    res.Normalize();

    // XMVECTORへ変換
    XMVECTOR q = XMVectorSet(res.x, res.y, res.z, res.w);

    // 回転行列の作成
    out = XMMatrixRotationQuaternion(q);
}

/* スケールのキーフレーム補間 */
void SkinnedMeshAnimator::CalcInterpolatedScaling(DirectX::XMMATRIX &out, float animTimeTicks, const NodeAnimChannel *pNodeAnimChannel)
{
    if ((pNodeAnimChannel->ScalingKeys.size() < 2)) {
        VEC3 res = pNodeAnimChannel->ScalingKeys[0].Value;
        out = XMMatrixScaling(res.x, res.y, res.z);
        return;
    }

    VEC3 startScl{};
    VEC3 endScl{};
    UINT sclIndex = FindScaling(animTimeTicks, pNodeAnimChannel);
    UINT nextsclIndex = sclIndex + 1;
    startScl = pNodeAnimChannel->ScalingKeys[sclIndex].Value;      // 現在位置
    endScl = pNodeAnimChannel->ScalingKeys[nextsclIndex].Value;    // 次の位置
    double t1 = pNodeAnimChannel->ScalingKeys[sclIndex].Time;
    double t2 = pNodeAnimChannel->ScalingKeys[nextsclIndex].Time;
    double delta = t2 - t1;
    double factor = (animTimeTicks - t1) / delta;
    assert(factor >= 0.0f && factor <= 1.0f);

    // 線形補間
    VEC3 tempVec = VEC3::Lerp(startScl, endScl, factor);

    //スケール行列を作成する
    out = XMMatrixScaling(tempVec.x, tempVec.y, tempVec.z);
}


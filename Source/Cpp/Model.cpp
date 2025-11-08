#include "pch.h"
#include "Model.h"
#include "ModelMesh.h"
#include "RendererManager.h"
#include "ResourceManager.h"
#include "DirectWriteManager.h"
#include "Path.h"
#include "Cubu.h"

using namespace VECTOR4;
using namespace VECTOR3;
using namespace VECTOR2;
using namespace Tool;

//*---------------------------------------------------------------------------------------
//* @:Model Class 
//*【?】コンストラクタ
//* 引数：なし
//*----------------------------------------------------------------------------------------
Model::Model() :
    m_pMeshes(nullptr),
    m_MeshNum(0),
    m_Color(VEC4(1.0f, 1.0f, 1.0f, 1.0)),
    m_Position(VEC3(0.f, 0.f, 0.f)),
    m_Rotation(VEC3(0.f, 0.f, 0.f)),
    m_Scale(VEC3(1.f, 1.f, 1.f)),
    m_pParentMtx(nullptr),
    m_WorldMtx(),
    m_pScene(nullptr),
    m_GlobalInversTransformMtx(XMMatrixIdentity()),
    m_ConstanrBufferBonesData(),
    m_pCBMaterialDataSet(nullptr),
    m_pCBTransformSet(nullptr),
    m_IsAnimationFlag(true),
    m_CurrentAnimIndex(0),
    m_AnimProcTime(0.0f)
{
#ifdef _DEBUG
    m_IsDrawLine = false;
    m_IsDrawDebugBone = false;
    m_BoneScale = { 3.0f,3.0f,3.0f };
#endif
}


//*---------------------------------------------------------------------------------------
//* @:Model Class 
//*【?】デストラクタ
//* 引数：なし
//*----------------------------------------------------------------------------------------
Model::~Model()
{
    // メッシュの解放
	SAFE_DELETE_ARRAY(m_pMeshes);

    // マテリアルの解放
    if (m_pCBMaterialDataSet) {
        if (m_pCBMaterialDataSet->pBuff) {
            m_pCBMaterialDataSet->pBuff->Release();
        }
        delete m_pCBMaterialDataSet;
        m_pCBMaterialDataSet = nullptr;
    }
}
//*---------------------------------------------------------------------------------------
//* @:Model Class 
//*【?】初期化
// 
//* 引数：1.RendererManager
//* 返値：bool
//*----------------------------------------------------------------------------------------
bool Model::Init(RendererManager &renderer)
{
    return true;
}


//*---------------------------------------------------------------------------------------
//* @:Model Class 
//*【?】更新
// 
//* 引数：1.RendererManager
//* 返値：void
//*----------------------------------------------------------------------------------------
void Model::Update(RendererManager &renderer)
{

}


//*---------------------------------------------------------------------------------------
//* @:Model Class 
//*【?】描画
// 
//* 引数：1.RendererManager
//* 返値：void
//*----------------------------------------------------------------------------------------
void Model::Draw(RendererManager &renderer)
{
    auto pDeviceContext = renderer.get_DeviceContext();

    // ボーン情報表示
    if (!m_BoneList.empty())
    {
        DirectWriteManager::Instance().DrawString("ボーン情報", VEC2(1500.0f,0.0f));
        Debugger::Instance().BeginDebugWindow("*****ModelInfo*****");
        Debugger::Instance().DG_TextValue("BoneNum: %d", m_BoneList.size());    // ボーン数
        Debugger::Instance().DG_TextValue("NodeNum: %d", m_NodeList.size());    // ノード数
        Debugger::Instance().DG_TextValue("AnimNum: %d", m_Animations.size());  // アニメーション数
        Debugger::Instance().DG_TextValue("MeshNum: %d", m_MeshNum);            // メッシュ数

        Debugger::Instance().DG_CheckBox("IsAnimation", &m_IsAnimationFlag);    // アニメーション再生するか
        Debugger::Instance().DG_SliderInt("AnimationIndex", 1, &m_CurrentAnimIndex, 0, m_Animations.size() - 1);    // アニメーションインデックス


#ifdef _DEBUG
        Debugger::Instance().DG_CheckBox("DrawToLine", &m_IsDrawLine);  // ライン表示にするか
        Debugger::Instance().DG_CheckBox("IsDrawDebugBone", &m_IsDrawDebugBone);    // ボーンを表示するか
        /*ボーンの大きさ変更*/
        Debugger::Instance().DG_DragVec3("BoneScale", &m_BoneScale, 0.05f, 1.0f, 100.0f);
#endif
        Debugger::Instance().DG_Text("");
        Debugger::Instance().DG_Text("MeshInfo ==========================");
        Debugger::Instance().DG_Text("");

        for (size_t i = 0; i < m_MeshNum; i++)
        {
            Debugger::Instance().DG_TextValue("VertexNum: %d", m_pMeshes[i].get_VertexNum());   // メッシュごとの頂点数
        }

        Debugger::Instance().DG_Text("");
        Debugger::Instance().DG_Text("NodeInfo =========================="); 
        Debugger::Instance().DG_Text(""); 

        // ノード
        for (size_t i = 0; i < m_NodeList.size(); i++)
        {
            Debugger::Instance().DG_Text(m_NodeList[i]->Name); // ノード名
        }
        Debugger::Instance().DG_Text("");
        Debugger::Instance().DG_Text("BoneInfo ==========================");
        Debugger::Instance().DG_Text("");
        
        // ボーン
        for (size_t i = 0; i < m_BoneList.size(); i++)
        {
            Debugger::Instance().DG_Text(std::to_string(m_BoneIndexMap[m_BoneList[i].Name]));   // インデックスマップ表示
            Debugger::Instance().DG_SameLine();
            Debugger::Instance().DG_Text(m_BoneList[i].Name); // ボーン名
        }
        Debugger::Instance().EndDebugWindow();
    }

    m_AnimProcTime +=0.023f;

    // ボーン変換
    BoneTransformsUpdate(renderer, m_AnimProcTime);

    // トランスフォーム更新
    SetupTransform(renderer);

    // マテリアル情報セット ==========================
    CB_MATERIAL mat{};
    mat.Diffuse = m_MaterialList[0].DiffuseColor;
    mat.Specular = m_MaterialList[0].SpecularColor;
    mat.Normal = m_MaterialList[0].NormalColor;
    mat.SpecularPower = m_MaterialList[0].SpecularPower;
    m_pCBMaterialDataSet->Data = mat;

    // 定数バッファに転送
    pDeviceContext->UpdateSubresource(
        m_pCBMaterialDataSet->pBuff,
        0,
        nullptr,
        &m_pCBMaterialDataSet->Data,
        0,
        0
    );

    // マテリアル用定数バッファのセット
    pDeviceContext->VSSetConstantBuffers(4, 1, &m_pCBMaterialDataSet->pBuff);

    // モデルシェーダに切り替え
    ShaderManager::Instance().DeviceToSetShader(SHADER_TYPE::MODEL);    


    /* デバッグモード指定の場合、ワイヤーフレームで表示 */
    if (m_IsDrawLine) {
        pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
    }
    else {
        pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    }


    // メッシュの描画
    for (u_int meshIdx = 0; meshIdx < m_MeshNum; meshIdx++)
    {
        aiMesh *mesh = m_pScene->mMeshes[meshIdx];
        auto &mat = m_MaterialList[mesh->mMaterialIndex];

        /* ビューの設定 */
        {
            // ディフューズ
            if (mat.Diffuse.Texture.lock() != nullptr) {
                auto diff = mat.Diffuse.Texture.lock()->get_SRV();
                if (diff != nullptr)
                    pDeviceContext->PSSetShaderResources(0, 1, &diff);
            }

            // ノーマル
            if (mat.Normal.Texture.lock() != nullptr) {
                auto norm = mat.Normal.Texture.lock()->get_SRV();
                if (norm != nullptr)
                    pDeviceContext->PSSetShaderResources(1, 1, &norm);
            }
            // スペキュラ
            if (mat.Specular.Texture.lock() != nullptr) {
                auto spec = mat.Specular.Texture.lock()->get_SRV();
                if (spec != nullptr)
                    pDeviceContext->PSSetShaderResources(2, 1, &spec);
            }
        }
        m_pMeshes[meshIdx].Draw(renderer);
    }
}


//*---------------------------------------------------------------------------------------
//* @:Model Class 
//*【?】終了
// 
//* 引数：なし
//* 返値：bool
//*----------------------------------------------------------------------------------------
bool Model::Term(RendererManager &renderer)
{
	for (u_int meshIdx = 0; meshIdx < m_MeshNum; ++meshIdx)
	{
		m_pMeshes[meshIdx].Term();
	}

	SAFE_DELETE_ARRAY(m_pMeshes);

    SAFE_RELEASE(m_ConstanrBufferBonesData->pBuff);

	m_MeshNum = 0;

    return true;
}


//*---------------------------------------------------------------------------------------
//* @:Model Class 
//*【?】ボーン変換行列の計算    ※再帰関数
// 
//* 引数：1.アニメーション単位のティック数
//* 引数：2.処理するノードインデックス
//* 引数：3.親ボーンのワールド変換行列
//* 返値：void
//*----------------------------------------------------------------------------------------
void Model::TransformBone(float animTimeTicks, UINT nodeIdx, const DirectX::XMMATRIX& parent)
{
    XMMATRIX localTransformMtx = XMMatrixIdentity(); // ローカル行列
    XMMATRIX worldTransformMtx = XMMatrixIdentity(); // ボーンの最終ワールド座標変換行列
    localTransformMtx = m_NodeList[nodeIdx]->Transformation; // 初期姿勢で初期化
    std::string nodeName = m_NodeList[nodeIdx]->Name;

    // アニメーションがあるなら
    if (!m_Animations.empty()) {
        const AnimationData* pAnim = m_Animations[m_CurrentAnimIndex];

        const NodeAnimChannel* pNodeAnim = FindNodeAnim(pAnim, nodeName);  // アニメーション確認

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
    worldTransformMtx = localTransformMtx * parent ;  


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
//* @:Model Class 
//*【?】ボーンの更新
// 
//* 引数：1.RendererManager
//* 引数：2.秒単位の時間
//* 返値：void
//*----------------------------------------------------------------------------------------
void Model::BoneTransformsUpdate(RendererManager& renderer, float timeInSeconds)
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

    // 定数バッファに転送
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

    /* とりま8つデバッグ表示 */
    /*
    for (int i = 0; i < 8; i++)
    {
        float temp1[4] = {
            m_ConstanrBufferBonesData->Data.BonesMatrices[i]._11,
            m_ConstanrBufferBonesData->Data.BonesMatrices[i]._12,
            m_ConstanrBufferBonesData->Data.BonesMatrices[i]._13,
            m_ConstanrBufferBonesData->Data.BonesMatrices[i]._14,
        };
        float temp2[4] = {
            m_ConstanrBufferBonesData->Data.BonesMatrices[i]._21,
            m_ConstanrBufferBonesData->Data.BonesMatrices[i]._22,
            m_ConstanrBufferBonesData->Data.BonesMatrices[i]._23,
            m_ConstanrBufferBonesData->Data.BonesMatrices[i]._24,
        };
        float temp3[4] = {
            m_ConstanrBufferBonesData->Data.BonesMatrices[i]._31,
            m_ConstanrBufferBonesData->Data.BonesMatrices[i]._32,
            m_ConstanrBufferBonesData->Data.BonesMatrices[i]._33,
            m_ConstanrBufferBonesData->Data.BonesMatrices[i]._34,
        };
        float temp4[4] = {
            m_ConstanrBufferBonesData->Data.BonesMatrices[i]._41,
            m_ConstanrBufferBonesData->Data.BonesMatrices[i]._42,
            m_ConstanrBufferBonesData->Data.BonesMatrices[i]._43,
            m_ConstanrBufferBonesData->Data.BonesMatrices[i]._44,
        };

        Debugger::Instance().BeginDebugWindow("FinalBoneMatrix");
        Debugger::Instance().DG_SliderFloat("_11~14:", 4, temp1, -10000.0, 10000.0);
        Debugger::Instance().DG_SliderFloat("_21~24:", 4, temp2, -10000.0, 10000.0);
        Debugger::Instance().DG_SliderFloat("_31~34:", 4, temp3, -10000.0, 10000.0);
        Debugger::Instance().DG_SliderFloat("_41~44:", 4, temp4, -10000.0, 10000.0);
        Debugger::Instance().EndDebugWindow();
    }
    */

    // ******************************************************************
    //
    /* ↓↓↓↓↓↓↓ デバッグ用のボーン表示  ↓↓↓↓↓↓↓ */
    //
    // ******************************************************************
#ifdef _DEBUG
    if (m_IsDrawDebugBone == false)return;

    for (size_t i = 0; i < m_pDebugBones.size(); i++)
    {
        // ボーンの行列（例: ローカル to ワールド or スキニング済みボーン行列）
        XMMATRIX boneMatrix = m_BoneList[i].DebugWorldMatrix;

        // 行列を位置・回転・スケールに分解
        XMVECTOR scale;
        XMVECTOR rotationQuat;
        XMVECTOR translation;

        if (XMMatrixDecompose(&scale, &rotationQuat, &translation, boneMatrix))
        {
            XMFLOAT3 pos, rot, scl;

            XMStoreFloat3(&pos, translation);
            m_pDebugBones[i]->get_Transform().lock()->set_Scale(m_BoneScale);
            m_pDebugBones[i]->get_Transform().lock()->set_RotateToRad(0.0,0.0,0.0);  // オイラー角で渡す想定
            m_pDebugBones[i]->get_Transform().lock()->set_Pos(pos.x, pos.y, pos.z);
        }
        m_pDebugBones[i]->Draw(renderer);
    }

#endif // _DEBUG
}


//*---------------------------------------------------------------------------------------
//* @:Model Class 
//*【?】指定のアニメーション情報があるか確認する
// 
//* 引数：1.AnimationData
//* 引数：2.確認するノード名
//* 返値：NodeAnimChannel*
//*----------------------------------------------------------------------------------------
const NodeAnimChannel* Model::FindNodeAnim(const AnimationData* pAnim, const std::string& nodeName)
{
    for (size_t i = 0; i < pAnim->Channels.size(); i++){
        const NodeAnimChannel* pNodeAnim = &pAnim->Channels[i];
        if (pNodeAnim->NodeName == nodeName)
        {
            return pNodeAnim;
        }
    }
    return nullptr;
}


//*---------------------------------------------------------------------------------------
//* @:Model Class 
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
UINT Model::FindPosition(float animTimeTicks, const NodeAnimChannel* pNodeAnimChannel)
{
    assert(pNodeAnimChannel->PositionKeys.size() > 0);

    for (UINT i = 0; i < pNodeAnimChannel->PositionKeys.size() - 1; i++)
    {
        double t = pNodeAnimChannel->PositionKeys[i+1].Time;
        if (animTimeTicks < t) {
            return i;
        }
    }
    return 0;
}

/* Rotationのキーフレームインデックスを探す */
UINT Model::FindRotation(float animTimeTicks, const NodeAnimChannel* pNodeAnimChannel)
{
    assert(pNodeAnimChannel->RotationKeys.size() > 0);

    for (UINT i = 0; i < pNodeAnimChannel->RotationKeys.size() - 1; i++)
    {
        double t = pNodeAnimChannel->RotationKeys[i+1].Time;
        if (animTimeTicks < t) {
            return i;
        }
    }
    return 0;
}

/* Scalingのキーフレームインデックスを探す */
UINT Model::FindScaling(float animTimeTicks, const NodeAnimChannel* pNodeAnimChannel)
{
    assert(pNodeAnimChannel->ScalingKeys.size() > 0);

    for (UINT i = 0; i < pNodeAnimChannel->ScalingKeys.size() - 1; i++)
    {
        double t = pNodeAnimChannel->ScalingKeys[i+1].Time;
        if (animTimeTicks < t) {
            return i;
        }
    }
    return 0;
}


//**************************************************************************************


//*---------------------------------------------------------------------------------------
//* @:Model Class 
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
void Model::CalcInterpolatedPosition(DirectX::XMMATRIX& out, float animTimeTicks, const NodeAnimChannel* pNodeAnimChannel)
{
    if ((pNodeAnimChannel->PositionKeys.size() < 2)) {
        VEC3 res = pNodeAnimChannel->PositionKeys[0].Value;
        out = XMMatrixTranslation(res.x, res.y, res.z);
        return;
    }

    VEC3 startPos{};
    VEC3 endPos{};
    UINT posIndex =  FindPosition(animTimeTicks, pNodeAnimChannel);
    UINT nextPosIndex = posIndex + 1;
    startPos = pNodeAnimChannel->PositionKeys[posIndex].Value;          // 現在位置
    endPos   = pNodeAnimChannel->PositionKeys[nextPosIndex].Value;      // 次の位置
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
void Model::CalcInterpolatedRotation(DirectX::XMMATRIX& out, float animTimeTicks, const NodeAnimChannel* pNodeAnimChannel)
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
void Model::CalcInterpolatedScaling(DirectX::XMMATRIX& out, float animTimeTicks, const NodeAnimChannel* pNodeAnimChannel)
{
    if ((pNodeAnimChannel->ScalingKeys.size()<2)){
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


//**************************************************************************************

//*---------------------------------------------------------------------------------------
//* @:Model Class 
//*【?】モデルデータのセットアップ     ※ 必ず最初に呼ぶ
//      ファイルパス名は日本語非対応
// 
//* 引数：1.RendererManager
//* 引数：2.ファイルパス
//* 返値：bool セットアップが完了したか
//*----------------------------------------------------------------------------------------
bool Model::Setup(RendererManager& render, const char* filePath)
{
    // aiProcess_FlipUVs ＵＶ値をdirect3Dに合うようにしてくれる (Ｖ軸の反転)
    // 他のフラグもあったほうがいいやつらしい https://qiita.com/dpals39/items/1681d9101e58b5aefa27
    u_int flag = aiProcess_Triangulate |
        aiProcess_CalcTangentSpace |
        aiProcess_GenNormals |
        aiProcess_MakeLeftHanded |
        aiProcess_FlipWindingOrder |
        aiProcess_FlipUVs |
        aiProcess_Triangulate |
        aiPrimitiveType_LINE |
        aiPrimitiveType_POINT;
    
    // 不要なノードを勝手に追加しないようにする https://qiita.com/24ban/items/3cdb37188b74bcf1028c
    m_Importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);

    /* ↓日本語は対応してないっぽい ↓*/
    m_pScene = m_Importer.ReadFile(filePath, flag);	// ファイルの読み込み

    // 読み込み失敗なら返す
    if (m_pScene == nullptr)return false;

    /* オフセット行列がそもそも初期姿勢の逆行列っぽいので以下の変数は要らない */
    /*
    //// 初期姿勢行列を転置して保持
    aiMatrix4x4 tempMat = m_pScene->mRootNode->mTransformation;
    m_GlobalInversTransformMtx = XMMatrixTranspose(
        XMMATRIX(
            tempMat.a1, tempMat.a2, tempMat.a3, tempMat.a4,
            tempMat.b1, tempMat.b2, tempMat.b3, tempMat.b4,
            tempMat.c1, tempMat.c2, tempMat.c3, tempMat.c4,
            tempMat.d1, tempMat.d2, tempMat.d3, tempMat.d4
        )
    );
    
    // 逆行列にする
    m_GlobalInversTransformMtx = XMMatrixInverse(NULL, m_GlobalInversTransformMtx);
    */


    // マテリアル情報の抽出
    //MatrialExtraction(m_pScene);

    // アニメーション情報の抽出
    if (m_pScene->HasAnimations()) {
        AnimationExtraction(m_pScene);
    }

    // ノード情報の抽出
    NodeExtraction(m_pScene->mRootNode, -1);

    // メッシュデータを取得し、セットアップ
    if (m_pScene->HasMeshes())
    {
        m_MeshNum = m_pScene->mNumMeshes;	// メッシュ数取得

        m_pMeshes = new ModelMesh[m_MeshNum];
        for (u_int meshIdx = 0; meshIdx < m_MeshNum; ++meshIdx)
        {
            aiMesh* pMeshData = m_pScene->mMeshes[meshIdx];

            // ボーン情報の抽出
            if (pMeshData->HasBones()) {
                BoneExtraction(pMeshData);
            }

            // メッシュのセットアップ
            if (m_pMeshes[meshIdx].Setup(render, pMeshData,this) == false) {
                return false;
            }
        }
    }

    m_MaterialList.resize(m_pScene->mNumMaterials);

    // ボーン変換用定数バッファの作成
    if (CreateBonesCBuffer(render) == false)
    {
        return false;
    }

    // マテリアル定数バッファの作成
    if (CreateMaterialCBuffer(render) == false)
    {
        return false;
    }

    // ワールド定数バッファの作成
    if (CreateTransformCBuffer(render) == false)
    {
        return false;
    }

    return true;
}


//*---------------------------------------------------------------------------------------
//* @:Model Class 
//*【?】アニメーション情報の抽出
// 
//* 引数：1.モデルシーン Assimp
//* 返値：void
//*----------------------------------------------------------------------------------------
void Model::AnimationExtraction(const aiScene* scene)
{
    for (size_t i = 0; i < scene->mNumAnimations; i++)
    {
        AnimationData* animData = new AnimationData();// 抽出データ格納先
        aiAnimation* anim = scene->mAnimations[i]; // アニメーション情報
        
        animData->Name = anim->mName.C_Str(); // アニメーション名
        animData->Duration = anim->mDuration; // アニメーションの長さ
        animData->TicksPerSecond = anim->mTicksPerSecond; // 1秒あたりのティック数
        if (animData->TicksPerSecond < 0.0)
        {
            animData->TicksPerSecond = 1.0;
        }
        // チャンネル情報を取得する ===============================================================
        for (size_t j = 0; j < anim->mNumChannels; j++)
        {
            NodeAnimChannel channelData{};                     // チャンネルデータ格納先
            aiNodeAnim* channel = anim->mChannels[j];          // チャンネル情報
            channelData.NodeName = channel->mNodeName.C_Str(); // 影響を受けるノード名

            // 座標キーフレームを取得 ===============================================================
            for (size_t key = 0; key < channel->mNumPositionKeys; key++)
            {
                PositionKey posKey{};
                posKey.Time = channel->mPositionKeys[key].mTime; // 時間
                posKey.Value.x = channel->mPositionKeys[key].mValue.x; // X座標
                posKey.Value.y = channel->mPositionKeys[key].mValue.y; // Y座標
                posKey.Value.z = channel->mPositionKeys[key].mValue.z; // Z座標
                channelData.PositionKeys.push_back(posKey); // チャンネルに追加
            }     
            
            // 回転キーフレームを取得 ===============================================================
            for (size_t key = 0; key < channel->mNumRotationKeys; key++)
            {
                RotationKey rotKey{};
                rotKey.Time = channel->mRotationKeys[key].mTime; // 時間
                rotKey.Value= channel->mRotationKeys[key].mValue; // クォータニオン値
                channelData.RotationKeys.push_back(rotKey); // チャンネルに追加
            }

            // スケールキーフレームを取得 ===============================================================
            for (size_t key = 0; key < channel->mNumScalingKeys; key++)
            {
                ScalingKey sclKey{};
                sclKey.Time = channel->mScalingKeys[key].mTime; // 時間
                sclKey.Value.x = channel->mScalingKeys[key].mValue.x; // Xスケール
                sclKey.Value.y = channel->mScalingKeys[key].mValue.y; // Yスケール
                sclKey.Value.z = channel->mScalingKeys[key].mValue.z; // Zスケール
                channelData.ScalingKeys.push_back(sclKey); // チャンネルに追加
            }

            animData->Channels.push_back(channelData); // アニメーションに追加
        }

        m_Animations.push_back(animData); // アニメーションリストに追加
    }
}


//*---------------------------------------------------------------------------------------
//* @:Model Class 
//*【?】ノード情報の抽出  再帰関数
// 
//* 引数：1.ノード    Assimp
//* 引数：2.親ノードのインデックス
//* 返値：void
//*----------------------------------------------------------------------------------------
void Model::NodeExtraction(const aiNode* pNode, int parentIdx)
{
    NodeInfo* nodeData = new NodeInfo();    // 格納先
    nodeData->Name = pNode->mName.C_Str();  // 名前
    nodeData->ParentIndex = parentIdx;      // 親のインデックス

    aiMatrix4x4 tempMat = pNode->mTransformation;   // ローカル変換行列
    nodeData->Transformation = XMMatrixTranspose(
        XMMATRIX(
            tempMat.a1, tempMat.a2, tempMat.a3, tempMat.a4,
            tempMat.b1, tempMat.b2, tempMat.b3, tempMat.b4,
            tempMat.c1, tempMat.c2, tempMat.c3, tempMat.c4,
            tempMat.d1, tempMat.d2, tempMat.d3, tempMat.d4
        )
    );

    // 現在のノードインデックスを保持
    int myIndex = static_cast<int>(m_NodeList.size());
    m_NodeList.push_back(nodeData);

    // 親ノードに現在のインデックスを子として登録
    // ルート（-1）以外
    if (parentIdx != -1)
    {
        m_NodeList[parentIdx]->ChildrenIndices.push_back(myIndex);
    }

    // 子ノードを再帰的に処理
    for (size_t i = 0; i < pNode->mNumChildren; i++)
    {
        // 現在のインデックスを親として渡す
        NodeExtraction(pNode->mChildren[i], myIndex);
    }

}


//*---------------------------------------------------------------------------------------
//* @:Model Class 
//*【?】ボーン情報の抽出
// 
//* 引数：1.メッシュ情報 Assimp
//* 返値：void
//*----------------------------------------------------------------------------------------
void Model::BoneExtraction(aiMesh* mesh)
{
    //mesh->mBones[i]->mName;           // ボーン名
    //mesh->mBones[i]->mNumWeights;     // このボーンの影響を受ける頂点数
    //mesh->mBones[i]->mWeights;        // 影響度
    //mesh->mBones[i]->mOffsetMatrix;   // オフセット行列
    
    for (size_t i = 0; i < mesh->mNumBones; i++)
    {
        aiBone* bone = mesh->mBones[i]; // メッシュに含まれるボーン
        BoneInfo extractionData{};     // 抽出データ格納先

        // ボーン名取得
        extractionData.Name = bone->mName.C_Str(); 
        
        // オフセット行列を転置して取得
        extractionData.OffsetMtx = XMMatrixTranspose(
            XMMATRIX(
                bone->mOffsetMatrix.a1, bone->mOffsetMatrix.a2, bone->mOffsetMatrix.a3, bone->mOffsetMatrix.a4,
                bone->mOffsetMatrix.b1, bone->mOffsetMatrix.b2, bone->mOffsetMatrix.b3, bone->mOffsetMatrix.b4,
                bone->mOffsetMatrix.c1, bone->mOffsetMatrix.c2, bone->mOffsetMatrix.c3, bone->mOffsetMatrix.c4,
                bone->mOffsetMatrix.d1, bone->mOffsetMatrix.d2, bone->mOffsetMatrix.d3, bone->mOffsetMatrix.d4
            )
        );

        extractionData.WeightList.reserve(bone->mNumWeights); // 影響度リストの容量確保

        // 各頂点への影響度を取得
        for (size_t j = 0; j < bone->mNumWeights; j++)
        {
            aiVertexWeight weight = bone->mWeights[j];
            VertexBoneWeightData weightData{};

            weightData.VertexID = weight.mVertexId;  // 影響を受ける頂点ID取得
            weightData.Weight = weight.mWeight;      // 影響度（0.0～1.0）取得
            extractionData.WeightList.push_back(weightData);    // ボーン情報に追加
        }

        // ボーンインデックスマップに登録
        m_BoneIndexMap[extractionData.Name] = static_cast<int>(i);
        
        // ボーンリストに追加
        m_BoneList.push_back(extractionData);
    }
}


//*---------------------------------------------------------------------------------------
//* @:Model Class 
//*【?】ボーン変換用定数バッファの作成
// 
//* 引数：1.RendererManager 
//* 返値：bool
//*----------------------------------------------------------------------------------------
bool Model::CreateBonesCBuffer(RendererManager& render)
{
    auto device = render.get_Device();	// デバイス取得
    m_ConstanrBufferBonesData = new CB_BONES_DATA_SET;

    HRESULT hr = S_OK;

    // リソース設定
    D3D11_BUFFER_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.ByteWidth = sizeof(CB_BONES_DATA);
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    desc.StructureByteStride = 0;
    for (size_t i = 0; i < ARRAYSIZE(m_ConstanrBufferBonesData->Data.BonesMatrices); i++)
    {
        m_ConstanrBufferBonesData->Data.BonesMatrices[i] = DirectX::XMFLOAT4X4(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }

    // 定数バッファ作成
    hr = device->CreateBuffer(&desc, NULL, &m_ConstanrBufferBonesData->pBuff);
    if (FAILED(hr))return false;

    // デバッグ表示用のボーン作成
#ifdef _DEBUG
    for (size_t i = 0; i < m_BoneList.size(); i++) {
        Cubu* bone = new Cubu();
        Tool::UV::SpriteUV uv[6];
        uv[0] = Tool::UV::MakeSpriteUV(0.0f, 0.0f, 2048.0f, 2048.0f, 2048.0f, 2048.0f);    // 前
        uv[1] = Tool::UV::MakeSpriteUV(0.0f, 0.0f, 2048.0f, 2048.0f, 2048.0f, 2048.0f);    // 右
        uv[2] = Tool::UV::MakeSpriteUV(0.0f, 0.0f, 2048.0f, 2048.0f, 2048.0f, 2048.0f);    // 上
        uv[3] = Tool::UV::MakeSpriteUV(0.0f, 0.0f, 2048.0f, 2048.0f, 2048.0f, 2048.0f);    // 下
        uv[4] = Tool::UV::MakeSpriteUV(0.0f, 0.0f, 2048.0f, 2048.0f, 2048.0f, 2048.0f);    // 左
        uv[5] = Tool::UV::MakeSpriteUV(0.0f, 0.0f, 2048.0f, 2048.0f, 2048.0f, 2048.0f);    // 裏
        MATERIAL* mat = new MATERIAL();
        mat->DiffuseColor = VEC4(1, 0, 0, 1);
        mat->Diffuse.Texture = ResourceManager::Instance().LoadTexture(L"Resource/Texture/Metal059B_2K-PNG_Color.png");
        if (!bone->Setup(render, mat, 1))return false;
        m_pDebugBones.push_back(bone);
    }
#endif

    return true;
}


//*---------------------------------------------------------------------------------------
//* @:Model Class 
//*【?】マテリアル用定数バッファの作成
// 
//* 引数：1.RendererManager 
//* 返値：bool
//*----------------------------------------------------------------------------------------
bool Model::CreateMaterialCBuffer(RendererManager& render)
{
    auto pDevice = render.get_Device();

    // 定数バッファの設定
    D3D11_BUFFER_DESC bd{};
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;						// 標準設定
    bd.ByteWidth = sizeof(CB_MATERIAL);				    // バッファのサイズ
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;			// 定数バッファとして使う
    bd.CPUAccessFlags = 0;								// CPUから書き込みしない
    bd.MiscFlags = 0;
    bd.StructureByteStride = 0;

    // マテリアル情報定数バッファの生成
    m_pCBMaterialDataSet = new CB_MATERIAL_SET;
    if (m_pCBMaterialDataSet == nullptr) {
        return false;
    }

    // 定数バッファの生成
    HRESULT hr = pDevice->CreateBuffer(&bd, nullptr, &m_pCBMaterialDataSet->pBuff);
    if (FAILED(hr)) {
        return false;
    }

    return true;
}


//*---------------------------------------------------------------------------------------
//* @:Model Class 
//*【?】ワールド変換用定数バッファの作成
// 
//* 引数：1.RendererManager 
//* 返値：bool
//*----------------------------------------------------------------------------------------
bool Model::CreateTransformCBuffer(RendererManager& renderer)
{
    auto pDevice = renderer.get_Device();

    // ワールド変換定数バッファの生成
    m_pCBTransformSet = new CB_TRANSFORM_SET;
    if (m_pCBTransformSet == nullptr) {
        return false;
    }

    // 定数バッファの設定
    D3D11_BUFFER_DESC bd{};
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;						// 標準設定
    bd.ByteWidth = sizeof(CB_TRANSFORM);				// バッファのサイズ
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;			// 定数バッファとして使う
    bd.CPUAccessFlags = 0;								// CPUから書き込みしない
    bd.MiscFlags = 0;
    bd.StructureByteStride = 0;

    // 定数バッファの生成
    HRESULT hr = pDevice->CreateBuffer(&bd, nullptr, &m_pCBTransformSet->pBuff);
    if (FAILED(hr)) {
        return false;
    }

    return true;
}


//--------------------------------------------------------------------------------------
//      * Model Class - マテリアル情報の抽出 - *
// ※本来はここで自動的にパスを参照する予定だったけど、抽出したパスが絶対パス？になっていたため、
//   set_TextureMapで手動でつけてね(｡-人-｡) ｺﾞﾒﾝﾈ
//--------------------------------------------------------------------------------------
void Model::MatrialExtraction(const aiScene* scene)
{
    m_MaterialList.resize(scene->mNumMaterials);

    // マテリアルの数分回す
    for (UINT i = 0; i < scene->mNumMaterials; i++)
    {
        aiMaterial* aiMatData = scene->mMaterials[i];   // マテリアル取得
        MATERIAL mat{};
        aiString path{};

        /* テクスチャの読み込みが上手くいかない */
        // GetTextureに内部的に紐づけられてるパスがpathに入るっぽいけど、間違ったパスを指してる？
        /*
        // ディフューズマップ
        if (aiMatData->GetTexture(aiTextureType_DIFFUSE, 0, &path) == aiReturn_SUCCESS)
        {
            mat.Diffuse = ResourceManager::Instance().LoadTexture(path);
        }
        
        // ノーマルマップ
        if (aiMatData->GetTexture(aiTextureType_NORMALS, 0, &path) == aiReturn_SUCCESS)
        {
            mat.Normal = ResourceManager::Instance().LoadTexture(path);
        }
        
        // スペキュラマップ
        if (aiMatData->GetTexture(aiTextureType_SPECULAR, 0, &path) == aiReturn_SUCCESS)
        {
            mat.Specular = ResourceManager::Instance().LoadTexture(path);
        }
        */

        // マテリアルリストに追加
        m_MaterialList[i] = mat;
    }
}


//*---------------------------------------------------------------------------------------
//* @:Model Class 
//*【?】テクスチャマップの設定
//   ※ 本当はAssimpの処理で内部のパスを参照し、テクスチャを自動で適応できるようにする予定だったけど、
//      mixamoから持ってきたモデルのパスがおかしかったため、手動で付けるようにした。
//      そこまで大きなモデルファイルを使う予定がないので、
//      マテリアル番号はモデルビューアなどで確認して・・・(m´・ω・｀)m ｺﾞﾒﾝ…
//* 引数：1.マップタイプ
//* 引数：2.マテリアル番号
//* 引数：3.ファイルパス
//* 返値：bool
//*----------------------------------------------------------------------------------------
bool Model::set_TextureMap(TEXTURE_MAP mapType,int matIndex, const std::wstring &path)
{
    MATERIAL mat{};

    // テクスチャ読み込み
    auto texture = ResourceManager::Instance().LoadTexture(path); 
    if (texture == nullptr) {
        return false;
    }

    // 範囲外アクセスチェック
    if (m_MaterialList.size() <= matIndex) {
        return false;
    }

    // 対応したマップにテクスチャを入れる
    switch (mapType)
    {
    case TEXTURE_MAP_NONE:
        break;
    case TEXTURE_MAP_DIFFUSE:
        m_MaterialList[matIndex].Diffuse.Texture = texture;
        break;
    case TEXTURE_MAP_NORMAL:
        m_MaterialList[matIndex].Normal.Texture = texture;
        break;
    case TEXTURE_MAP_SPECULAR:
        m_MaterialList[matIndex].Specular.Texture = texture;
        break;
    default:
        break;
    }

    return true;
}


//*---------------------------------------------------------------------------------------
//* @:Model Class 
//*【?】定数バッファにワールド変換行列を書き込む
// 
//* 引数：1.RendererManager 
//* 返値：bool
//*----------------------------------------------------------------------------------------
void Model::SetupTransform(RendererManager& render)
{
    auto pDeviceContext = render.get_DeviceContext();   // コンテキスト

    // ワールド行列セット ==========================
    XMMATRIX worldMtx = get_WorldMtx();
    XMStoreFloat4x4(&m_pCBTransformSet->Data.WorldMtx, worldMtx);  // XMMATRIX → XMFLOAT4X4変換

    // 定数バッファに転送
    pDeviceContext->UpdateSubresource(
        m_pCBTransformSet->pBuff,
        0,
        nullptr,
        &m_pCBTransformSet->Data,
        0,
        0
    );

    pDeviceContext->VSSetConstantBuffers(0, 1, &m_pCBTransformSet->pBuff);
}



//*---------------------------------------------------------------------------------------
//* @:Model Class 
//*【?】定数バッファにワールド変換行列を書き込む
// 
//* 引数：1.カラー 
//* 引数：2.設定するメッシュインデックス 
//* 引数：3.設定する頂点インデックス 
//* 返値：void
//*----------------------------------------------------------------------------------------
void Model::set_Color(const VECTOR4::VEC4& col, UINT meshIdx, UINT vtxIdx)
{
    if (m_pMeshes == nullptr)return;


    m_Color = col;

    if (meshIdx < 0 || meshIdx > m_MeshNum) {
        for (u_int i = 0; i < m_MeshNum; i++)
        {
            if (vtxIdx == -1)
                m_pMeshes[i].set_Color(m_Color);
            else
                m_pMeshes[i].set_Color(m_Color, vtxIdx);
        }
    }
    else {
        if (vtxIdx == -1)
            m_pMeshes[meshIdx].set_Color(m_Color);
        else
            m_pMeshes[meshIdx].set_Color(m_Color, vtxIdx);

    }
}


//--------------------------------------------------------------------------------------
//      * Model Class - カラー設定 - *
// 引数 1.取得するメッシュのインデックス
//--------------------------------------------------------------------------------------
VEC4 Model::get_MeshIdxColor(int meshIdx)const
{
    if (m_pMeshes == nullptr)return VEC4(0.0f,0.0f,0.0f,0.0f);

    if (meshIdx < 0)return VEC4(0.0f, 0.0f, 0.0f, 0.0);
    VEC4 col{ 0.0f,0.0f,0.0f,0.0f };
    col = m_pMeshes[meshIdx].get_Color();

    return col;
}


//--------------------------------------------------------------------------------------
//      * Model Class - メッシュ＆頂点インデックス指定型 カラー取得 - *
// 引数 1.取得するメッシュのインデックス
//--------------------------------------------------------------------------------------
VEC4 Model::get_MeshAndVtxIdxColor(int meshIdx, int vtxIdxconst) const
{
    if (meshIdx <= 0 || vtxIdxconst <= 0)return VEC4(0.0f, 0.0f, 0.0f, 0.0);
    VEC4 col;
    col = m_pMeshes[meshIdx].get_Color(vtxIdxconst);

    return col;
}

//--------------------------------------------------------------------------------------
//      * Model Class - 指定メッシュの頂点数取得 - *
// 引数 1.取得するメッシュインデックス
//--------------------------------------------------------------------------------------
unsigned int Model::get_VertexNum(unsigned int meshIdx)const
{
    if (meshIdx < 0 || m_MeshNum < meshIdx)
        return 0;

    return m_pMeshes[meshIdx].get_VertexNum();
}

//--------------------------------------------------------------------------------------
//      * Model Class - ボーンインデックス取得 - *
// 引数 1.取得するボーンのタグ
//--------------------------------------------------------------------------------------
int Model::get_BoneIndex(const std::string& tag)
{
    return m_BoneIndexMap[tag];
}

//--------------------------------------------------------------------------------------
//      * Model Class - ポジション取得 - *
//--------------------------------------------------------------------------------------
VEC3 Model::get_Pos() const
{
    return m_Position;
}

//--------------------------------------------------------------------------------------
//      * Model Class - 回転地取得 デグリー - *
//--------------------------------------------------------------------------------------
VEC3 Model::get_RotateToDeg() const
{
    VEC3 res{};
    res.x = RadToDeg(m_Rotation.x);
    res.y = RadToDeg(m_Rotation.y);
    res.z = RadToDeg(m_Rotation.z);

    return res;
}

//--------------------------------------------------------------------------------------
//      * Model Class -回転地取得 ラジアン - *
//--------------------------------------------------------------------------------------
VEC3 Model::get_RotateToRad() const
{
    return m_Rotation;
}

//--------------------------------------------------------------------------------------
//      * Model Class - スケール取得 - *
//--------------------------------------------------------------------------------------
VEC3 Model::get_Scale() const
{
    return m_Scale;
}

//--------------------------------------------------------------------------------------
//      * Model Class - 座標を設定 - *
//--------------------------------------------------------------------------------------
void Model::set_Pos(VECTOR3::VEC3 pos)
{
    m_Position = pos;
}


//--------------------------------------------------------------------------------------
//      * Model Class - 回転値を設定 ※ラジアン指定 - *
//--------------------------------------------------------------------------------------
void Model::set_RotateToRad(VECTOR3::VEC3 rot)
{
    m_Rotation = rot;
}

//--------------------------------------------------------------------------------------
//      * Model Class - 回転値を設定 ※デグリー指定 - *
//--------------------------------------------------------------------------------------
void Model::set_RotateToDeg(VECTOR3::VEC3 rot)
{
    // ラジアンへ変換
    VEC3 deg;
    deg.x = DegToRad(rot.x);
    deg.y = DegToRad(rot.y);
    deg.z = DegToRad(rot.z);

    m_Rotation = deg;
}


//--------------------------------------------------------------------------------------
//      * Model Class - スケールを設定 - *
//--------------------------------------------------------------------------------------
void Model::set_Scale(VECTOR3::VEC3 scl)
{
    m_Scale = scl;
}

//--------------------------------------------------------------------------------------
//      * Model Class - 親のワールド行列を設定 - *
//--------------------------------------------------------------------------------------
void Model::set_ParentMtx(Model* parent)
{
    m_pParentMtx = parent;
}


//--------------------------------------------------------------------------------------
//      * Model Class - 座標行列を取得 - *
//--------------------------------------------------------------------------------------
XMMATRIX Model::get_MtxPos()const
{
    return XMMatrixScaling(m_Position.x, m_Position.y, m_Position.z);
}

//--------------------------------------------------------------------------------------
//      * Model Class - 回転行列を取得 - *
//--------------------------------------------------------------------------------------
XMMATRIX Model::get_MtxRotate()const
{
    return XMMatrixScaling(m_Rotation.x, m_Rotation.y, m_Rotation.z);
}

//--------------------------------------------------------------------------------------
//      * Model Class - スケール行列を取得 - *
//--------------------------------------------------------------------------------------
XMMATRIX Model::get_MtxScale()const
{
    return XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);
}

//--------------------------------------------------------------------------------------
//      * Model Class - ワールド行列を取得 - *
//--------------------------------------------------------------------------------------
XMMATRIX &Model::get_WorldMtx()
{
    XMMATRIX mtxS = XMMatrixScalingFromVector(m_Scale);
    XMMATRIX mtxRot = XMMatrixRotationRollPitchYawFromVector(m_Rotation);
    XMMATRIX mtxT = XMMatrixTranslationFromVector(m_Position);

    XMMATRIX localMtx = mtxS * mtxRot * mtxT;

    // 親がいるなら自分と親を掛けたものを返す
    if (m_pParentMtx != nullptr)
        m_WorldMtx = localMtx * m_pParentMtx->get_WorldMtx();
    else
       m_WorldMtx = localMtx;

    return m_WorldMtx;
}

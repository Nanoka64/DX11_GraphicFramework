#pragma once
//--------------------------------------------------------------------------------------
//      * Includes *
//--------------------------------------------------------------------------------------
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <queue>
#include "GameObject.h"
#include "IComponent.h"
#include "ModelData.h"



// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:Component_SkinnedMeshAnimator Class --- */
//
//  ★継承：IComponent ★
// 
// 【?】スキンメッシュアニメーション用コンポーネント
//
// ***************************************************************************************
class SkinnedMeshAnimator : public IComponent
{
private:
	std::weak_ptr<class ModelMeshResource> m_pMeshResource;	// メッシュ情報を持つ

	CB_BONES_DATA_SET *m_ConstanrBufferBonesData;			// ボーン変換用定数バッファ

	/***********アニメーション関連***********/
	std::vector<BoneInfo> m_BoneList;				     // ボーンリスト
	std::vector<NodeInfo *> m_NodeList;					 // Nodeツリー情報
	std::unordered_map<std::string, int> m_BoneIndexMap; // Node名からBoneListのインデックスを引くためのマップ
	std::vector<AnimationData *>m_Animations;			 // 読み取ったアニメーション

	double m_AnimationTime = 0.0;						 // 現在のアニメーション時間(ticks)
	int m_CurrentAnimIndex = -1;						 // 現在のアニメーションインデックス
	bool m_IsAnimationFlag;	// アニメーションさせるかどうか（とりまデバッグ用）
	float m_AnimProcTime;	// 更新用カウンタ

public:
	SkinnedMeshAnimator(std::weak_ptr<GameObject> pOwner, int updateRank = 100);
	~SkinnedMeshAnimator();

	void Init(RendererManager& renderer)override;
	void Update(RendererManager& renderer)override;

	void BoneTransformsUpdate(RendererManager &renderer, float timeInSeconds);	// ボーンの更新
	void set_MeshResource(std::weak_ptr<class ModelMeshResource> meshResource);	// リソースの設定

	void set_AnimTime(float t) { m_AnimationTime = t; };		// アニメーション再生時間の設定
	void set_AnimIndex(int idx) { m_CurrentAnimIndex = idx; };	// アニメーションインデックスの設定
	void set_IsAnim(bool f) { m_IsAnimationFlag = f; };			// アニメーションフラグ

private:
	void TransformBone(float animTimeTicks, UINT nodeIdx, const DirectX::XMMATRIX &parent); // ボーン変換行列の更新
	const NodeAnimChannel *FindNodeAnim(const AnimationData *pAnim, const std::string &nodeName);	// アニメーションがあるか確認
	UINT FindPosition(float animTimeTicks, const NodeAnimChannel *pNodeAnimChannel);		// 対応するPosキーのインデックスを探す
	UINT FindRotation(float animTimeTicks, const NodeAnimChannel *pNodeAnimChannel);		// 対応するRotキーのインデックスを探す
	UINT FindScaling(float animTimeTicks, const NodeAnimChannel *pNodeAnimChannel);			// 対応するSclキーのインデックスを探す
	void CalcInterpolatedPosition(DirectX::XMMATRIX &out, float animTimeTicks, const NodeAnimChannel *pNodeAnimChannel);		// Pos補間
	void CalcInterpolatedRotation(DirectX::XMMATRIX &out, float animTimeTicks, const NodeAnimChannel *pNodeAnimChannel);		// Rot補間
	void CalcInterpolatedScaling(DirectX::XMMATRIX &out, float animTimeTicks, const NodeAnimChannel *pNodeAnimChannel);			// Scl補間
};


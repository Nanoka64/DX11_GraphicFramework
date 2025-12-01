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
#include "ModelData.h"


class ModelMesh;


// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:Model Class --- */
//
// 【?】メッシュで構成されるオブジェクト
//
// ***************************************************************************************
class Model : public GameObject
{
private:
	ModelMesh* m_pMeshes;	// メッシュ
	u_int m_MeshNum;		// メッシュ数
	std::vector<BoneInfo> m_BoneList;		// ボーンリスト
	DirectX::XMMATRIX m_GlobalInversTransformMtx;	// 初期姿勢逆行列（モデルを原点に戻す際に必要）
	Assimp::Importer m_Importer;			// インポーター
	const aiScene *m_pScene;				// シーン情報
	std::vector<MATERIAL> m_MaterialList;	// マテリアル

	// Transform関連
	VECTOR3::VEC3 m_Position;
	VECTOR3::VEC3 m_Rotation;
	VECTOR3::VEC3 m_Scale;

	VECTOR4::VEC4 m_Color;	// カラー

	// ワールド行列
	DirectX::XMMATRIX m_WorldMtx;
	Model *m_pParentMtx;	// 親 (今後Objectクラスか何かに移す)

	// ボーン変換用定数バッファ
	CB_BONES_DATA_SET *m_ConstanrBufferBonesData;
	CB_TRANSFORM_SET* m_pCBTransformSet;		// 定数バッファ(ワールド行列用)
	CB_MATERIAL_SET* m_pCBMaterialDataSet;		// 定数バッファ(マテリアル用)

	/***********アニメーション関連***********/
	std::vector<NodeInfo*> m_NodeList;					 // Nodeツリー情報
	std::unordered_map<std::string, int> m_BoneIndexMap; // Node名からBoneListのインデックスを引くためのマップ
	std::vector<AnimationData*>m_Animations;			 // 読み取ったアニメーション
	double m_AnimationTime = 0.0;						 // 現在のアニメーション時間(ticks)
	int m_CurrentAnimIndex = -1;						 // 現在のアニメーションインデックス

	bool m_IsAnimationFlag;	// アニメーションさせるかどうか（とりまデバッグ用）
	float m_AnimProcTime;	// 更新用カウンタ

#ifdef _DEBUG
	/*デバッグ用　ボーン*/
	std::vector<std::weak_ptr<GameObject>> m_pDebugBones;
	VECTOR3::VEC3 m_BoneScale;	// ボーンの大きさ
	bool m_IsDrawDebugBone;	// ボーンを表示するかどうか
	bool m_IsDrawLine;	// ライン表示にするか
#endif
public:
	Model();
	~Model();

	bool Init(RendererEngine&renderer) override;	// 初期化
	void Update(RendererEngine&renderer) override;	// 更新
	void Draw(RendererEngine&renderer) override;	// 描画
	bool Term(RendererEngine&renderer) override;	// 終了

	bool Setup(RendererEngine&render, const char *filePath);	// モデルデータのセットアップ
	bool set_TextureMap(TEXTURE_MAP mapType, int matIndex, const std::wstring &path);	// テクスチャマップ設定 Setup後に呼ぶ

	void set_Pos(VECTOR3::VEC3 pos);		 // 座標設定
	void set_RotateToRad(VECTOR3::VEC3 rot); // 回転値設定 - ラジアン指定
	void set_RotateToDeg(VECTOR3::VEC3 rot); // 回転値設定 - デグリー指定
	void set_Scale(VECTOR3::VEC3 scl);		 // スケール設定
	void set_ParentMtx(Model *parent);		 // 親を設定

	VECTOR3::VEC3 get_Pos() const;			// 座標取得
	VECTOR3::VEC3 get_RotateToRad() const ;	// 回転値取得 - ラジアン
	VECTOR3::VEC3 get_RotateToDeg() const ;	// 回転値取得 - デグリー
	VECTOR3::VEC3 get_Scale() const;		// スケール取得

	DirectX::XMMATRIX get_MtxPos()const;			// 座標取得 - 行列
	DirectX::XMMATRIX get_MtxRotate()const;			// 回転値取得 - 行列
	DirectX::XMMATRIX get_MtxScale()const;			// スケール取得 - 行列
	DirectX::XMMATRIX &get_WorldMtx();				// ワールド行列取得

	void set_Color(const VECTOR4::VEC4 &col, UINT meshIdx = -1, UINT vtxIdx = -1);	// カラー設定 
	VECTOR4::VEC4 get_MeshIdxColor(int meshIdx)const;								// カラー取得 メッシュ指定
	VECTOR4::VEC4 get_MeshAndVtxIdxColor(int meshIdx, int vtxIdx)const;				// カラー取得 メッシュ＆頂点指定
	unsigned int get_MeshNum()const { return m_MeshNum; }							// メッシュ数取得
	unsigned int get_VertexNum(unsigned int meshIdx = 0)const;						// 頂点数取得 メッシュ指定

	void BoneTransformsUpdate(RendererEngine& renderer, float timeInSeconds);

	int get_BoneIndex(const std::string& tag);	// ボーンインデックス取得

	std::vector<BoneInfo> &get_BoneList() { return m_BoneList; };						// ボーンリストの取得
	std::unordered_map<std::string, int> &get_BoneIndexMap() { return m_BoneIndexMap; }	// ボーンにアクセスするためのインデックスマップ


	void set_AnimIndex(const int idx) { m_CurrentAnimIndex = idx; };	// アニメーションインデックスの設定
private:
	void AnimationExtraction(const aiScene* scene);				// アニメーション情報抽出
	void NodeExtraction(const aiNode* pNode, int parentIdx);	// ノード情報を再帰的に抽出
	void BoneExtraction(aiMesh* mesh);							// ボーン情報抽出
	void MatrialExtraction(const aiScene* scene);				// マテリアル情報抽出 ※下のset_TextureMapで直接指定させているので要らないかも
	void SetupTransform(RendererEngine &render);				// 行列変換

	void TransformBone(float animTimeTicks,UINT nodeIdx,const DirectX::XMMATRIX& parent);// ボーン変換行列の更新
	const NodeAnimChannel* FindNodeAnim(const AnimationData*pAnim, const std::string& nodeName);	// アニメーションがあるか確認
	UINT FindPosition(float animTimeTicks, const NodeAnimChannel* pNodeAnimChannel);		// 対応するキーのインデックスを探す
	UINT FindRotation(float animTimeTicks, const NodeAnimChannel* pNodeAnimChannel);		// 対応するキーのインデックスを探す
	UINT FindScaling(float animTimeTicks, const NodeAnimChannel* pNodeAnimChannel);			// 対応するキーのインデックスを探す
	void CalcInterpolatedPosition(DirectX::XMMATRIX& out,float animTimeTicks, const NodeAnimChannel* pNodeAnimChannel);		// 補間
	void CalcInterpolatedRotation(DirectX::XMMATRIX& out,float animTimeTicks, const NodeAnimChannel* pNodeAnimChannel);		// 補間
	void CalcInterpolatedScaling(DirectX::XMMATRIX& out,float animTimeTicks,  const NodeAnimChannel* pNodeAnimChannel);		// 補間

	bool CreateBonesCBuffer(RendererEngine& render);						// ボーン変換用定数バッファの作成
	bool CreateMaterialCBuffer(RendererEngine& render);					// マテリアル用定数バッファの作成
	bool CreateTransformCBuffer(RendererEngine& render);					// トランスフォーム用定数バッファの作成


};


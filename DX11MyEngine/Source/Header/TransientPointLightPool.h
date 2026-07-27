#pragma once
#include <cstddef>
#include <cstdint>
#include <vector>
#include "VEC.h"

struct CB_PointLightData;

enum class LIGHT_FADE_CURVE
{
	LINEAR,				// 一定速度で開始値から終了値へ変化
	EASE_OUT_QUINT,		// 開始直後に大きく変化し、終了値へ緩やかに近づく
};

/// <summary>
/// 一時ポイントライトの生成情報。
/// 範囲と光量は、指定したカーブに従って開始値から終了値へ補間される。
/// </summary>
struct TransientPointLightDesc
{
	VECTOR3::VEC3 _color{ 1.0f, 1.0f, 1.0f };	// カラー

	float _startRange = 0.0f;					// 開始範囲
	float _endRange = 0.0f;						// 終了範囲
	float _startIntensity = 0.0f;				// 開始光量
	float _endIntensity = 0.0f;					// 終了光量

	float _duration = 0.1f;						// 生存時間
	float _specularIntensity = 150.0f;			// スペキュラ強度
	int   _priority = 0;						// 一時ライト間の描画優先度

	LIGHT_FADE_CURVE _curve = LIGHT_FADE_CURVE::EASE_OUT_QUINT;
};

/// <summary>
/// 再利用されたライトを安全に識別するためのハンドル
/// </summary>
struct TransientLightHandle
{
	uint32_t _index = UINT32_MAX;	// プール内のスロット番号
	uint32_t _generation = 0;		// 同じスロットが再利用されたかを判別する番号

	bool IsValid() const
	{
		return _index != UINT32_MAX;
	}
};

/// <summary>
/// 一時ポイントライトの実行時情報
/// </summary>
struct TransientPointLightRuntime
{
	TransientPointLightDesc _desc;	// 生成時に渡された変更しない設定
	VECTOR3::VEC3 _position{};		// ワールド座標

	float _elapsed = 0.0f;			// 生成されてからの経過時間
	uint32_t _generation = 0;		// 現在このスロットを使用している世代
	bool _active = false;			// 使用中のスロットならtrue
};

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:TransientPointLightPool Class --- */
//
// 【?】一時ポイントライトの生成、更新、再利用を管理する
//
// ***************************************************************************************
class TransientPointLightPool
{
public:
	// 従来の爆発ライトプールの最大数と同じ値を初期容量とする
	static constexpr uint32_t DEFAULT_CAPACITY = 50;

	explicit TransientPointLightPool(uint32_t capacity = DEFAULT_CAPACITY);

	/// <summary>
	/// 空きスロットを取得してライトを開始する
	/// </summary>
	TransientLightHandle Play(
		const VECTOR3::VEC3& position,
		const TransientPointLightDesc& desc);

	/// <summary>
	/// ハンドルが指すライトを停止する
	/// </summary>
	bool Stop(TransientLightHandle handle);

	/// <summary>
	/// すべてのライトを停止してスロットを空にする
	/// </summary>
	void Clear();

	/// <summary>
	/// ライトを更新し、描画用データを出力配列へ追加する。
	/// outputには常設ライトが入っている場合があるため、maxOutputCountを超えない範囲だけ追加する。
	/// </summary>
	void Update(
		float deltaTime,
		std::vector<CB_PointLightData>& output,
		std::size_t maxOutputCount);

	uint32_t get_ActiveCount() const { return m_ActiveCount; }
	uint32_t get_Capacity() const { return static_cast<uint32_t>(m_Slots.size()); }

private:
	std::vector<TransientPointLightRuntime> m_Slots;	// ライト本体を保持する固定数のスロット
	std::vector<uint32_t> m_FreeIndices;				// 未使用スロットの番号。Play時に末尾から取り出す
	std::vector<uint32_t> m_ActiveIndices;				// Update中だけ使用するアクティブスロット一覧
	uint32_t m_ActiveCount = 0;							// 全スロットを走査せず使用数を取得するための値

	// インデックスと世代番号の両方が一致する場合のみ有効とする
	bool IsAlive(TransientLightHandle handle) const;
	void Release(uint32_t index);
	void RebuildFreeIndices();

	static float EvaluateCurve(LIGHT_FADE_CURVE curve, float rate);
	static uint32_t NextGeneration(uint32_t generation);
};

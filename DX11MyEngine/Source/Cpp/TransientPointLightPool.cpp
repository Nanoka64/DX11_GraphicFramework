#include "pch.h"
#include "TransientPointLightPool.h"

namespace
{
	constexpr float MIN_DURATION = 0.0001f;
}

TransientPointLightPool::TransientPointLightPool(uint32_t capacity)
	: m_Slots(capacity)
{
	// 実行中のPlay、Stop、Updateで再確保が発生しないようにしておく
	m_FreeIndices.reserve(capacity);
	m_ActiveIndices.reserve(capacity);
	RebuildFreeIndices();
}

TransientLightHandle TransientPointLightPool::Play(
	const VECTOR3::VEC3& position,
	const TransientPointLightDesc& desc)
{
	// 0に近い時間では補間できないため、生成失敗として扱う
	if (!std::isfinite(desc._duration) ||
		desc._duration < MIN_DURATION ||
		m_FreeIndices.empty())
	{
		return {};
	}

	const uint32_t index = m_FreeIndices.back();
	m_FreeIndices.pop_back();

	auto& slot = m_Slots[index];

	// 同じスロットの古いハンドルと区別できるよう、使用開始ごとに世代を進める
	const uint32_t generation = NextGeneration(slot._generation);

	// 前回使用時の色や経過時間を残さないよう、スロット全体を初期化する
	slot = {};
	slot._desc = desc;
	slot._position = position;
	slot._generation = generation;
	slot._active = true;

	++m_ActiveCount;
	return { index, generation };
}

bool TransientPointLightPool::Stop(TransientLightHandle handle)
{
	if (!IsAlive(handle))
	{
		return false;
	}

	Release(handle._index);
	return true;
}

void TransientPointLightPool::Clear()
{
	for (auto& slot : m_Slots)
	{
		// 世代番号は残し、Clear前に発行したハンドルが再利用後に一致しないようにする
		const uint32_t generation = slot._generation;
		slot = {};
		slot._generation = generation;
	}

	m_ActiveCount = 0;
	m_ActiveIndices.clear();
	RebuildFreeIndices();
}

void TransientPointLightPool::Update(
	float deltaTime,
	std::vector<CB_PointLightData>& output,
	std::size_t maxOutputCount)
{
	const float safeDeltaTime =
		std::isfinite(deltaTime) ? std::max(deltaTime, 0.0f) : 0.0f;
	const std::size_t availableCount =
		output.size() < maxOutputCount ? maxOutputCount - output.size() : 0;

	// 描画対象の選別と寿命更新で共通利用するため、使用中の番号だけを集める
	m_ActiveIndices.clear();
	for (uint32_t index = 0; index < m_Slots.size(); ++index)
	{
		if (m_Slots[index]._active)
		{
			m_ActiveIndices.push_back(index);
		}
	}

	// 描画枠が不足した場合は、優先度が高いライトから採用する
	if (availableCount > 0 && m_ActiveIndices.size() > availableCount)
	{
		std::partial_sort(
			m_ActiveIndices.begin(),
			m_ActiveIndices.begin() + availableCount,
			m_ActiveIndices.end(),
			[this](uint32_t lhs, uint32_t rhs)
			{
				return m_Slots[lhs]._desc._priority >
					m_Slots[rhs]._desc._priority;
			});
	}

	const std::size_t submitCount =
		std::min(availableCount, m_ActiveIndices.size());

	for (std::size_t i = 0; i < submitCount; ++i)
	{
		const auto& slot = m_Slots[m_ActiveIndices[i]];

		// 経過時間を0～1へ正規化し、指定されたカーブを補間率へ変換する
		const float rate = std::clamp(
			slot._elapsed / slot._desc._duration,
			0.0f,
			1.0f);
		const float fadeRate = EvaluateCurve(slot._desc._curve, rate);

		CB_PointLightData data{};
		data.Pos = slot._position;
		data.Range = std::max(
			0.0f,
			Tool::Lerp(slot._desc._startRange, slot._desc._endRange, fadeRate));
		data.DiffuseColor = slot._desc._color;
		data.DiffuseIntensity = std::max(
			0.0f,
			Tool::Lerp(
				slot._desc._startIntensity,
				slot._desc._endIntensity,
				fadeRate));
		data.SpecularColor = VECTOR3::VEC3(1.0f);
		data.SpecularIntensity = std::max(0.0f, slot._desc._specularIntensity);

		output.push_back(data);
	}

	// 描画対象から外れたライトも寿命は進める
	for (const uint32_t index : m_ActiveIndices)
	{
		auto& slot = m_Slots[index];
		slot._elapsed += safeDeltaTime;

		if (slot._elapsed >= slot._desc._duration)
		{
			Release(index);
		}
	}
}

bool TransientPointLightPool::IsAlive(TransientLightHandle handle) const
{
	if (!handle.IsValid() || handle._index >= m_Slots.size())
	{
		return false;
	}

	const auto& slot = m_Slots[handle._index];
	return slot._active && slot._generation == handle._generation;
}

void TransientPointLightPool::Release(uint32_t index)
{
	auto& slot = m_Slots[index];
	if (!slot._active)
	{
		return;
	}

	// 設定値は消すが、次回Playで世代を進めるため現在の世代番号だけ残す
	const uint32_t generation = slot._generation;
	slot = {};
	slot._generation = generation;

	// 使用済みスロットを空き一覧へ戻し、次回のPlayで再利用できるようにする
	m_FreeIndices.push_back(index);
	--m_ActiveCount;
}

void TransientPointLightPool::RebuildFreeIndices()
{
	m_FreeIndices.clear();

	// pop_back()で小さいインデックスから取得できる順番に積む
	for (uint32_t index = static_cast<uint32_t>(m_Slots.size());
		index > 0;
		--index)
	{
		m_FreeIndices.push_back(index - 1);
	}
}

float TransientPointLightPool::EvaluateCurve(
	LIGHT_FADE_CURVE curve,
	float rate)
{
	switch (curve)
	{
	case LIGHT_FADE_CURVE::EASE_OUT_QUINT:
		return Tool::Easing::EaseOutQuint(rate);

	case LIGHT_FADE_CURVE::LINEAR:
	default:
		return rate;
	}
}

uint32_t TransientPointLightPool::NextGeneration(uint32_t generation)
{
	++generation;

	// 0は初期状態として扱うため、オーバーフロー時は1へ戻す
	if (generation == 0)
	{
		generation = 1;
	}

	return generation;
}

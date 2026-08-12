#pragma once
#include "Tween.h"

class TweenManager
{
private:
	using TweenVariant = std::variant<
		Tool::Tween<float>,
		Tool::Tween<double>,
		Tool::Tween<int>,
		Tool::Tween<VECTOR2::VEC2>,
		Tool::Tween<VECTOR3::VEC3>,
		Tool::Tween<VECTOR4::VEC4>
	>;

	std::vector<TweenVariant> m_Tweens;

public:
	TweenManager();
	~TweenManager();

	/// <summary>
	/// ‰Šú‰»
	/// </summary>
	/// <returns>¬Œ÷/¸”s</returns>
	bool Init();

	/// <summary>
	/// XV
	/// </summary>
	void Update(float deltaTime);


	template<typename T>
	void AddTween(T* _pTarget, const T& _start, const T& _end, float _duration, Tool::TweenType _tweenType)
	{
		static_assert(
			std::is_same_v<T, float> ||
			std::is_same_v<T, double> ||
			std::is_same_v<T, int> ||
			std::is_same_v<T, VECTOR2::VEC2> ||
			std::is_same_v<T, VECTOR3::VEC3> ||
			std::is_same_v<T, VECTOR4::VEC4>,
			"TweenManager‚Å‘Î‰‚µ‚Ä‚¢‚È‚¢Œ^‚Å‚·");

		m_Tweens.emplace_back(
			std::in_place_type<Tool::Tween<T>>,
			_pTarget,
			_start,
			_end,
			_duration,
			_tweenType);
	}

	void ClearTweens() { m_Tweens.clear(); }
	void RemoveFinishedTweens();

private:
	// ƒRƒs[‹Ö~
	TweenManager(const TweenManager&) = delete;
	TweenManager& operator=(const TweenManager&) = delete;
	// ------------------------------------------------------
};



#pragma once
class TweenManager
{
private:
	std::vector<class Tween*> m_Tweens;

public:
	TweenManager();
	~TweenManager();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <returns>成功/失敗</returns>
	bool Init();

	/// <summary>
	/// 更新
	/// </summary>
	void Update(float deltaTime);

private:
	// コピー禁止
	TweenManager(const TweenManager&) = delete;
	TweenManager& operator=(const TweenManager&) = delete;
	// ------------------------------------------------------

};


#pragma once

namespace Tool
{
    /// <summary>
    /// 補間の種類
    /// </summary>
    enum class TweenType
    {
        LINEAR,     // 線形補間
        EASE_IN,    // 加速
        EASE_OUT,   // 減速
		BOUNCE      // 跳ねるような補間
	};

    class Tween
    {
    private:
        float* m_Target;    // 参照する変数のポインタ
		TweenType m_Type;   // 補間の種類

		float m_Start;      // 開始値
        float m_End;        // 終了値

        float m_Duration;   // 補間にかかる時間
        float m_Elapsed;    // 経過時間

    public:
        void Update(float deltaTime);
		bool IsFinished() const { return m_Elapsed >= m_Duration; }
    };
}
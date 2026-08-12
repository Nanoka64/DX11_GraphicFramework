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

    // ***************************************************************************************
    // ---------------------------------------------------------------------------------------
    /* --- @:Tween Class --- */
    //
    // 【?】開始点から終了点までの補間を行うクラス
    //
    // ***************************************************************************************
    template<typename T>
    class Tween
    {
    private:
        T* m_Target;    // 参照する変数のポインタ
        TweenType m_Type;   // 補間の種類

        T m_Start;      // 開始値
        T m_End;        // 終了値

        float m_Duration;   // 補間にかかる時間
        float m_Elapsed;    // 経過時間

    public:
        Tween();
        Tween(T* _pTarget, T _start, T _end, float _duration, TweenType _tweenType);
        ~Tween() = default;

        void Update(float deltaTime);
        bool IsFinished() const { return m_Elapsed >= m_Duration; }
    };

    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <typeparam name="T"></typeparam>
    template<typename T> Tween<T>::Tween() :
        m_Target(nullptr),
        m_Type(TweenType::LINEAR),
        m_Start(0.0f),
        m_End(0.0f),
        m_Duration(0.0f),
        m_Elapsed(0.0f)
    {
    }

    /// <summary>
    /// デストラクタ
    /// </summary>
    /// <typeparam name="T"></typeparam>
    /// <param name="_pTarget">変更する値</param>
    /// <param name="_start">開始値</param>
    /// <param name="_end">終了値</param>
    /// <param name="_duration">補間にかかる時間</param>
    /// <param name="_tweenType">補間の方法</param>
    template<typename T> Tween<T>::Tween(T* _pTarget, T _start, T _end, float _duration, TweenType _tweenType) :
        m_Target(_pTarget),
        m_Type(_tweenType),
        m_Start(_start),
        m_End(_end),
        m_Duration(_duration),
        m_Elapsed(0.0f)
    {
    }

    /// <summary>
    /// 更新処理
    /// </summary>
    /// <typeparam name="T"></typeparam>
    /// <param name="deltaTime"></param>
    template<typename T> void Tween<T>::Update(float deltaTime)
    {
        m_Elapsed += deltaTime;

        float t = m_Elapsed / m_Duration;
        t = std::clamp(t, 0.0f, 1.0f);

        switch (m_Type)
        {
            // 線形補間はそのまま
        case Tool::TweenType::LINEAR:           break;

        case Tool::TweenType::EASE_IN:
            t = Tool::Easing::EaseInQuad(t);    break;

            // 減速補間
        case Tool::TweenType::EASE_OUT:
            t = Tool::Easing::EaseOutQuad(t);   break;

            // 跳ねるような補間
        case Tool::TweenType::BOUNCE:
            t = Tool::Easing::EaseOutBounce(t); break;

        default:
            break;
        }

        *m_Target = Tool::Lerp(m_Start, m_End, t);
    }
};
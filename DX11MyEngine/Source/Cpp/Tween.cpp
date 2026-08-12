#include "pch.h"
#include "Tween.h"
//
//Tool::Tween::Tween():
//    m_Target(nullptr),
//    m_Type(TweenType::LINEAR),
//    m_Start(0.0f),
//    m_End(0.0f),
//    m_Duration(0.0f),
//    m_Elapsed(0.0f) 
//{}
//
//Tool::Tween::Tween(float* _pTarget, float _start, float _end, float _duration, TweenType _tweenType) :
//    m_Target(_pTarget),
//    m_Type(_tweenType),
//    m_Start(_start),
//    m_End(_end),
//    m_Duration(_duration),
//    m_Elapsed(0.0f) 
//{}
//
//void Tool::Tween::Update(float deltaTime)
//{
//    m_Elapsed += deltaTime;
//
//    float t = m_Elapsed / m_Duration;
//    t = std::clamp(t, 0.0f, 1.0f);
//
//    switch (m_Type)
//    {
//        // ê¸å`ï‚ä‘ÇÕÇªÇÃÇ‹Ç‹
//    case Tool::TweenType::LINEAR:           break;
//        
//    case Tool::TweenType::EASE_IN:          
//        t = Tool::Easing::EaseInQuad(t);    break;
//
//        // å∏ë¨ï‚ä‘
//    case Tool::TweenType::EASE_OUT:        
//		t = Tool::Easing::EaseOutQuad(t);   break;
//
//        // íµÇÀÇÈÇÊÇ§Ç»ï‚ä‘
//    case Tool::TweenType::BOUNCE:           
//        t = Tool::Easing::EaseOutBounce(t); break;
//
//    default:
//        break;
//    }
//
//    *m_Target = Lerp(m_Start, m_End, t);
//}
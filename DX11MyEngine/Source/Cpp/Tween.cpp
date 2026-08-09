#include "pch.h"
#include "Tween.h"

void Tool::Tween::Update(float deltaTime)
{
    m_Elapsed += deltaTime;

    float t = m_Elapsed / m_Duration;
    t = std::clamp(t, 0.0f, 1.0f);

    switch (m_Type)
    {
        // ê¸å`ï‚ä‘ÇÕÇªÇÃÇ‹Ç‹
    case Tool::TweenType::LINEAR:           break;
        
    case Tool::TweenType::EASE_IN:          
        t = Tool::Easing::EaseInQuad(t);    break;

        // å∏ë¨ï‚ä‘
    case Tool::TweenType::EASE_OUT:        
		t = Tool::Easing::EaseOutQuad(t);   break;

        // íµÇÀÇÈÇÊÇ§Ç»ï‚ä‘
    case Tool::TweenType::BOUNCE:           
        t = Tool::Easing::EaseOutBounce(t); break;

    default:
        break;
    }

    *m_Target = Lerp(m_Start, m_End, t);
}
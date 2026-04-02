#include <LE/Events/RenderEvent.hpp>

namespace le
{
    RenderEvent::RenderEvent(const float deltaTime)
        :
        Event(false),
        m_DeltaTime(deltaTime)
    {}

    float RenderEvent::GetDeltaTime() const
    {
        return m_DeltaTime;
    }
}
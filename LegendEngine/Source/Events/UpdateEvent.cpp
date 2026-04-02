#include <LE/Events/UpdateEvent.hpp>

namespace le
{
    UpdateEvent::UpdateEvent(const float deltaTime)
        :
        Event(false),
        m_DeltaTime(deltaTime)
    {}

    float UpdateEvent::GetDeltaTime() const
    {
        return m_DeltaTime;
    }
}
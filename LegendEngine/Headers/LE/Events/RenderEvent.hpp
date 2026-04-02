#pragma once

#include <LE/Events/Event.hpp>

namespace le
{
    class RenderEvent : public Event
    {
    public:
        explicit RenderEvent(float deltaTime);

        [[nodiscard]] float GetDeltaTime() const;
    private:
        float m_DeltaTime;
    };
}
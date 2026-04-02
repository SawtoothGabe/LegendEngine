#pragma once

#include <LE/Events/Event.hpp>

namespace le
{
    class UpdateEvent : public Event
    {
    public:
        explicit UpdateEvent(float deltaTime);

        [[nodiscard]] float GetDeltaTime() const;
    private:
        float m_DeltaTime;
    };
}
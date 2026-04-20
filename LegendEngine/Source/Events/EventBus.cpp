#include <LE/Events/EventBus.hpp>

#include <ranges>

namespace le
{
    void EventBus::Subscribe(const std::type_index eventID, EventBusSubscriber& subscriber)
    {
        std::unique_lock lock(m_mutex);

        auto& [subscribersMutex, subscribers] = m_events[eventID];
        if (std::ranges::find(subscribers, &subscriber)
            != subscribers.end())
            return;

        subscribers.push_back(&subscriber);
    }

    void EventBus::Unsubscribe(const std::type_index eventID, EventBusSubscriber& subscriber)
    {
        std::shared_lock lock(m_mutex);
        auto& [subscribersMutex, subscribers] = m_events[eventID];

        std::unique_lock subLock(subscribersMutex);
        std::erase(subscribers, &subscriber);
    }

    void EventBus::UnsubscribeAll(EventBusSubscriber& subscriber)
    {
        std::shared_lock lock(m_mutex);

        for (auto& [subscribersMutex, subscribers] : m_events | std::views::values)
        {
            std::unique_lock subLock(subscribersMutex);
            std::erase(subscribers, &subscriber);
        }
    }
}

#pragma once

#include <mutex>
#include <shared_mutex>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include <LE/Events/EventBusSubscriber.hpp>

namespace le
{
    class Event;
    class EventBus final
    {
        friend class EventBusSubscriber;
    public:
        template<typename T>
            requires std::is_base_of_v<Event, T>
        void DispatchEvent(const T& event)
        {
            std::shared_lock lock(m_mutex);
            auto& [subscribersMutex, subscribers] = m_events[typeid(T)];

            std::shared_lock lock2(subscribersMutex);
            for (EventBusSubscriber* subscriber : subscribers)
            {
                subscriber->DispatchEvent(event);
                if (event.IsCanceled())
                    return;
            }
        }
    protected:
        void Subscribe(std::type_index eventID, EventBusSubscriber& subscriber);
        void Unsubscribe(std::type_index eventID, EventBusSubscriber& subscriber);
        void UnsubscribeAll(EventBusSubscriber& subscriber);
    private:
        struct EventType
        {
            std::shared_mutex subscribersMutex;
            std::vector<EventBusSubscriber*> subscribers;
        };

        std::shared_mutex m_mutex;
        std::unordered_map<std::type_index, EventType> m_events{};
    };
}

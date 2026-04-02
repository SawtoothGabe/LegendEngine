#include <LE/Events/EventBusSubscriber.hpp>

#include <LE/Events/EventBus.hpp>

namespace le
{
    EventBusSubscriber::EventBusSubscriber(EventBus& eventBus)
        :
        m_EventBus(eventBus)
    {}

    EventBusSubscriber::~EventBusSubscriber()
    {
        m_EventBus.UnsubscribeAll(*this);
    }

    void EventBusSubscriber::AddSubscription(const std::type_index id)
    {
        m_EventBus.Subscribe(id, *this);
    }

    void EventBusSubscriber::RemoveSubscription(const std::type_index id)
    {
        m_EventBus.Unsubscribe(id, *this);
    }
}

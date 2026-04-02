#include <LE/Events/Event.hpp>
#include <LE/IO/Logger.hpp>

namespace le
{
    Event::Event(const bool cancelable)
        :
        m_Cancelable(cancelable)
    {}

    void Event::SetCanceled(const bool isCancelled)
    {
        if (!m_Cancelable && isCancelled)
        {
            LE_WARN("An event was set to be canceled, but it was not cancelable");
            return;
        }

        m_IsCancelled = isCancelled;
    }

    bool Event::IsCanceled() const
    {
        return m_IsCancelled;
    }

    bool Event::IsCancelable() const
    {
        return m_Cancelable;
    }
}
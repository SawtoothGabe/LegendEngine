#pragma once

namespace le
{
    class Event
    {
    public:
        explicit Event(bool cancelable = true);

        void SetCanceled(bool isCancelled);

        [[nodiscard]] bool IsCanceled() const;
        [[nodiscard]] bool IsCancelable() const;
    private:
        bool m_Cancelable;
        bool m_IsCancelled = false;
    };
}
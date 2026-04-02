#include <LE/Common/Assert.hpp>
#include <LE/Components/Component.hpp>
#include <LE/Components/ComponentStorage.hpp>

namespace le
{
    ComponentStorage::ComponentStorage(const size_t componentSize)
        :
        m_componentSize(componentSize)
    {}

    ComponentStorage::ComponentStorage(ComponentStorage&& other) noexcept
        :
        m_componentSize(other.m_componentSize),
        m_count(other.m_count),
        m_componentData(std::move(other.m_componentData))
    {
        other.m_count = 0;
    }

    ComponentStorage::~ComponentStorage()
    {
        for (size_t i = 0; i < m_count; i++)
            static_cast<Component*>(GetData(i))->~Component();
    }

    void* ComponentStorage::GetData(const size_t index)
    {
        LE_ASSERT(index < m_count, "Index out of range");
        return m_componentData.data() + m_componentSize * index;
    }

    void* ComponentStorage::Allocate()
    {
        const size_t offset = m_componentData.size();
        m_componentData.resize(offset + m_componentSize);
        m_count++;

        return m_componentData.data() + offset;
    }

    void ComponentStorage::SwapAndPop(const size_t index, const bool destructor)
    {
        if (m_count == 0)
            return;

        if (m_count == 1)
        {
            if (destructor)
                reinterpret_cast<Component*>(m_componentData.data())->~Component();

            m_componentData.clear();
            m_count = 0;
            return;
        }

        void* target = GetData(index);
        const void* last = GetData(m_count - 1);

        if (destructor)
        {
            LE_ASSERT(target != nullptr, "Target is nullptr");
            static_cast<Component*>(target)->~Component();
        }

        memcpy(target, last, m_componentSize);

        m_componentData.resize(m_componentData.size() - m_componentSize);
        m_count--;
    }

    size_t ComponentStorage::GetElementSize() const
    {
        return m_componentSize;
    }
}
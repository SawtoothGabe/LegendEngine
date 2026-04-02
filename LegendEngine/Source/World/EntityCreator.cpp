#include <LE/World/EntityCreator.hpp>

namespace le
{
    const std::unordered_map<size_t, ComponentStorage>& EntityCreator::GetComponents() const
    {
        return m_components;
    }

    UID EntityCreator::GetUID() const
    {
        return m_UID;
    }
}

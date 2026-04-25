#include <LE/World/EntityCreator.hpp>

namespace le
{
    const Archetype::ComponentsType& EntityCreator::GetComponents() const
    {
        return m_components;
    }

    UID EntityCreator::GetUID() const
    {
        return m_UID;
    }
}

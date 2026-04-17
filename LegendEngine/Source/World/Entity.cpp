#include <LE/World/Entity.hpp>

namespace le
{
    Entity::operator UID() const
    {
        return uid;
    }

    Entity::Entity(Scene& scene)
        :
        m_pScene(&scene)
    {}
}

#pragma once
#include <typeindex>
#include <unordered_map>
#include <LE/Common/UID.hpp>
#include <LE/Components/ComponentStorage.hpp>

namespace le
{
    class EntityCreator
    {
    public:
        EntityCreator() = default;
        EntityCreator(EntityCreator&&) = default;
        EntityCreator(const EntityCreator&) = delete;

        template <typename T>
            requires std::is_base_of_v<Component, T>
        void AddComponent(const T& component)
        {
            size_t componentID = typeid(T).hash_code();
            LE_ASSERT(!m_components.contains(componentID), "Component added twice");

            m_components.emplace(componentID, ComponentStorage(sizeof(T)));
            memcpy(m_components.at(componentID).Allocate(), &component, sizeof(T));
        }

        const std::unordered_map<size_t, ComponentStorage>& GetComponents() const;
        UID GetUID() const;
    private:
        UID m_UID;
        std::unordered_map<size_t, ComponentStorage> m_components;
    };
}

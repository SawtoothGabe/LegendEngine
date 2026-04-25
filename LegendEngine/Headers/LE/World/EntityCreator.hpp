#pragma once

#include <unordered_map>
#include <LE/Common/UID.hpp>
#include <LE/Components/ComponentStorage.hpp>
#include <LE/World/Archetype.hpp>

namespace le
{
    class EntityCreator
    {
    public:
        EntityCreator() = default;
        EntityCreator(EntityCreator&&) noexcept = default;
        EntityCreator(const EntityCreator&) = delete;

        template <typename T>
            requires std::is_base_of_v<Component, T>
        void AddComponent(const T& component)
        {
            size_t componentID = typeid(T).hash_code();
            LE_ASSERT(!m_components.contains(componentID), "Component added twice");

            auto& storageInterface = m_components.emplace(componentID,
                std::make_unique<ComponentStorage<T>>()).first->second;
            auto& storage = ComponentStorage<T>::Cast(storageInterface);
            storage.Push(component);
        }

        [[nodiscard]] const Archetype::ComponentsType& GetComponents() const;
        [[nodiscard]] UID GetUID() const;
    private:
        UID m_UID;
        Archetype::ComponentsType m_components;
    };
}

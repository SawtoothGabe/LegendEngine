#pragma once

#include <unordered_map>
#include <vector>
#include <LE/Common/Types.hpp>
#include <LE/Common/UID.hpp>
#include <LE/Components/ComponentStorage.hpp>
#include <LE/World/ECS.hpp>

namespace le
{
    struct Archetype
    {
        using ComponentsType = std::unordered_map<size_t, Scope<IComponentStorage>>;

        std::vector<UID> entityIDs;
        std::vector<size_t> componentIDs; // Sorted component ID list
        ComponentsType componentData;

        void MoveComponentsFrom(Archetype& otherArchetype, size_t row);

        template<typename T, typename... Args>
        void AddComponentToEmptyEntity(const size_t componentID, ECS::EntityRecord& record, const UID entity,
            Args&&... args)
        {
            // If the archetype doesn't have T (which also would mean it doesn't have any at all)
            if (!componentData.contains(componentID))
            {
                // Make sure it exists
                componentData.emplace(componentID, std::make_unique<ComponentStorage<T>>());
                componentIDs.push_back(componentID);
            }

            // Here, the archetype exists and there is component storage for it.
            // It may or may not have components in it, so we act like it always does.

            record.row = entityIDs.size();
            entityIDs.push_back(entity);
            auto& storage = ComponentStorage<T>::Cast(componentData.at(componentID));
            storage.Push(args...);

            // Update the record
            record.archetypeID = ECS::ArchetypeID<T>();
        }
    };
}

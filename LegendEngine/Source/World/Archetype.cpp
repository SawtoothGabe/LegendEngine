#include <LE/World/Archetype.hpp>

namespace le
{
    void Archetype::MoveComponentsFrom(Archetype& otherArchetype, const size_t row)
    {
        // Copy over existing components
        for (size_t oldComponent : otherArchetype.componentIDs)
        {
            IComponentStorage& oldStorage = *otherArchetype.componentData.at(oldComponent);

            // Ensure the component exists
            if (!componentData.contains(oldComponent))
                componentData.emplace(oldComponent, oldStorage.CloneEmpty());

            IComponentStorage& newStorage = *componentData.at(oldComponent);

            newStorage.MoveFrom(row, oldStorage);
        }

        // Swap and pop the entity ID from the old archetype
        otherArchetype.entityIDs[row] = otherArchetype.entityIDs.back();
        otherArchetype.entityIDs.pop_back();
    }
}

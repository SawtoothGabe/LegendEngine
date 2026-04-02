#include <LE/World/Archetype.hpp>

namespace le
{
    void Archetype::MoveComponentsFrom(Archetype& otherArchetype, const size_t row)
    {
        // Copy over existing components
        for (size_t oldComponent : otherArchetype.componentIDs)
        {
            // Ensure the component exists
            componentData.try_emplace(oldComponent, otherArchetype.componentData.at(oldComponent).GetElementSize());

            ComponentStorage& oldStorage = otherArchetype.componentData.at(oldComponent);
            ComponentStorage& newStorage = componentData.at(oldComponent);

            // Copy it over
            void* data = newStorage.Allocate();
            memcpy(data, oldStorage.GetData(row), oldStorage.GetElementSize());

            // Delete it from the old archetype
            oldStorage.SwapAndPop(row, false);
        }

        // Swap and pop the entity ID from the old archetype
        otherArchetype.entityIDs[row] = otherArchetype.entityIDs.back();
        otherArchetype.entityIDs.pop_back();
    }
}

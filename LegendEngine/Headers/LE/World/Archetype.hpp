#pragma once

#include <unordered_map>
#include <vector>
#include <LE/Common/UID.hpp>
#include <LE/Components/ComponentStorage.hpp>

namespace le
{
    struct Archetype
    {
        std::vector<UID> entityIDs;
        std::vector<size_t> componentIDs; // Sorted component ID list
        std::unordered_map<size_t, ComponentStorage> componentData;

        void MoveComponentsFrom(Archetype& otherArchetype, size_t row);
    };
}

#include <LE/Common/Assert.hpp>
#include <LE/World/Archetype.hpp>
#include <LE/World/ECS.hpp>

namespace le
{
    size_t ECS::HashIDs(const std::span<size_t>& ids)
    {
        size_t seed = 0;
        for (const size_t h : ids)
            seed ^= h + 0x9E3779B97F4A7C15ULL + (seed << 12) + (seed >> 4);

        return seed;
    }

    std::vector<size_t> ECS::AddComponentToSortedIDList(const Archetype& oldArchetype, const size_t componentID)
    {
        std::vector<size_t> newComponentIDs(oldArchetype.componentIDs.size() + 1);
        LE_ASSERT(newComponentIDs.size() >= 2, "Invalid component ID vector length");

        size_t i = 0;
        for (; i < oldArchetype.componentIDs.size(); i++)
        {
            if (oldArchetype.componentIDs[i] < componentID)
            {
                newComponentIDs[i] = oldArchetype.componentIDs[i];
                continue;
            }

            newComponentIDs[i] = componentID;
            break;
        }

        if (i == oldArchetype.componentIDs.size())
            newComponentIDs[i] = componentID;

        i++;

        for (; i < newComponentIDs.size(); ++i)
            newComponentIDs[i] = oldArchetype.componentIDs[i - 1];

        return newComponentIDs;
    }
}

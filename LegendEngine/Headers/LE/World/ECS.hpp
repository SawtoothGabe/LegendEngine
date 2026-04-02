#pragma once

#include <algorithm>
#include <array>
#include <span>
#include <vector>

namespace le
{
    struct Archetype;
}

namespace le::ECS
{
    struct EntityRecord
    {
        size_t archetypeID = -1;
        size_t row = -1;
    };

    size_t HashIDs(const std::span<size_t>& ids);
    std::vector<size_t> AddComponentToSortedIDList(const Archetype& oldArchetype, size_t componentID);

    template <typename... Ts>
    std::array<size_t, sizeof...(Ts)>& GetSortedIDs()
    {
        static std::array<size_t, sizeof...(Ts)> sortedIDs = []
        {
            std::array<size_t, sizeof...(Ts)> ids = { typeid(Ts).hash_code()... };
            std::sort(ids.begin(), ids.end());

            return ids;
        }();

        return sortedIDs;
    }

    template <typename... Ts>
    size_t ArchetypeID()
    {
        static size_t id = []
        {
            auto ids = GetSortedIDs<Ts...>();
            return HashIDs(ids);
        }();

        return id;
    }
}

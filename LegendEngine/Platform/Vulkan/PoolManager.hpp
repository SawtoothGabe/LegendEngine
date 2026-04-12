#pragma once

#include <unordered_map>
#include <vector>
#include <LE/Common/UID.hpp>
#include <LE/Graphics/Types.hpp>

#include "VkDefs.hpp"

namespace le
{
    struct DescriptorSetLayout;

    class PoolManager final
    {
    public:
        static constexpr size_t GROWTH_FACTOR = 2;

        explicit PoolManager(vk::Device device, const DescriptorSetLayout& layout, size_t startSize);
        ~PoolManager();

        std::vector<DescriptorSetID> Allocate(DescriptorPoolID& outPool, size_t count);
        void Free(vk::DescriptorPool pool, size_t count, DescriptorSetID* sets);
        void ResetAllPools();
    private:
        struct Pool
        {
            vk::DescriptorPool pool;
            size_t remaining = 0;
        };

        void PopulateSizes(const DescriptorSetLayout& layout);
        Pool& FindAvailablePool(size_t allocCount);
        Pool& CreateNewPool();

        vk::Device m_device;
        std::vector<Pool> m_pools;
        size_t m_nextAllocSize;
        size_t m_startSize;

        vk::DescriptorSetLayout m_layout;
        std::vector<vk::DescriptorPoolSize> m_sizes;
    };
}

#pragma once

#include <vector>
#include <LE/Graphics/Types.hpp>

#include "VkDefs.hpp"

namespace le
{
    class PoolManager final
    {
    public:
        static constexpr size_t GROWTH_FACTOR = 2;

        explicit PoolManager(vk::Device device, DescriptorSetLayoutID layout,
            std::span<DescriptorSetLayoutBinding> bindings, size_t startSize);
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

        void PopulateSizes(std::span<DescriptorSetLayoutBinding> bindings);
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

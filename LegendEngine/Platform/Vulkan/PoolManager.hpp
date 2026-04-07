#pragma once

#include <unordered_map>
#include <vector>
#include <LE/Graphics/Types.hpp>

#include "VkDefs.hpp"

namespace le
{
    struct DescriptorSetLayout;

    class PoolManager
    {
    public:
        struct Set
        {
            vk::DescriptorSet set;
            vk::DescriptorPool pool;
            DescriptorSetLayout* pLayout;
        };

        PoolManager();

        std::vector<DescriptorSetID> Allocate(std::span<DescriptorSetLayoutID> layouts);
        void Free(Set& set);
    private:
        struct Pool
        {
            vk::DescriptorPool pool;
            std::unordered_map<vk::DescriptorType, size_t> sizes;
        };

        std::vector<Pool> m_pools;
    };
}

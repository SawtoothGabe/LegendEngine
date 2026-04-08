#include "PoolManager.hpp"

#include <DescriptorSetLayout.hpp>
#include <ranges>

namespace le
{
    PoolManager::PoolManager(const vk::Device device, const DescriptorSetLayout& layout, const size_t startSize)
        :
        m_device(device),
        m_nextAllocSize(startSize),
        m_startSize(startSize)
    {
        m_layout = layout.layout;
        PopulateSizes(layout);
    }

    PoolManager::~PoolManager()
    {
        for (const auto& [pool, remaining] : m_pools)
            m_device.destroyDescriptorPool(pool);
    }

    std::vector<DescriptorSetID> PoolManager::Allocate(const size_t count)
    {
        auto& [pool, remaining] = FindAvailablePool(count);
        remaining -= count;

        const std::vector vkLayouts(count, m_layout);

        const vk::DescriptorSetAllocateInfo allocInfo(
            pool, vkLayouts.size(), vkLayouts.data()
        );

        const std::vector<vk::DescriptorSet> sets = m_device.allocateDescriptorSets(allocInfo);
        std::vector<DescriptorSetID> setIDs;

        const auto pSets = reinterpret_cast<const DescriptorSetID*>(sets.data());
        setIDs.assign(pSets, pSets + sets.size());

        return setIDs;
    }

    void PoolManager::ResetAllPools()
    {
        for (size_t i = 0; i < m_pools.size(); ++i)
        {
            auto& [pool, remaining] = m_pools[i];
            m_device.resetDescriptorPool(pool);
            remaining = m_startSize * (i + 1);
        }
    }

    void PoolManager::PopulateSizes(const DescriptorSetLayout& layout)
    {
        m_sizes.reserve(layout.descriptorCounts.size());
        for (const auto& [type, count] : layout.descriptorCounts)
        {
            vk::DescriptorPoolSize size;
            size.type = type;
            size.descriptorCount = count * m_nextAllocSize;

            m_sizes.push_back(size);
        }
    }

    PoolManager::Pool& PoolManager::FindAvailablePool(const size_t allocCount)
    {
        for (Pool& pool : m_pools)
            if (pool.remaining >= allocCount)
                return pool;

        return CreateNewPool();
    }

    PoolManager::Pool& PoolManager::CreateNewPool()
    {
        const vk::DescriptorPoolCreateInfo info(
            {}, m_nextAllocSize, m_sizes
        );

        Pool& pool = m_pools.emplace_back(m_device.createDescriptorPool(info), m_nextAllocSize);

        m_nextAllocSize *= GROWTH_FACTOR;
        for (vk::DescriptorPoolSize size : m_sizes)
            size.descriptorCount *= GROWTH_FACTOR;

        return pool;
    }
}

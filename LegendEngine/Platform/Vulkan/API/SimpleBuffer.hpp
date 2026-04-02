#pragma once

#include <vk_mem_alloc.h>

#include "GraphicsContext.hpp"
#include "API/Buffer.hpp"

namespace le::vk
{
    class SimpleBuffer final : public Buffer
    {
    public:
        SimpleBuffer(GraphicsContext& context, UsageFlags usage, size_t size, bool createMapped);
        ~SimpleBuffer() override;

        void Update(size_t size, size_t offset, const void* data) override;
        void Resize(size_t newSize) override;
        size_t GetSize() override;

        void* GetMappedData() override;

        Desc GetDesc() override;
    private:
        TetherVulkan::GraphicsContext& m_context;

        VkBuffer m_buffer = nullptr;
        VmaAllocation m_allocation = nullptr;
        VmaAllocationInfo m_allocationInfo{};
        VkBufferUsageFlags m_usage = 0;
        VmaAllocationCreateFlags m_allocFlags = 0;
    };
}

#pragma once

#include <vector>
#include <vk_mem_alloc.h>

#include "GraphicsContext.hpp"
#include "API/Buffer.hpp"

namespace le::vk
{
    class PerFrameBuffer final : public Buffer
    {
    public:
        PerFrameBuffer(GraphicsContext& context, UsageFlags usage, size_t size);
        ~PerFrameBuffer() override;

        void Update(size_t size, size_t offset, const void* data) override;
        void Resize(size_t newSize) override;
        size_t GetSize() override;

        Desc GetDesc() override;
    private:
        void CreateBuffer(VkBuffer* pBuffer, VmaAllocation* pAlloc, VmaAllocationInfo* pAllocInfo,
            VkBufferUsageFlags bufferUsage, VmaAllocationCreateFlags allocCreateFlags, size_t size) const;
        void RecreateBuffer(size_t currentFrame);

        TetherVulkan::GraphicsContext& m_context;

        size_t m_size = 0;

        std::vector<VkBuffer> m_buffers;
        std::vector<VmaAllocation> m_allocations;
        std::vector<VmaAllocationInfo> m_allocationInfos;
        std::vector<size_t> m_bufferSizes;

        VkBufferUsageFlags m_usage = 0;
        VmaAllocationCreateFlags m_allocFlags = 0;
    };
}

#include "API/SimpleBuffer.hpp"

#include <VkDefs.hpp>
#include <LE/IO/Logger.hpp>

namespace le::vk
{
    SimpleBuffer::SimpleBuffer(GraphicsContext& context, const UsageFlags usage, const size_t size, const bool createMapped)
        :
        m_context(context.GetTetherGraphicsContext())
    {
        m_usage = ToVulkanUsageFlags(usage);
        m_allocFlags = ToVmaAllocationCreateFlags(usage, createMapped);

        VkBufferCreateInfo bufferCreateInfo{};
        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCreateInfo.size = size;
        bufferCreateInfo.usage = m_usage;

        VmaAllocationCreateInfo allocInfo{};
        allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
        allocInfo.flags = m_allocFlags;

        LE_CHECK_VK(vmaCreateBuffer(m_context.GetAllocator(), &bufferCreateInfo, &allocInfo,
            &m_buffer, &m_allocation, &m_allocationInfo));
    }

    SimpleBuffer::~SimpleBuffer()
    {
        vmaDestroyBuffer(m_context.GetAllocator(), m_buffer, m_allocation);
    }

    void SimpleBuffer::Update(const size_t size, size_t offset, const void* data)
    {
        LE_ASSERT(m_allocationInfo.pMappedData != nullptr, "Buffer was updated but not mapped");
        LE_ASSERT(size + offset <= m_allocationInfo.size, "Tried to copy too much data into buffer");
        memcpy(static_cast<uint8_t*>(m_allocationInfo.pMappedData) + offset, data, size);
    }

    void SimpleBuffer::Resize(const size_t newSize)
    {
        LE_WARN(
            "Resize called on a SimpleBuffer. SimpleBuffer functions as "
            "a single buffer that doesn't take into account per-frame "
            "updates or frequent resizing. This action recreates the buffer, "
            "which must wait for frames to finish rendering. This could cause "
            "stalls or latency spikes, so consider using a different buffer type "
            "if resizing or updates happen often."
        );

        vkDeviceWaitIdle(m_context.GetDevice());

        vmaDestroyBuffer(m_context.GetAllocator(), m_buffer, m_allocation);

        VkBufferCreateInfo bufferCreateInfo{};
        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCreateInfo.size = newSize;
        bufferCreateInfo.usage = m_usage;

        VmaAllocationCreateInfo allocInfo{};
        allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
        allocInfo.flags = m_allocFlags;

        LE_CHECK_VK(vmaCreateBuffer(m_context.GetAllocator(), &bufferCreateInfo, &allocInfo,
            &m_buffer, &m_allocation, &m_allocationInfo));
    }

    size_t SimpleBuffer::GetSize()
    {
        return m_allocationInfo.size;
    }

    Buffer::Desc SimpleBuffer::GetDesc()
    {
        return {
            m_buffer,
            m_allocationInfo.size,
        };
    }

    void* SimpleBuffer::GetMappedData()
    {
        return m_allocationInfo.pMappedData;
    }
}

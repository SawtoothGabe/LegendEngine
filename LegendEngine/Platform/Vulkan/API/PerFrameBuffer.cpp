#include "API/PerFrameBuffer.hpp"

#include <VkDefs.hpp>
#include <LE/Application.hpp>

namespace le::vk
{
    PerFrameBuffer::PerFrameBuffer(GraphicsContext& context, UsageFlags usage, size_t size)
        :
        m_context(context.GetTetherGraphicsContext()),
        m_size(size)
    {
        m_buffers.resize(Application::FRAMES_IN_FLIGHT);
        m_allocations.resize(Application::FRAMES_IN_FLIGHT);
        m_allocationInfos.resize(Application::FRAMES_IN_FLIGHT);
        m_bufferSizes.resize(Application::FRAMES_IN_FLIGHT);

        m_usage = ToVulkanUsageFlags(usage);
        m_allocFlags = ToVmaAllocationCreateFlags(usage, true);
        for (size_t i = 0; i < Application::FRAMES_IN_FLIGHT; i++)
        {
            m_bufferSizes[i] = size;
            CreateBuffer(&m_buffers[i], &m_allocations[i], &m_allocationInfos[i], m_usage, m_allocFlags, size);
        }
    }

    PerFrameBuffer::~PerFrameBuffer()
    {
        for (size_t i = 0; i < Application::FRAMES_IN_FLIGHT; i++)
            vmaDestroyBuffer(m_context.GetAllocator(), m_buffers[i], m_allocations[i]);
    }

    void PerFrameBuffer::Update(const size_t size, const size_t offset, const void* data)
    {
        const size_t currentFrame = Application::Get().GetCurrentFrame();
        const size_t allocSize = m_bufferSizes[currentFrame];

        if (allocSize != m_size)
            RecreateBuffer(currentFrame);

        void* pMappedData = m_allocationInfos[currentFrame].pMappedData;

        LE_ASSERT(pMappedData != nullptr, "Buffer was updated but not mapped");
        LE_ASSERT(size + offset <= allocSize, "Tried to copy too much data into buffer");

        memcpy(static_cast<uint8_t*>(pMappedData) + offset, data, size);
    }

    void PerFrameBuffer::Resize(const size_t newSize)
    {
        m_size = newSize;
    }

    size_t PerFrameBuffer::GetSize()
    {
        return m_bufferSizes[Application::Get().GetCurrentFrame()];
    }

    Buffer::Desc PerFrameBuffer::GetDesc()
    {
        return {
            m_buffers[Application::Get().GetCurrentFrame()],
            m_bufferSizes[Application::Get().GetCurrentFrame()],
        };
    }

    void PerFrameBuffer::CreateBuffer(VkBuffer* pBuffer, VmaAllocation* pAlloc, VmaAllocationInfo* pAllocInfo,
        const VkBufferUsageFlags bufferUsage, const VmaAllocationCreateFlags allocCreateFlags, const size_t size) const
    {
        VkBufferCreateInfo bufferCreateInfo{};
        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCreateInfo.size = size;
        bufferCreateInfo.usage = bufferUsage;

        VmaAllocationCreateInfo allocInfo{};
        allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
        allocInfo.flags = allocCreateFlags;

        LE_CHECK_VK(vmaCreateBuffer(m_context.GetAllocator(), &bufferCreateInfo, &allocInfo,
            pBuffer, pAlloc, pAllocInfo));
    }

    void PerFrameBuffer::RecreateBuffer(const size_t currentFrame)
    {
        vmaDestroyBuffer(m_context.GetAllocator(), m_buffers[currentFrame], m_allocations[currentFrame]);
        m_bufferSizes[currentFrame] = m_size;
        CreateBuffer(&m_buffers[currentFrame], &m_allocations[currentFrame],
            &m_allocationInfos[currentFrame], m_usage, m_allocFlags, m_size);
    }
}

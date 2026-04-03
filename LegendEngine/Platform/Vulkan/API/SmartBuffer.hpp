#pragma once

#include <atomic>
#include <BufferStager.hpp>
#include <LE/Events/EventBusSubscriber.hpp>

#include "GraphicsContext.hpp"
#include "API/Buffer.hpp"

namespace le::vk
{
    class SmartBuffer final : public Buffer
    {
    public:
        SmartBuffer(GraphicsContext& context, UsageFlags usage);
        ~SmartBuffer() override;

        void Update(size_t size, size_t offset, const void* data) override;
        void Resize(size_t newSize) override;
        size_t GetSize() override;

        Desc GetDesc() override;
    private:
        struct BufferDesc
        {
            std::atomic<VkBuffer> buffer = nullptr;
            std::atomic<VmaAllocation> allocation = nullptr;
            std::atomic<size_t> size = 0;
        };

        VkBuffer CreateBuffer(BufferDesc* target, size_t size) const;
        void DestroyBuffer(BufferDesc& buffer) const;
        void DeleteStagingBuffer();
        void DeleteUnusedBuffers();

        TetherVulkan::GraphicsContext& m_context;

        VkBufferUsageFlags m_usage;

        BufferDesc m_buffer1;
        BufferDesc m_buffer2;
        BufferStager m_stager;

        std::mutex m_updateMutex;
        std::atomic<BufferDesc*> m_currentBuffer = nullptr;
        std::atomic<BufferDesc*> m_updatedBuffer = nullptr;
        size_t m_framesSinceDeletion = 0;

        EventBusSubscriber m_sub;
    };
}

#pragma once

#include <LE/Events/EventBusSubscriber.hpp>
#include <LE/Graphics/Explicit/Buffer.hpp>
#include <LE/Graphics/Explicit/BufferStager.hpp>
#include <LE/Graphics/Explicit/ExplicitResources.hpp>

namespace le
{
    class SmartBuffer : public Buffer
    {
    public:
        SmartBuffer(const ExplicitResources& resources, BufferUsageFlagBits usage);
        ~SmartBuffer() override;

        void Update(std::size_t size, std::size_t offset, const void* data, size_t) override;
        void Resize(std::size_t newSize) override;

        Desc GetDesc(size_t) const override;
    private:
        struct BufferDesc
        {
            uint64_t buffer = 0;
            size_t size = 0;
        };

        BufferID CreateBuffer(BufferDesc& target, size_t size) const;
        void DestroyBuffer(BufferDesc& buffer) const;
        void DeleteUnusedBuffers();

        ExplicitDriver& m_driver;
        QueueID m_queue;
        BufferUsageFlagBits m_usage;

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

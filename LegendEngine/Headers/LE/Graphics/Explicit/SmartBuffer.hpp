#pragma once

#include <LE/Events/EventBusSubscriber.hpp>
#include <LE/Graphics/Explicit/Buffer.hpp>
#include <LE/Graphics/Explicit/ExplicitRenderer.hpp>

namespace le
{
    class SmartBuffer : public Buffer
    {
    public:
        SmartBuffer(ExplicitRenderer& renderer, BufferUsageFlags usage);

        void Update(std::size_t size, std::size_t offset, const void* data) override;
        void Resize(std::size_t newSize) override;

        Desc GetDesc() const override;
    private:
        struct BufferDesc
        {
            std::atomic<uint64_t> buffer;
            std::atomic_size_t size;
        };

        BufferID CreateBuffer(BufferDesc& target, size_t size);
        void DestroyBuffer(BufferDesc& buffer);
        void DeleteUnusedBuffers();

        ExplicitDriver& m_driver;
        BufferUsageFlags m_usage;

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

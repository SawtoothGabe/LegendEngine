#pragma once
#include <LE/Graphics/Explicit/ExplicitDriver.hpp>
#include <LE/Graphics/Explicit/ExplicitRenderer.hpp>

namespace le
{
    class BufferStager final
    {
    public:
        explicit BufferStager(const ExplicitRenderer& renderer);
        ~BufferStager();

        void CreateStagingBuffer(const BufferID& target, size_t targetSize);
        void Upload(const void* data, size_t targetSize);
        void DeleteStagingBuffer();

        bool IsFenceSignaled() const;
        bool HasStagingBuffer() const;

        void Wait();
    private:
        void CreateCommandBuffer();
        void RecordCommandBuffer(const BufferID& target, size_t targetSize) const;
        void CreateFence();

        ExplicitDriver& m_driver;
        std::mutex& m_transferMutex;
        QueueID m_queue;
        CommandPoolID m_commandPool;
        CommandBufferID m_commandBuffer;

        BufferID m_stagingBuffer;
        FenceID m_fence;
    };
}

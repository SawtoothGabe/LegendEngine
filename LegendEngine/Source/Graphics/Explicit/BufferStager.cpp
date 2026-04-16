#include <LE/Graphics/Explicit/BufferStager.hpp>

namespace le
{
    BufferStager::BufferStager(const ExplicitResources& resources)
        :
        m_driver(resources.GetDriver()),
        m_transferMutex(resources.GetTransferMutex()),
        m_queue(resources.GetTransferQueue()),
        m_commandPool(resources.GetTransferPool())
    {
        CreateCommandBuffer();
    }

    BufferStager::~BufferStager()
    {
        DeleteStagingBuffer();

        m_driver.FreeCommandBuffers(m_commandPool, 1, &m_commandBuffer);
    }

    void BufferStager::CreateStagingBuffer(const BufferID& target, const size_t targetSize)
    {
        if (!m_fence)
            CreateFence();

        Wait();
        m_driver.DestroyBuffer(m_stagingBuffer);
        m_stagingBuffer = m_driver.CreateBuffer(BufferUsageFlagBits::TRANSFER_SRC, targetSize, true);

        RecordCommandBuffer(target, targetSize);
    }

    void BufferStager::Upload(const void* data, const size_t targetSize)
    {
        memcpy(m_driver.GetMappedBufferData(m_stagingBuffer), data, targetSize);

        SubmitInfo info;
        info.commandBuffer = m_commandBuffer;

        m_driver.ResetFences(1, &m_fence);
        std::scoped_lock lock(m_transferMutex);
        m_driver.QueueSubmit(m_queue, info);
    }

    void BufferStager::DeleteStagingBuffer()
    {
        m_driver.DestroyBuffer(m_stagingBuffer);
        m_driver.DestroyFence(m_fence);
        m_stagingBuffer = {};
        m_fence = {};
    }

    bool BufferStager::IsFenceSignaled() const
    {
        return m_driver.IsFenceSignaled(m_fence);
    }

    bool BufferStager::HasStagingBuffer() const
    {
        return static_cast<bool>(m_stagingBuffer);
    }

    void BufferStager::Wait()
    {
        m_driver.WaitForFences(1, &m_fence);
    }

    void BufferStager::CreateCommandBuffer()
    {
        m_commandBuffer = m_driver.AllocateCommandBuffer(m_commandPool);
    }

    void BufferStager::RecordCommandBuffer(const BufferID& target, const size_t targetSize) const
    {
        m_driver.ResetCommandBuffer(m_commandBuffer);
        m_driver.BeginCommandBuffer(m_commandBuffer, false);
        {
            BufferCopy copy;
            copy.size = targetSize;

            m_driver.CmdCopyBuffer(m_commandBuffer, m_stagingBuffer, target,
                std::span(&copy, 1));
        }
        m_driver.EndCommandBuffer(m_commandBuffer);
    }

    void BufferStager::CreateFence()
    {
        m_fence = m_driver.CreateFence(true);
    }
}

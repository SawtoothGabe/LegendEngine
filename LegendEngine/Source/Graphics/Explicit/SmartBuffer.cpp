#include <LE/Application.hpp>
#include <LE/Events/RenderEvent.hpp>
#include <LE/Graphics/Explicit/SmartBuffer.hpp>

namespace le
{
    SmartBuffer::SmartBuffer(const ExplicitResources& resources, const BufferUsageFlagBits usage)
        :
        m_driver(resources.GetDriver()),
        m_queue(resources.GetGraphicsQueue()),
        m_usage(usage),
        m_stager(resources),
        m_sub(Application::Get().GetEventBus())
    {
        m_sub.AddEventHandler<RenderEvent>([this](const RenderEvent&) { DeleteUnusedBuffers(); });
    }

    SmartBuffer::~SmartBuffer()
    {
        // Synchronization shouldn't be necessary here
        // An object being destroyed while it is still in use is a bug

        DestroyBuffer(m_buffer1);
        DestroyBuffer(m_buffer2);
    }

    void SmartBuffer::Update(const std::size_t size, const std::size_t offset, const void* data)
    {
        if (size == 0 || data == nullptr)
            return;

        LE_ASSERT(offset == 0, "Offset must be zero when using a SmartBuffer");

        BufferDesc* buffer = m_updatedBuffer.load();
        if (buffer) // An update is in progress
            m_stager.Wait(); // Wait on the stager, then do this update

        // It's possible that another update happens here and then this locks
        std::scoped_lock lock(m_updateMutex);

        buffer = m_updatedBuffer.load();

        BufferID bufferID;
        if (buffer) // An update is in progress
        {
            if (m_stager.HasStagingBuffer())
            {
                // Even though the other wait exists, a wait must be here too,
                // since another update might have happened between them
                m_stager.Wait();
            }
            else
            {
                // If the update buffer is non-null and the stager is deleted, that means
                // the main thread deleted the stager and is about to delete the updated
                // buffer. If both the stager deletion and this update are on the main thread,
                // we cannot wait for the deletion of the previously updated buffer, since
                // that would result in a deadlock. The only option is to finish its job
                // once no frames are using it, then create a new buffer as usual.

                // This should be sufficient for now. It could be converted to wait for
                // the in-flight fences for all the renderers using this buffer later
                // if it creates too many stutters, but this exact scenario should be
                // rare enough that it doesn't matter.
                m_driver.QueueWaitIdle(m_queue);
            }

            bufferID = BufferID(buffer->buffer);

            // Reuse the old buffer if the size is the same, otherwise delete it
            // then create a new one like normal.
            if (buffer->size != size)
            {
                DestroyBuffer(*buffer);
                bufferID = CreateBuffer(*buffer, size);
            }
        }
        else // No update is in progress, so create a new buffer as normal
        {
            buffer = m_currentBuffer.load() == &m_buffer1 ? &m_buffer2 : &m_buffer1;
            LE_ASSERT(!buffer->buffer, "Buffer already exists");

            // Relaxed because of the update mutex lock
            m_updatedBuffer.store(buffer, std::memory_order_relaxed);
            bufferID = CreateBuffer(*buffer, size);
        }

        m_stager.CreateStagingBuffer(bufferID, size);
        m_stager.Upload(data, size);
        m_framesSinceDeletion = 0;
    }

    void SmartBuffer::Resize(std::size_t newSize)
    {
        LE_WARN("Resize called on a smart buffer. This doesn't affect the size");
    }

    Buffer::Desc SmartBuffer::GetDesc() const
    {
        const BufferDesc* pDesc = m_currentBuffer.load();

        if (!pDesc)
            return { {}, 0 };

        const Desc desc {
            BufferID(pDesc->buffer),
            pDesc->size,
        };

        return desc;
    }

    BufferID SmartBuffer::CreateBuffer(BufferDesc& target, const size_t size) const
    {
        const BufferID buffer = m_driver.CreateBuffer(
            m_usage | BufferUsageFlagBits::TRANSFER_DST, size, false);

        target.buffer = buffer.id;
        target.size = size;

        return buffer;
    }

    void SmartBuffer::DestroyBuffer(BufferDesc& buffer) const
    {
        if (!buffer.buffer)
            return;

        m_driver.DestroyBuffer(BufferID(buffer.buffer));
        buffer.buffer = 0;
    }

    void SmartBuffer::DeleteUnusedBuffers()
    {
        // If no update has happened
        if (!m_updatedBuffer.load())
            return;

        std::scoped_lock lock(m_updateMutex);

        // If the staging buffer exists, an update has occurred, and it could
        // be still uploading or not. There should never be a case where the
        // staging buffer exists but no update happened.
        if (m_stager.HasStagingBuffer())
        {
            if (!m_stager.IsFenceSignaled())
                return;

            m_stager.DeleteStagingBuffer();

            LE_ASSERT(m_updatedBuffer.load(), "Updated buffer is nullptr");
            m_updatedBuffer = m_currentBuffer.exchange(m_updatedBuffer.load());
        }

        // Check to make sure no frames are using the current buffer
        if (m_framesSinceDeletion <= Application::FRAMES_IN_FLIGHT)
        {
            m_framesSinceDeletion++;
            return;
        }

        // At this point, the new buffer has been created by Update (possibly
        // on another thread) and the old one isn't in use by frames in flight,
        // so it can be deleted.

        if (m_updatedBuffer.load())
            DestroyBuffer(*m_updatedBuffer.load());

        // Signal that nothing has been updated again
        m_updatedBuffer = nullptr;
        m_framesSinceDeletion = 0;
    }
}

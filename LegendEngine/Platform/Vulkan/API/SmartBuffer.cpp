#include "API/SmartBuffer.hpp"

#include <VkDefs.hpp>
#include <LE/Application.hpp>
#include <LE/Events/RenderEvent.hpp>

namespace le::vk
{
    SmartBuffer::SmartBuffer(GraphicsContext& context, const UsageFlags usage)
        :
        m_context(context.GetTetherGraphicsContext()),
        m_stager(context),
        m_sub(Application::Get().GetEventBus())
    {
        m_usage = ToVulkanUsageFlags(usage);
        m_sub.AddEventHandler<RenderEvent>([this](const RenderEvent&) { DeleteUnusedBuffers(); });
    }

    SmartBuffer::~SmartBuffer()
    {
        // Synchronization shouldn't be necessary here
        // An object being destroyed while it is still in use is a bug

        DestroyBuffer(m_buffer1);
        DestroyBuffer(m_buffer2);
    }

    void SmartBuffer::Update(const size_t size, const size_t offset, const void* data)
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

        VkBuffer vkBuffer = nullptr;
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
                vkQueueWaitIdle(m_context.GetQueue());
            }

            vkBuffer = buffer->buffer;

            // Reuse the old buffer if the size is the same, otherwise delete it
            // then create a new one like normal.
            if (buffer->size != size)
            {
                DestroyBuffer(*buffer);
                vkBuffer = CreateBuffer(buffer, size);
            }
        }
        else // No update is in progress, so create a new buffer as normal
        {
            buffer = m_currentBuffer.load() == &m_buffer1 ? &m_buffer2 : &m_buffer1;
            LE_ASSERT(buffer->buffer == nullptr, "Buffer already exists");

            // Relaxed because of the update mutex lock
            m_updatedBuffer.store(buffer, std::memory_order_relaxed);
            vkBuffer = CreateBuffer(buffer, size);
        }

        m_stager.CreateStagingBuffer(vkBuffer, size);
        m_stager.Upload(data, size);
        m_framesSinceDeletion = 0;
    }

    void SmartBuffer::Resize(const size_t newSize)
    {
        LE_WARN("Resize called on a smart buffer. This doesn't affect the size");
    }

    size_t SmartBuffer::GetSize()
    {
        return m_currentBuffer.load()->size;
    }

    Buffer::Desc SmartBuffer::GetDesc()
    {
        const BufferDesc* pDesc = m_currentBuffer.load();

        if (!pDesc)
        {
            LE_ASSERT(!m_updated, "m_currentBuffer was nullptr after update");
            return { nullptr, 0 };
        }

        const Desc desc {
            pDesc->buffer,
            pDesc->size,
        };

        return desc;
    }

    VkBuffer SmartBuffer::CreateBuffer(BufferDesc* target, const size_t size) const
    {
        VkBuffer buffer = nullptr;
        VmaAllocation allocation = nullptr;

        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = m_usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo allocInfo{};
        allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
        allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
            VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT;

        LE_CHECK_VK(vmaCreateBuffer(m_context.GetAllocator(), &bufferInfo, &allocInfo, &buffer, &allocation, nullptr));

        target->buffer = buffer;
        target->allocation = allocation;
        target->size = size;

        return buffer;
    }

    void SmartBuffer::DestroyBuffer(BufferDesc& buffer) const
    {
        if (!buffer.buffer)
            return;

        vmaDestroyBuffer(m_context.GetAllocator(), buffer.buffer, buffer.allocation);

        // Signal to this function that the buffer is deleted
        buffer.buffer = nullptr;
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
            m_updated = true;
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

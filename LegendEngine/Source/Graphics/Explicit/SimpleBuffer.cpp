#include <LE/Graphics/Explicit/SimpleBuffer.hpp>

namespace le
{
    SimpleBuffer::SimpleBuffer(const ExplicitRenderer& renderer, const BufferUsageFlags usage, const size_t size)
        :
        m_driver(renderer.GetDriver()),
        m_usage(usage)
    {
        m_buffer = m_driver.CreateBuffer(usage, size, false);
    }

    SimpleBuffer::~SimpleBuffer()
    {
        m_driver.DestroyBuffer(m_buffer);
    }

    void SimpleBuffer::Update(const std::size_t size, const std::size_t offset, const void* data)
    {
        void* mappedData = m_driver.GetMappedBufferData(m_buffer);
        LE_ASSERT(mappedData != nullptr, "Buffer was updated but not mapped");
        LE_ASSERT(size + offset <= m_driver.GetBufferSize(m_buffer), "Tried to copy too much data into buffer");
        // ReSharper disable once CppDFANullDereference
        memcpy(static_cast<uint8_t*>(mappedData) + offset, data, size);
    }

    void SimpleBuffer::Resize(std::size_t newSize)
    {
        LE_WARN(
            "Resize called on a SimpleBuffer. SimpleBuffer functions as "
            "a single buffer that doesn't take into account per-frame "
            "updates or frequent resizing. This action recreates the buffer, "
            "which must wait for frames to finish rendering. This could cause "
            "stalls or latency spikes, so consider using a different buffer type "
            "if resizing or updates happen often."
        );

        m_driver.WaitIdle();

        m_driver.DestroyBuffer(m_buffer);
        m_buffer = m_driver.CreateBuffer(m_usage, newSize, false);
    }

    std::size_t SimpleBuffer::GetSize()
    {
        return m_driver.GetBufferSize(m_buffer);
    }

    BufferID SimpleBuffer::GetBuffer()
    {
        return m_buffer;
    }
}

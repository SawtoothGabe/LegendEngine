#include <LE/Application.hpp>
#include <LE/Graphics/Explicit/PerFrameBuffer.hpp>

namespace le
{
    PerFrameBuffer::PerFrameBuffer(const ExplicitRenderer& renderer, const BufferUsageFlags usage, const size_t size)
        :
        m_driver(renderer.GetDriver()),
        m_size(size),
        m_usage(usage)
    {
        m_buffers.resize(Application::FRAMES_IN_FLIGHT);
        m_sizes.resize(Application::FRAMES_IN_FLIGHT);

        for (size_t i = 0; i < Application::FRAMES_IN_FLIGHT; ++i)
        {
            m_sizes[i] = size;
            CreateBuffer(m_buffers[i]);
        }
    }

    PerFrameBuffer::~PerFrameBuffer()
    {
        for (size_t i = 0; i < m_buffers.size(); ++i)
            m_driver.DestroyBuffer(m_buffers[i]);
    }

    void PerFrameBuffer::Update(const std::size_t size, const std::size_t offset, const void* data)
    {
        const size_t currentFrame = Application::Get().GetCurrentFrame();
        const size_t allocSize = m_sizes[currentFrame];

        if (allocSize != m_size)
            RecreateBuffer(currentFrame);

        void* pMappedData = m_driver.GetMappedBufferData(m_buffers[currentFrame]);

        LE_ASSERT(pMappedData != nullptr, "Buffer was updated but not mapped");
        LE_ASSERT(size + offset <= allocSize, "Tried to copy too much data into buffer");

        // ReSharper disable once CppDFANullDereference
        memcpy(static_cast<uint8_t*>(pMappedData) + offset, data, size);
    }

    void PerFrameBuffer::Resize(const std::size_t newSize)
    {
        m_size = newSize;
    }

    Buffer::Desc PerFrameBuffer::GetDesc() const
    {
        return {
            m_buffers[Application::Get().GetCurrentFrame()],
            m_sizes[Application::Get().GetCurrentFrame()],
        };
    }

    void PerFrameBuffer::CreateBuffer(BufferID& buffer) const
    {
        buffer = m_driver.CreateBuffer(m_usage, m_size, m_mapped);
    }

    void PerFrameBuffer::RecreateBuffer(const size_t currentFrame)
    {
        m_driver.DestroyBuffer(m_buffers[currentFrame]);
        m_sizes[currentFrame] = m_size;
        CreateBuffer(m_buffers[currentFrame]);
    }
}

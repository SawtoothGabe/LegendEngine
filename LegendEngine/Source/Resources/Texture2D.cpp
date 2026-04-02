#include <LE/Application.hpp>
#include <LE/IO/TextureData.hpp>
#include <LE/Resources/Texture2D.hpp>

namespace le
{
    Texture2D::Texture2D(const TextureData& loader)
        :
        m_Width(loader.GetWidth()),
        m_Height(loader.GetHeight()),
        m_Channels(loader.GetChannels()),
        m_context(Application::Get().GetGraphicsContext())
    {
        Image::Format format;
        switch (m_Channels)
        {
            case 1: format = Image::Format::R8; break;
            case 2: format = Image::Format::R8G8; break;
            case 3: format = Image::Format::R8G8B8; break;
            case 4: format = Image::Format::R8G8B8A8; break;
            default: format = Image::Format::R8; break;
        }

        const Image::Info imageInfo =
        {
            .width = m_Width,
            .height = m_Height,
            .format = format,
            .type = Image::Type::TYPE_2D,
        };

        m_image = m_context.CreateImage(imageInfo);

        Upload(loader.GetData(), m_Width * m_Height * m_Channels, format);
    }

    uint64_t Texture2D::GetWidth() const
    {
        return m_Width;
    }

    uint64_t Texture2D::GetHeight() const
    {
        return m_Height;
    }

    uint8_t Texture2D::GetChannels() const
    {
        return m_Channels;
    }

    Image& Texture2D::GetImage()
    {
        return *m_image;
    }

    void Texture2D::Upload(const void* data, const size_t size, const Image::Format format) const
    {
        const Scope<Buffer> stagingBuffer = m_context.CreateSimpleBuffer(
            Buffer::UsageFlags::TRANSFER_SRC, size, true);

        stagingBuffer->Update(size, 0, data);

        const Scope<CommandBuffer> cmdBuffer = m_context.CreateCommandBuffer(false);

        cmdBuffer->Begin(true);
        {
            cmdBuffer->TransitionImageLayout(*m_image,
                Image::Layout::UNDEFINED,
                Image::Layout::TRANSFER_DST_OPTIMAL,
                Image::Aspect::COLOR);

            CommandBuffer::BufferImageCopy region{};
            region.imageSubresource.aspect = Image::Aspect::COLOR;
            region.imageSubresource.layerCount = 1;
            region.imageExtent =
            {
                static_cast<uint32_t>(m_Width),
                static_cast<uint32_t>(m_Height),
                1
            };

            cmdBuffer->CmdCopyBufferToImage(*stagingBuffer, *m_image,
                Image::Layout::TRANSFER_DST_OPTIMAL, 1, &region);

            cmdBuffer->TransitionImageLayout(*m_image,
                Image::Layout::TRANSFER_DST_OPTIMAL,
                Image::Layout::SHADER_READ_ONLY_OPTIMAL,
                Image::Aspect::COLOR);
        }
        cmdBuffer->End();
        cmdBuffer->Submit(true);
    }
}

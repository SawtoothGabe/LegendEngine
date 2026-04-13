#include <LE/Graphics/Explicit/ExplicitTexture2D.hpp>

namespace le
{
    ExplicitTexture2D::ExplicitTexture2D(const ExplicitRenderer& renderer, const TextureData& loader)
        :
        m_driver(renderer.GetDriver())
    {
        Format format;
        switch (loader.GetChannels())
        {
            case 1: format = Format::R8_UNORM; break;
            case 2: format = Format::R8G8_UNORM; break;
            case 3: format = Format::R8G8B8_UNORM; break;
            case 4: format = Format::R8G8B8A8_UNORM; break;
            default: format = Format::R8G8B8A8_UNORM; break;
        }

        ImageInfo imageInfo;
        imageInfo.width = loader.GetWidth();
        imageInfo.height = loader.GetHeight();
        imageInfo.format = format;

        m_image = m_driver.CreateImage(imageInfo);
        m_view = m_driver.CreateImageView(m_image, format, ImageViewType::TYPE_2D);

        const Extent3D extent = {
            .width  = static_cast<uint32_t>(loader.GetWidth()),
            .height = static_cast<uint32_t>(loader.GetHeight()),
            .depth = 1,
        };

        Upload(loader.GetData(), imageInfo.width * imageInfo.height * loader.GetChannels(), extent,
            renderer.GetTransferQueue(), renderer.GetTransferPool());
    }

    ExplicitTexture2D::~ExplicitTexture2D()
    {
        m_driver.DestroyImage(m_image);
        m_driver.DestroyImageView(m_view);
    }

    void ExplicitTexture2D::Upload(const void* data, const size_t size, Extent3D extent,
        const QueueID& queue, const CommandPoolID& commandPool) const
    {
        const BufferID stagingBuffer = m_driver.CreateBuffer(
            BufferUsageFlagBits::TRANSFER_SRC, size, true);

        memcpy(m_driver.GetMappedBufferData(stagingBuffer), data, size);

        CommandBufferID cmdBuffer = m_driver.AllocateCommandBuffer(commandPool);
        m_driver.BeginCommandBuffer(cmdBuffer, true);
        {
            m_driver.TransitionImageLayout(cmdBuffer, m_image,
                ImageLayout::UNDEFINED,
                ImageLayout::TRANSFER_DST_OPTIMAL,
                ImageAspect::COLOR);

            BufferImageCopy region{};
            region.imageSubresource.aspect = ImageAspect::COLOR;
            region.imageSubresource.layerCount = 1;
            region.imageExtent = extent;

            m_driver.CmdCopyBufferToImage(cmdBuffer, stagingBuffer, m_image,
                ImageLayout::TRANSFER_DST_OPTIMAL, std::span(&region, 1));

            m_driver.TransitionImageLayout(cmdBuffer, m_image,
                ImageLayout::TRANSFER_DST_OPTIMAL,
                ImageLayout::SHADER_READ_ONLY_OPTIMAL,
                ImageAspect::COLOR
            );
        }
        m_driver.EndCommandBuffer(cmdBuffer);

        const FenceID fence = m_driver.CreateFence(true);

        SubmitInfo info;
        info.commandBuffer = cmdBuffer;
        info.fence = fence;

        m_driver.QueueSubmit(queue, info);
        m_driver.DestroyFence(fence);
        m_driver.FreeCommandBuffers(commandPool, 1, &cmdBuffer);
        m_driver.DestroyBuffer(stagingBuffer);
    }
}

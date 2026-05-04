#include <LE/Graphics/Explicit/ExplicitTexture2DArray.hpp>

namespace le
{
    ExplicitTexture2DArray::ExplicitTexture2DArray(ExplicitResources& resources,
        const size_t width, const size_t height, const uint8_t channels,
        const std::span<TextureData*>& textureData)
        :
        m_driver(resources.GetDriver()),
        m_mutex(resources.GetGraphicsMutex())
    {
        Format format;
        switch (channels)
        {
            case 1: format = Format::R8_SRGB; break;
            case 2: format = Format::R8G8_SRGB; break;
            case 3: format = Format::R8G8B8_SRGB; break;
            default: format = Format::R8G8B8A8_SRGB; break;
        }

        ImageInfo imageInfo;
        imageInfo.width = width;
        imageInfo.height = height;
        imageInfo.format = format;

        m_image = m_driver.CreateImage(imageInfo);

        ImageViewInfo viewInfo;
        viewInfo.image = m_image;
        viewInfo.format = imageInfo.format;

        m_view = m_driver.CreateImageView(viewInfo);

        const Extent3D extent {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height),
            channels
        };

        Upload(textureData, extent, resources.GetGraphicsQueue(), resources.GetGraphicsPool());
    }

    ExplicitTexture2DArray::~ExplicitTexture2DArray()
    {
        m_driver.DestroyImage(m_image);
        m_driver.DestroyImageView(m_view);
    }

    ImageID ExplicitTexture2DArray::GetImage() const
    {
        return m_image;
    }

    ImageViewID ExplicitTexture2DArray::GetImageView() const
    {
        return m_view;
    }

    void ExplicitTexture2DArray::Upload(const std::span<TextureData*>& textureData,
                                        const Extent3D extent, const QueueID& queue, const CommandPoolID& commandPool) const
    {
        const size_t size = static_cast<size_t>(extent.width)
            * static_cast<size_t>(extent.height)
            * static_cast<size_t>(extent.depth);

        const BufferID buffer = m_driver.CreateBuffer(
            BufferUsageFlagBits::TRANSFER_SRC, size, true);

        CopyImagesToBuffer(static_cast<uint8_t*>(m_driver.GetMappedBufferData(buffer)),
            size, textureData);

        CommandBufferID c = m_driver.AllocateCommandBuffer(commandPool);
        m_driver.BeginCommandBuffer(c, true);
        {
            ImageMemoryBarrier barrier{};
            barrier.oldLayout = ImageLayout::UNDEFINED;
            barrier.newLayout = ImageLayout::TRANSFER_DST_OPTIMAL;
            barrier.dstAccessMask = AccessFlagBits::TRANSFER_WRITE_BIT;
            barrier.image = m_image;
            barrier.subresourceRange.aspect = ImageAspect::COLOR;
            barrier.subresourceRange.layerCount = static_cast<uint32_t>(textureData.size());

            m_driver.CmdPipelineBarrier(c, PipelineStage::TOP_OF_PIPE,
                PipelineStage::TRANSFER, std::span(&barrier, 1));

            std::vector<BufferImageCopy> copies(textureData.size());
            for (size_t i = 0; i < copies.size(); i++)
            {
                copies[i].bufferOffset = i * size;
                copies[i].imageSubresource.aspect = ImageAspect::COLOR;
                copies[i].imageSubresource.baseArrayLayer = static_cast<uint32_t>(i);
                copies[i].imageSubresource.layerCount = 1;
                copies[i].imageOffset = { 0, 0, 0 };
                copies[i].imageExtent = extent;
            }

            m_driver.CmdCopyBufferToImage(c, buffer, m_image,
                ImageLayout::TRANSFER_DST_OPTIMAL, copies);

            barrier.oldLayout = ImageLayout::TRANSFER_DST_OPTIMAL;
            barrier.newLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL;
            barrier.srcAccessMask = AccessFlagBits::TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = AccessFlagBits::SHADER_READ_BIT;

            m_driver.CmdPipelineBarrier(c, PipelineStage::TRANSFER,
                PipelineStage::FRAGMENT_SHADER, std::span(&barrier, 1));
        }
        m_driver.EndCommandBuffer(c);

        FenceID fence = m_driver.CreateFence(false);

        SubmitInfo info;
        info.commandBuffer = c;
        info.fence = fence;

        {
            std::scoped_lock lock(m_mutex);
            m_driver.QueueSubmit(queue, info);
        }

        m_driver.WaitForFences(1, &fence);
        m_driver.FreeCommandBuffers(commandPool, 1, &c);
        m_driver.DestroyFence(fence);
        m_driver.DestroyBuffer(buffer);
    }

    void ExplicitTexture2DArray::CopyImagesToBuffer(uint8_t* data, const size_t size,
        const std::span<TextureData*>& textureData)
    {
        for (size_t i = 0; i < textureData.size(); i++)
        {
            const TextureData* texture = textureData[i];
            const size_t textureSize = texture->GetWidth() * texture->GetHeight()
                * texture->GetChannels();

            LE_ASSERT(textureSize == size, "All textures given to Texture2DArray must be the same size");

            memcpy(data + i * size, texture->GetData(), size);
        }
    }
}

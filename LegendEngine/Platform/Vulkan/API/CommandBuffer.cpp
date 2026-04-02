#include "API/CommandBuffer.hpp"

#include <API/Buffer.hpp>
#include <API/Image.hpp>

#include "VkDefs.hpp"

namespace le::vk
{
    CommandBuffer::CommandBuffer(GraphicsContext& context, const VkCommandPool pool, const VkQueue queue, std::mutex& mutex)
        :
        m_context(context.GetTetherGraphicsContext()),
        m_mutex(mutex),
        m_pool(pool),
        m_queue(queue)
    {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = m_pool;
        allocInfo.commandBufferCount = 1;

        vkAllocateCommandBuffers(m_context.GetDevice(), &allocInfo, &m_commandBuffer);

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

        LE_CHECK_VK(vkCreateFence(m_context.GetDevice(), &fenceInfo, nullptr, &m_fence));
    }

    void CommandBuffer::CmdPipelineBarrier(const PipelineStage srcStage, const PipelineStage dstStage,
        const size_t imageMemoryBarrierCount, ImageMemoryBarrier* imageMemoryBarriers)
    {
        const VkPipelineStageFlags srcStageMask = GetPipelineStageFlags(srcStage);
        const VkPipelineStageFlags dstStageMask = GetPipelineStageFlags(dstStage);
        std::vector<VkImageMemoryBarrier> vkImageMemoryBarriers(imageMemoryBarrierCount);

        for (size_t i = 0; i < imageMemoryBarrierCount; i++)
        {
            const ImageMemoryBarrier& barrier = imageMemoryBarriers[i];
            VkImageMemoryBarrier& vkBarrier = vkImageMemoryBarriers[i];

            vkBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            vkBarrier.oldLayout = Image::GetImageLayout(barrier.oldLayout);
            vkBarrier.newLayout = Image::GetImageLayout(barrier.newLayout);
            vkBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            vkBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            vkBarrier.image = static_cast<Image*>(barrier.image)->GetImage();
            vkBarrier.subresourceRange.aspectMask = Image::GetImageAspectFlags(barrier.subresourceRange.aspect);
            vkBarrier.subresourceRange.levelCount = barrier.subresourceRange.mipLevel;
            vkBarrier.subresourceRange.baseArrayLayer = barrier.subresourceRange.baseArrayLayer;
            vkBarrier.subresourceRange.layerCount = barrier.subresourceRange.layerCount;
        }

        vkCmdPipelineBarrier(
            m_commandBuffer, srcStageMask, dstStageMask,
            0,
            0, nullptr,
            0, nullptr,
            imageMemoryBarrierCount, vkImageMemoryBarriers.data()
        );
    }

    void CommandBuffer::TransitionImageLayout(le::Image& leImage, const le::Image::Layout oldImageLayout,
        const le::Image::Layout newImageLayout, const Image::Aspect aspect)
    {
        const VkImage image = static_cast<Image&>(leImage).GetImage();
        const VkImageLayout oldLayout = Image::GetImageLayout(oldImageLayout);
        const VkImageLayout newLayout = Image::GetImageLayout(newImageLayout);
        const VkImageAspectFlags aspectMask = Image::GetImageAspectFlags(aspect);

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.aspectMask = aspectMask;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        VkPipelineStageFlags sourceStage = 0;
        VkPipelineStageFlags destinationStage = 0;

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED
            && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
        {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT
                | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        }
        else
        {
            LE_ASSERT(false, "Unsupported layout transition");
        }

        vkCmdPipelineBarrier(
            m_commandBuffer,
            sourceStage, destinationStage,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier
        );
    }

    void CommandBuffer::Begin(const bool oneTimeSubmit)
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = oneTimeSubmit ? VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT : 0;

        LE_CHECK_VK(vkBeginCommandBuffer(m_commandBuffer, &beginInfo));
    }

    void CommandBuffer::End()
    {
        LE_CHECK_VK(vkEndCommandBuffer(m_commandBuffer));
    }

    void CommandBuffer::Submit(const bool wait)
    {
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &m_commandBuffer;

        vkQueueSubmit(m_queue, 1, &submitInfo, m_fence);

        if (wait)
            Wait();
    }

    void CommandBuffer::CmdCopyBufferToImage(le::Buffer& buffer, le::Image& image, le::Image::Layout layout, size_t count,
        BufferImageCopy* regions)
    {
        std::vector<VkBufferImageCopy> vkRegions(count);
        for (size_t i = 0; i < count; i++)
        {
            const BufferImageCopy& region = regions[i];
            VkBufferImageCopy& regionCopy = vkRegions[i];

            regionCopy.bufferOffset = region.bufferOffset;
            regionCopy.bufferOffset = region.bufferOffset;
            regionCopy.bufferRowLength = region.bufferRowLength;
            regionCopy.bufferImageHeight = region.bufferImageHeight;
            regionCopy.imageSubresource.aspectMask     = Image::GetImageAspectFlags(region.imageSubresource.aspect);
            regionCopy.imageSubresource.mipLevel       = region.imageSubresource.mipLevel;
            regionCopy.imageSubresource.baseArrayLayer = region.imageSubresource.baseArrayLayer;
            regionCopy.imageSubresource.layerCount     = region.imageSubresource.layerCount;
            regionCopy.imageOffset.x = region.imageOffset.x;
            regionCopy.imageOffset.y = region.imageOffset.y;
            regionCopy.imageOffset.z = region.imageOffset.z;
            regionCopy.imageExtent.width = region.imageExtent.width;
            regionCopy.imageExtent.height = region.imageExtent.height;
            regionCopy.imageExtent.depth = region.imageExtent.depth;
        }

        vkCmdCopyBufferToImage(
            m_commandBuffer,
            static_cast<Buffer&>(buffer).GetDesc().buffer,
            static_cast<Image&>(image).GetImage(),
            Image::GetImageLayout(layout),
            1, vkRegions.data()
        );
    }

    VkPipelineStageFlags CommandBuffer::GetPipelineStageFlags(const PipelineStage stage)
    {
        VkPipelineStageFlags stageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        switch (stage)
        {
            case PipelineStage::TOP_OF_PIPE: stageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT; break;
            case PipelineStage::TRANSFER: stageMask = VK_PIPELINE_STAGE_TRANSFER_BIT; break;
            case PipelineStage::FRAGMENT_SHADER: stageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT; break;
        }

        return stageMask;
    }

    void CommandBuffer::Wait() const
    {
        vkWaitForFences(m_context.GetDevice(), 1, &m_fence, VK_TRUE, UINT64_MAX);
    }

    void CommandBuffer::CmdCopyBuffer(le::Buffer& src, le::Buffer& dst, size_t regionCount, BufferCopy* pRegions)
    {
        std::vector<VkBufferCopy> vkRegions(regionCount);
        for (size_t i = 0; i < regionCount; i++)
        {
            const BufferCopy& region = pRegions[i];

            VkBufferCopy regionCopy{};
            regionCopy.size = region.size;
            regionCopy.srcOffset = region.srcOffset;
            regionCopy.dstOffset = region.dstOffset;

            vkRegions[i] = regionCopy;
        }

        const auto& vkSrc = static_cast<Buffer&>(src).GetDesc();
        const auto& vkDst = static_cast<Buffer&>(dst).GetDesc();
        vkCmdCopyBuffer(m_commandBuffer, vkSrc.buffer, vkDst.buffer,
            vkRegions.size(), vkRegions.data());
    }
}

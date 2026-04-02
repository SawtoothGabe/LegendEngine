#pragma once

#include <mutex>
#include <LE/Graphics/API/CommandBuffer.hpp>
#include <vulkan/vulkan.h>

#include "GraphicsContext.hpp"

namespace le::vk
{
    class CommandBuffer final : public le::CommandBuffer
    {
    public:
        CommandBuffer(GraphicsContext& context, VkCommandPool pool, VkQueue queue, std::mutex& mutex);

        void TransitionImageLayout(Image& leImage, Image::Layout oldImageLayout,
            Image::Layout newImageLayout, Image::Aspect aspect) override;

        void Begin(bool oneTimeSubmit) override;

        void End() override;

        void Submit(bool wait) override;
        void Wait() const;

        void CmdCopyBuffer(le::Buffer& src, le::Buffer& dst, size_t regionCount, BufferCopy* pRegions) override;
        void CmdCopyBufferToImage(le::Buffer& buffer, le::Image& image, Image::Layout layout, size_t count,
            BufferImageCopy* regions) override;
        void CmdPipelineBarrier(PipelineStage srcStage, PipelineStage dstStage, size_t imageMemoryBarrierCount,
            ImageMemoryBarrier* imageMemoryBarriers) override;
    private:
        static VkPipelineStageFlags GetPipelineStageFlags(PipelineStage stage);

        TetherVulkan::GraphicsContext& m_context;
        std::mutex& m_mutex;

        VkCommandPool m_pool = nullptr;
        VkQueue m_queue = nullptr;
        VkCommandBuffer m_commandBuffer = nullptr;
        VkFence m_fence = nullptr;
    };
}

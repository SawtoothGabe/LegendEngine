#pragma once

#include <LE/Common/Types.hpp>
#include <LE/Graphics/Types.hpp>

#include <LE/TetherBindings.hpp>

namespace le
{
    class Renderer;
    class ExplicitDriver
    {
    public:
        virtual ~ExplicitDriver() = default;

        [[nodiscard]] virtual std::vector<CommandBufferID> AllocateCommandBuffers(CommandPoolID pool, size_t count) = 0;
        [[nodiscard]] virtual CommandBufferID AllocateCommandBuffer(CommandPoolID pool) = 0;
        [[nodiscard]] virtual std::vector<DescriptorSetID> AllocateDescriptorSets(PoolManagerID manager, DescriptorPoolID& outPool, size_t count) = 0;
        [[nodiscard]] virtual BufferID CreateBuffer(BufferUsageFlags flags, std::size_t size, bool createMapped) = 0;
        [[nodiscard]] virtual CommandPoolID CreateCommandPool(QueueFamily family) = 0;
        [[nodiscard]] virtual PoolManagerID CreateLayoutPoolManager(DescriptorSetLayoutID layout) = 0;
        [[nodiscard]] virtual FenceID CreateFence(bool signaled) = 0;
        [[nodiscard]] virtual ImageID CreateImage(const ImageInfo& info) = 0;
        [[nodiscard]] virtual ImageViewID CreateImageView(ImageID image, Format format, ImageViewType type) = 0;
        [[nodiscard]] virtual PipelineID CreatePipeline(const PipelineInfo& info) = 0;
        [[nodiscard]] virtual PipelineLayoutID CreatePipelineLayout(std::span<PushConstantRange> ranges,
            std::span<DescriptorSetLayoutID> layouts) = 0;
        [[nodiscard]] virtual SemaphoreID CreateSemaphore() = 0;
        [[nodiscard]] virtual SwapchainID CreateSwapchain(const SwapchainInfo& info) = 0;
        [[nodiscard]] virtual SurfaceID CreateSurface(Window& window) = 0;
        [[nodiscard]] virtual ShaderModuleID CreateShaderModule(const ShaderModuleInfo& info) = 0;
        [[nodiscard]] virtual DescriptorSetLayoutID CreateDescriptorSetLayout(std::span<DescriptorSetLayoutBinding> bindings) = 0;
        [[nodiscard]] virtual SamplerID CreateSampler(const SamplerInfo& info) = 0;
        [[nodiscard]] virtual QueueID GetQueue(QueueFamily family) = 0;

        virtual void FreeCommandBuffers(CommandPoolID pool, size_t count, CommandBufferID* buffers) = 0;
        virtual void FreeDescriptorSets(PoolManagerID manager, DescriptorPoolID pool, size_t count, DescriptorSetID* sets) = 0;
        virtual void DestroyBuffer(BufferID buffer) = 0;
        virtual void DestroyCommandPool(CommandPoolID pool) = 0;
        virtual void DestroyLayoutPoolManager(PoolManagerID manager) = 0;
        virtual void DestroyFence(FenceID fence) = 0;
        virtual void DestroyImage(ImageID image) = 0;
        virtual void DestroyImageView(ImageViewID view) = 0;
        virtual void DestroyPipeline(PipelineID pipeline) = 0;
        virtual void DestroyPipelineLayout(PipelineLayoutID layoutID);
        virtual void DestroySemaphore(SemaphoreID semaphore) = 0;
        virtual void DestroySwapchain(SwapchainID swapchain) = 0;
        virtual void DestroySurface(SurfaceID surface) = 0;
        virtual void DestroyShaderModule(ShaderModuleID shaderModule) = 0;
        virtual void DestroyDescriptorSetLayout(DescriptorSetLayoutID layout) = 0;
        virtual void DestroySampler(SamplerID sampler) = 0;

        virtual void ResetAllPools(PoolManagerID manager) = 0;
        virtual void WaitForFences(size_t count, FenceID* fences) = 0;
        virtual void WaitIdle() = 0;
        virtual void ResetFences(size_t count, FenceID* fences) = 0;
        virtual void QueueSubmit(QueueID queue, const SubmitInfo& info) = 0;
        virtual void QueuePresent(QueueID queue, const PresentInfo& info) = 0;
        virtual void QueueWaitIdle(QueueID queue) = 0;
        virtual void ResetCommandBuffer(CommandBufferID buffer) = 0;
        virtual void BeginCommandBuffer(CommandBufferID buffer, bool singleUse) = 0;
        virtual void EndCommandBuffer(CommandBufferID buffer) = 0;
        virtual void* GetMappedBufferData(BufferID buffer) = 0;
        virtual size_t GetBufferSize(BufferID buffer) = 0;
        virtual bool IsFenceSignaled(FenceID fence) = 0;
        virtual SurfaceCapabilities GetSurfaceCapabilities(SurfaceID surface) = 0;
        virtual FormatProperties GetFormatProperties() = 0;
        virtual Format FindDepthFormat() = 0;

        virtual void CmdCopyBuffer(CommandBufferID buffer, BufferID src, BufferID dst, std::span<BufferCopy> regions) = 0;
        virtual void CmdCopyBufferToImage(CommandBufferID buffer, BufferID src, ImageID dst,
            ImageLayout layout, std::span<BufferImageCopy> regions) = 0;
        virtual void CmdPipelineBarrier(CommandBufferID buffer,
            PipelineStage srcStage, PipelineStage dstStage,
            std::span<ImageMemoryBarrier> imageMemoryBarriers) = 0;
        virtual void CmdBeginRendering(CommandBufferID buffer, const RenderingInfo& info) = 0;
        virtual void CmdSetViewport(CommandBufferID buffer, Extent2D size) = 0;
        virtual void CmdSetScissor(CommandBufferID buffer, Rect2D rect) = 0;
        virtual void CmdBindPipeline(CommandBufferID buffer, PipelineBindPoint bindPoint,
            PipelineID pipeline) = 0;
        virtual void CmdSetCullMode(CommandBufferID buffer, CullMode cullMode) = 0;
        virtual void CmdPushConstants(CommandBufferID buffer, PipelineLayoutID layout,
            ShaderStageFlagBits stage, size_t offset, size_t size, void* values) = 0;
        virtual void CmdBindDescriptorSets(CommandBufferID buffer, PipelineBindPoint bindPoint,
            PipelineLayoutID layout, size_t firstSet, std::span<DescriptorSetID> sets) = 0;
        virtual void CmdBindVertexBuffers(CommandBufferID buffer, uint32_t firstBinding, std::span<BufferID> buffers) = 0;
        virtual void CmdBindIndexBuffer(CommandBufferID buffer, BufferID indexBuffer, uint64_t offset) = 0;
        virtual void CmdDrawIndexed(CommandBufferID buffer, uint32_t indexCount,
            uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset,
            uint32_t firstInstance) = 0;
        virtual void CmdEndRendering(CommandBufferID buffer) = 0;

        virtual void TransitionImageLayout(CommandBufferID buffer, ImageID image, ImageLayout oldLayout,
            ImageLayout newLayout, ImageAspect aspect) = 0;

        static Scope<ExplicitDriver> Create(GraphicsAPI api,
            std::string_view applicationName);
    };
}

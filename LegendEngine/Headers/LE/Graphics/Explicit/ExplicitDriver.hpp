#pragma once

#include <cstdint>

#include <LE/Common/Assert.hpp>
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

        virtual std::vector<CommandBufferID> AllocateCommandBuffers(CommandPoolID pool, size_t count) = 0;
        virtual std::vector<DescriptorSetID> AllocateDescriptorSets() = 0;
        virtual BufferID CreateBuffer(BufferUsageFlags flags, std::size_t size, bool createMapped) = 0;
        virtual CommandPoolID CreateCommandPool(QueueFamily family) = 0;
        virtual FenceID CreateFence(bool signaled = true) = 0;
        virtual ImageID CreateImage(const ImageInfo& info) = 0;
        virtual ImageViewID CreateImageView(ImageID image, Format format, ImageViewType type) = 0;
        virtual PipelineID CreatePipeline(const PipelineInfo& info) = 0;
        virtual PipelineLayoutID CreatePipelineLayout(std::span<PushConstantRange> ranges,
            std::span<DescriptorSetLayoutID> layouts) = 0;
        virtual SemaphoreID CreateSemaphore() = 0;
        virtual SwapchainID CreateSwapchain() = 0;
        virtual SurfaceID CreateSurface(Window& window) = 0;
        virtual ShaderModuleID CreateShaderModule() = 0;
        virtual DescriptorSetLayoutID CreateDescriptorSetLayout() = 0;
        virtual SamplerID CreateSampler() = 0;

        virtual void FreeCommandBuffers() = 0;
        virtual void FreeDescriptorSets() = 0;
        virtual void DestroyBuffer(BufferID buffer) = 0;
        virtual void DestroyCommandPool(CommandPoolID pool) = 0;
        virtual void DestroyFence(FenceID fence) = 0;
        virtual void DestroyImage(ImageID image) = 0;
        virtual void DestroyImageView(ImageViewID view) = 0;
        virtual void DestroyPipeline(PipelineID pipeline) = 0;
        virtual void DestroySemaphore(SemaphoreID semaphore) = 0;
        virtual void DestroySwapchain(SwapchainID swapchain) = 0;
        virtual void DestroySurface(SurfaceID surface) = 0;
        virtual void DestroyShaderModule(ShaderModuleID shaderModule) = 0;
        virtual void DestroyDescriptorSetLayout(DescriptorSetLayoutID layout) = 0;
        virtual void DestroySampler(SamplerID sampler) = 0;

        virtual void WaitForFences(size_t count, FenceID* fences) = 0;
        virtual void WaitIdle() = 0;
        virtual void ResetFences(size_t count, uint64_t* fences) = 0;
        virtual void QueueSubmit() = 0;
        virtual void QueuePresent() = 0;
        virtual void ResetCommandBuffer(CommandBufferID buffer) = 0;
        virtual void BeginCommandBuffer(CommandBufferID buffer) = 0;
        virtual void EndCommandBuffer(CommandBufferID buffer) = 0;

        virtual void CmdCopyBuffer(CommandBufferID buffer) = 0;
        virtual void CmdCopyBufferToImage(CommandBufferID buffer) = 0;
        virtual void CmdPipelineBarrier(CommandBufferID buffer) = 0;
        virtual void CmdBeginRendering(CommandBufferID buffer) = 0;
        virtual void CmdSetViewport(CommandBufferID buffer) = 0;
        virtual void CmdSetScissor(CommandBufferID buffer) = 0;
        virtual void CmdBindPipeline(CommandBufferID buffer) = 0;
        virtual void CmdSetCullMode(CommandBufferID buffer) = 0;
        virtual void CmdPushConstants(CommandBufferID buffer) = 0;
        virtual void CmdBindDescriptorSets(CommandBufferID buffer) = 0;
        virtual void CmdBindVertexBuffers(CommandBufferID buffer) = 0;
        virtual void CmdBindIndexBuffer(CommandBufferID buffer) = 0;
        virtual void CmdDrawIndexed(CommandBufferID buffer) = 0;
        virtual void CmdEndRendering(CommandBufferID buffer) = 0;

        static Scope<ExplicitDriver> Create(GraphicsAPI api,
            std::string_view applicationName);
    };
}

#pragma once

#include <PoolManager.hpp>

#include "VkDefs.hpp"

#include <vk_mem_alloc.h>
#include <LE/TetherBindings.hpp>
#include <LE/Graphics/Explicit/ExplicitDriver.hpp>

namespace le
{
    class VulkanDriver : public ExplicitDriver
    {
    public:
        explicit VulkanDriver(std::string_view applicationName);
        ~VulkanDriver() override;

        std::vector<CommandBufferID> AllocateCommandBuffers(CommandPoolID pool, size_t count) override;
        CommandBufferID AllocateCommandBuffer(CommandPoolID pool) override;
        std::vector<DescriptorSetID> AllocateDescriptorSets(PoolManagerID manager, DescriptorPoolID& outPool, size_t count) override;
        [[nodiscard]] BufferID CreateBuffer(BufferUsageFlags flags, std::size_t size, bool createMapped) override;
        CommandPoolID CreateCommandPool(QueueFamily family) override;
        PoolManagerID CreateLayoutPoolManager(DescriptorSetLayoutID layout) override;
        FenceID CreateFence(bool signaled) override;
        ImageID CreateImage(const ImageInfo& info) override;
        ImageViewID CreateImageView(ImageID image, Format format, ImageViewType type) override;
        PipelineID CreatePipeline(const PipelineInfo& info) override;
        PipelineLayoutID CreatePipelineLayout(std::span<PushConstantRange> ranges,
            std::span<DescriptorSetLayoutID> layouts) override;
        SemaphoreID CreateSemaphore() override;
        SwapchainID CreateSwapchain(const SwapchainInfo& info) override;
        SurfaceID CreateSurface(Window& window) override;
        DescriptorSetLayoutID CreateDescriptorSetLayout(std::span<DescriptorSetLayoutBinding> bindings) override;
        SamplerID CreateSampler(const SamplerInfo& info) override;
        QueueID GetQueue(QueueFamily family) override;

        void FreeCommandBuffers(CommandPoolID pool, size_t count, CommandBufferID* buffers) override;
        void FreeDescriptorSets(PoolManagerID manager, DescriptorPoolID pool, size_t count, DescriptorSetID* sets) override;
        void DestroyBuffer(BufferID buffer) override;
        void DestroyCommandPool(CommandPoolID pool) override;
        void DestroyLayoutPoolManager(PoolManagerID manager) override;
        void DestroyFence(FenceID fence) override;
        void DestroyImage(ImageID image) override;
        void DestroyImageView(ImageViewID view) override;
        void DestroyPipeline(PipelineID pipeline) override;
        void DestroyPipelineLayout(PipelineLayoutID pipelineLayout) override;
        void DestroySemaphore(SemaphoreID semaphore) override;
        void DestroySwapchain(SwapchainID swapchain) override;
        void DestroySurface(SurfaceID surface) override;
        void DestroyShaderModule(ShaderModuleID shaderModule) override;
        void DestroyDescriptorSetLayout(DescriptorSetLayoutID layout) override;
        void DestroySampler(SamplerID sampler) override;

        void ResetAllPools(PoolManagerID manager) override;
        void WaitForFences(size_t count, FenceID* fences) override;
        void WaitIdle() override;
        void ResetFences(size_t count, FenceID* fences) override;
        void QueueSubmit(QueueID queue, const SubmitInfo& info) override;
        void QueuePresent(QueueID queue, const PresentInfo& info) override;
        void QueueWaitIdle(QueueID queue) override;
        void ResetCommandBuffer(CommandBufferID buffer) override;
        void BeginCommandBuffer(CommandBufferID buffer, bool singleUse) override;
        void EndCommandBuffer(CommandBufferID buffer) override;
        void* GetMappedBufferData(BufferID buffer) override;
        size_t GetBufferSize(BufferID buffer) override;
        bool IsFenceSignaled(FenceID fence) override;
        SurfaceCapabilities GetSurfaceCapabilities(SurfaceID surface) override;
        Format FindDepthFormat() override;
        bool HasTransferQueue() override;
        void UpdateDescriptorSets(std::span<WriteDescriptorSet> writes) override;

        void CmdCopyBuffer(CommandBufferID buffer, BufferID src, BufferID dst, std::span<BufferCopy> regions) override;
        void CmdCopyBufferToImage(CommandBufferID buffer, BufferID src, ImageID dst,
            ImageLayout layout, std::span<BufferImageCopy> regions) override;
        void CmdPipelineBarrier(CommandBufferID buffer,
            PipelineStage srcStage, PipelineStage dstStage,
            std::span<ImageMemoryBarrier> imageMemoryBarriers) override;
        void CmdBeginRendering(CommandBufferID buffer, const RenderingInfo& info) override;
        void CmdSetViewport(CommandBufferID buffer, Extent2D size) override;
        void CmdSetScissor(CommandBufferID buffer, Rect2D rect) override;
        void CmdBindPipeline(CommandBufferID buffer, PipelineBindPoint bindPoint,
            PipelineID pipeline) override;
        void CmdSetCullMode(CommandBufferID buffer, CullMode cullMode) override;
        void CmdPushConstants(CommandBufferID buffer, PipelineLayoutID layout,
            ShaderStageFlagBits stage, size_t offset, size_t size, const void* values) override;
        void CmdBindDescriptorSets(CommandBufferID buffer, PipelineBindPoint bindPoint,
            PipelineLayoutID layout, size_t firstSet, std::span<DescriptorSetID> sets) override;
        void CmdBindVertexBuffers(CommandBufferID buffer, uint32_t firstBinding, std::span<BufferID> buffers) override;
        void CmdBindIndexBuffer(CommandBufferID buffer, BufferID indexBuffer,
            uint64_t offset) override;
        void CmdDrawIndexed(CommandBufferID buffer, uint32_t indexCount,
            uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset,
            uint32_t firstInstance) override;
        void CmdEndRendering(CommandBufferID buffer) override;

        void TransitionImageLayout(CommandBufferID buffer, ImageID image, ImageLayout oldLayout,
                                   ImageLayout newLayout, ImageAspect aspect) override;

        [[nodiscard]] Scope<Renderer> CreateRenderer(GraphicsResources& resources) override;
        [[nodiscard]] Scope<GraphicsResources> CreateResources() override;
    private:
        struct QueueFamilyIndices
        {
            bool hasGraphicsFamily = false;
            bool hasComputeFamily = false;
            bool hasTransferFamily = false;
            uint32_t graphicsFamilyIndex = 0;
            uint32_t computeFamilyIndex = 0;
            uint32_t transferFamilyIndex = 0;
        };

        void CreateInstance(std::string_view applicationName);
        void CreateDevice();
        void CreateAllocator();

        vk::PhysicalDevice PickDevice();

        void FindQueueFamilies(vk::PhysicalDevice device);
        bool IsDeviceSuitable(vk::PhysicalDevice device);
        [[nodiscard]] vk::PresentModeKHR PickPresentMode(vk::SurfaceKHR surface, bool vsync) const;

        static bool CheckDeviceExtensionSupport(vk::PhysicalDevice device,
            const char* const* deviceExtensions, uint64_t extensionCount);
        static bool IsValidationSupported();

        vk::Instance m_instance;
        vk::DebugUtilsMessengerEXT m_messenger;
        vk::PhysicalDevice m_physicalDevice;
        vk::Device m_device;

        VmaAllocator m_allocator = nullptr;

        QueueFamilyIndices m_indices;
    };
}

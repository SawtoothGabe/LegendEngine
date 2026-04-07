#pragma once

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
        std::vector<DescriptorSetID> AllocateDescriptorSets() override;
        BufferID CreateBuffer(BufferUsageFlags flags, std::size_t size, bool createMapped) override;
        CommandPoolID CreateCommandPool(QueueFamily family) override;
        FenceID CreateFence(bool signaled) override;
        ImageID CreateImage(const ImageInfo& info) override;
        ImageViewID CreateImageView(ImageID image, Format format, ImageViewType type) override;
        PipelineID CreatePipeline(const PipelineInfo& info) override;
        PipelineLayoutID CreatePipelineLayout(std::span<PushConstantRange> ranges,
            std::span<DescriptorSetLayoutID> layouts) override;
        SemaphoreID CreateSemaphore() override;
        SwapchainID CreateSwapchain(const SwapchainInfo& info) override;
        SurfaceID CreateSurface(Window& window) override;
        ShaderModuleID CreateShaderModule(const ShaderModuleInfo& info) override;
        DescriptorSetLayoutID CreateDescriptorSetLayout(std::span<DescriptorSetLayoutBinding> bindings) override;
        SamplerID CreateSampler(const SamplerInfo& info) override;
        QueueID GetQueue(QueueFamily family) override;

        void FreeCommandBuffers(CommandPoolID pool, size_t count, CommandBufferID* buffers) override;
        void FreeDescriptorSets(size_t count, DescriptorSetID* sets) override;
        void DestroyBuffer(BufferID buffer) override;
        void DestroyCommandPool(CommandPoolID pool) override;
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

        void WaitForFences(size_t count, FenceID* fences) override;
        void WaitIdle() override;
        void ResetFences(size_t count, FenceID* fences) override;
        void QueueSubmit(QueueID queue, const SubmitInfo& info) override;
        void QueuePresent(QueueID queue, const PresentInfo& info) override;
        void ResetCommandBuffer(CommandBufferID buffer) override;
        void BeginCommandBuffer(CommandBufferID buffer, bool singleUse) override;
        void EndCommandBuffer(CommandBufferID buffer) override;

        void CmdCopyBuffer(CommandBufferID buffer, BufferID src, BufferID dst, std::span<BufferCopy> regions) override;
        void CmdCopyBufferToImage(CommandBufferID buffer) override;
        void CmdPipelineBarrier(CommandBufferID buffer) override;
        void CmdBeginRendering(CommandBufferID buffer) override;
        void CmdSetViewport(CommandBufferID buffer) override;
        void CmdSetScissor(CommandBufferID buffer) override;
        void CmdBindPipeline(CommandBufferID buffer) override;
        void CmdSetCullMode(CommandBufferID buffer) override;
        void CmdPushConstants(CommandBufferID buffer) override;
        void CmdBindDescriptorSets(CommandBufferID buffer) override;
        void CmdBindVertexBuffers(CommandBufferID buffer) override;
        void CmdBindIndexBuffer(CommandBufferID buffer) override;
        void CmdDrawIndexed(CommandBufferID buffer) override;
        void CmdEndRendering(CommandBufferID buffer) override;
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
        vk::PresentModeKHR PickPresentMode(vk::SurfaceKHR surface, bool vsync) const;

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

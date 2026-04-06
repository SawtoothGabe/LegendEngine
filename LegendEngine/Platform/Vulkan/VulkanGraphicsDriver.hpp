#pragma once

#include <LE/Graphics/GraphicsDriver.hpp>
#include <vulkan/vulkan.hpp>

namespace le
{
    class VulkanGraphicsDriver : public GraphicsDriver
    {
    public:
        VulkanGraphicsDriver(const std::string_view applicationName);
        ~VulkanGraphicsDriver();

        Scope<Renderer> CreateRenderer(CommandPoolID pool) override;

        void AllocateCommandBuffers(CommandPoolID pool) override;
        void AllocateDescriptorSets() override;
        BufferID CreateBuffer(BufferUsageFlags flags, std::size_t size, bool createMapped) override;
        CommandPoolID CreateCommandPool() override;
        FenceID CreateFence() override;
        ImageID CreateImage() override;
        ImageViewID CreateImageView() override;
        PipelineID CreatePipeline() override;
        SemaphoreID CreateSemaphore() override;
        SwapchainID CreateSwapchain() override;
        SurfaceID CreateSurface() override;
        ShaderModuleID CreateShaderModule() override;
        DescriptorSetLayoutID CreateDescriptorSetLayout() override;
        SamplerID CreateSampler() override;

        void FreeCommandBuffers() override;
        void FreeDescriptorSets() override;
        void DestroyBuffer(BufferID buffer) override;
        void DestroyCommandPool(CommandPoolID pool) override;
        void DestroyFence(FenceID fence) override;
        void DestroyImage(ImageID image) override;
        void DestroyImageView(ImageViewID view) override;
        void DestroyPipeline(PipelineID pipeline) override;
        void DestroySemaphore(SemaphoreID semaphore) override;
        void DestroySwapchain(SwapchainID swapchain) override;
        void DestroySurface(SurfaceID surface) override;
        void DestroyShaderModule(ShaderModuleID shaderModule) override;
        void DestroyDescriptorSetLayout(DescriptorSetLayoutID layout) override;
        void DestroySampler(SamplerID sampler) override;

        void WaitForFences(size_t count, FenceID* fences) override;
        void WaitIdle() override;
        void ResetFences(size_t count, uint64_t* fences) override;
        void QueueSubmit() override;
        void QueuePresent() override;
        void ResetCommandBuffer(CommandBufferID buffer) override;
        void BeginCommandBuffer(CommandBufferID buffer) override;
        void EndCommandBuffer(CommandBufferID buffer) override;

        void CmdCopyBuffer(CommandBufferID buffer) override;
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
        void CreateInstance(std::string_view applicationName);
        void CreateDevice();

        vk::PhysicalDevice PickDevice();

        void FindQueueFamilies(vk::PhysicalDevice device);
        bool IsDeviceSuitable(vk::PhysicalDevice device);
        static bool CheckDeviceExtensionSupport(vk::PhysicalDevice device,
            const char* const* deviceExtensions, uint64_t extensionCount);
        static bool IsValidationSupported();

        vk::Instance m_instance;
        vk::DebugUtilsMessengerEXT m_messenger;
        vk::PhysicalDevice m_physicalDevice;
        vk::Device m_device;

        QueueFamilyIndices m_indices;
    };
}

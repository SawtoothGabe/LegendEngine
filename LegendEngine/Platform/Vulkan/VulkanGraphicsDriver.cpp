#include "VulkanGraphicsDriver.hpp"

namespace le
{
    Scope<GraphicsDriver> CreateVulkanGraphicsDriver(std::string_view applicationName)
    {
        return std::make_unique<vk::VulkanGraphicsDriver>();
    }
}

namespace le::vk
{
    Scope<Renderer> VulkanGraphicsDriver::CreateRenderer(CommandPoolID pool) {}
    void VulkanGraphicsDriver::AllocateCommandBuffers(CommandPoolID pool) {}
    void VulkanGraphicsDriver::AllocateDescriptorSets() {}
    BufferID VulkanGraphicsDriver::CreateBuffer(BufferUsageFlags flags, std::size_t size, bool createMapped) {}
    CommandPoolID VulkanGraphicsDriver::CreateCommandPool()
    {
        return CommandPoolID();
    }

    FenceID VulkanGraphicsDriver::CreateFence() {}
    ImageID VulkanGraphicsDriver::CreateImage() {}
    ImageViewID VulkanGraphicsDriver::CreateImageView() {}
    PipelineID VulkanGraphicsDriver::CreatePipeline() {}
    SemaphoreID VulkanGraphicsDriver::CreateSemaphore() {}
    SwapchainID VulkanGraphicsDriver::CreateSwapchain() {}
    SurfaceID VulkanGraphicsDriver::CreateSurface() {}
    ShaderModuleID VulkanGraphicsDriver::CreateShaderModule() {}
    DescriptorSetLayoutID VulkanGraphicsDriver::CreateDescriptorSetLayout() {}
    SamplerID VulkanGraphicsDriver::CreateSampler() {}
    void VulkanGraphicsDriver::FreeCommandBuffers() {}
    void VulkanGraphicsDriver::FreeDescriptorSets() {}
    void VulkanGraphicsDriver::DestroyBuffer(BufferID buffer) {}
    void VulkanGraphicsDriver::DestroyCommandPool(CommandPoolID pool) {}
    void VulkanGraphicsDriver::DestroyFence(FenceID fence) {}
    void VulkanGraphicsDriver::DestroyImage(ImageID image) {}
    void VulkanGraphicsDriver::DestroyImageView(ImageViewID view) {}
    void VulkanGraphicsDriver::DestroyPipeline(PipelineID pipeline) {}
    void VulkanGraphicsDriver::DestroySemaphore(SemaphoreID semaphore) {}
    void VulkanGraphicsDriver::DestroySwapchain(SwapchainID swapchain) {}
    void VulkanGraphicsDriver::DestroySurface(SurfaceID surface) {}
    void VulkanGraphicsDriver::DestroyShaderModule(ShaderModuleID shaderModule) {}
    void VulkanGraphicsDriver::DestroyDescriptorSetLayout(DescriptorSetLayoutID layout) {}
    void VulkanGraphicsDriver::DestroySampler(SamplerID sampler) {}
    void VulkanGraphicsDriver::WaitForFences(size_t count, FenceID* fences) {}
    void VulkanGraphicsDriver::WaitIdle() {}
    void VulkanGraphicsDriver::ResetFences(size_t count, uint64_t* fences) {}
    void VulkanGraphicsDriver::QueueSubmit() {}
    void VulkanGraphicsDriver::QueuePresent() {}
    void VulkanGraphicsDriver::ResetCommandBuffer(CommandBufferID buffer) {}
    void VulkanGraphicsDriver::BeginCommandBuffer(CommandBufferID buffer) {}
    void VulkanGraphicsDriver::EndCommandBuffer(CommandBufferID buffer) {}
    void VulkanGraphicsDriver::CmdCopyBuffer(CommandBufferID buffer) {}
    void VulkanGraphicsDriver::CmdCopyBufferToImage(CommandBufferID buffer) {}
    void VulkanGraphicsDriver::CmdPipelineBarrier(CommandBufferID buffer) {}
    void VulkanGraphicsDriver::CmdBeginRendering(CommandBufferID buffer) {}
    void VulkanGraphicsDriver::CmdSetViewport(CommandBufferID buffer) {}
    void VulkanGraphicsDriver::CmdSetScissor(CommandBufferID buffer) {}
    void VulkanGraphicsDriver::CmdBindPipeline(CommandBufferID buffer) {}
    void VulkanGraphicsDriver::CmdSetCullMode(CommandBufferID buffer) {}
    void VulkanGraphicsDriver::CmdPushConstants(CommandBufferID buffer) {}
    void VulkanGraphicsDriver::CmdBindDescriptorSets(CommandBufferID buffer) {}
    void VulkanGraphicsDriver::CmdBindVertexBuffers(CommandBufferID buffer) {}
    void VulkanGraphicsDriver::CmdBindIndexBuffer(CommandBufferID buffer) {}
    void VulkanGraphicsDriver::CmdDrawIndexed(CommandBufferID buffer) {}
    void VulkanGraphicsDriver::CmdEndRendering(CommandBufferID buffer) {}
}

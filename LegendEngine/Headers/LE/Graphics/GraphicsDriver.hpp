#pragma once

#include <cstdint>
#include <functional>

#include <LE/Common/Assert.hpp>
#include <LE/Common/Types.hpp>
#include <LE/Graphics/Types.hpp>

namespace le
{
    class GraphicsDriver
    {
    public:
        virtual ~GraphicsDriver() = default;

        virtual Scope<Renderer> CreateRenderer() = 0;

        virtual void AllocateCommandBuffers(CommandPoolID pool) = 0;
        virtual void AllocateDescriptorSets() = 0;
        virtual BufferID CreateBuffer(BufferUsageFlags flags, std::size_t size, bool createMapped) = 0;
        virtual CommandPoolID CreateCommandPool() = 0;
        virtual FenceID CreateFence() = 0;
        virtual ImageID CreateImage() = 0;
        virtual ImageViewID CreateImageView() = 0;
        virtual PipelineID CreatePipeline() = 0;
        virtual SemaphoreID CreateSemaphore() = 0;
        virtual SwapchainID CreateSwapchain() = 0;
        virtual SurfaceID CreateSurface() = 0;
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

        static Scope<GraphicsDriver> Create(GraphicsAPI api,
            std::string_view applicationName);
    };
    
#ifdef LE_VULKAN_API
    Scope<GraphicsDriver> CreateVulkanGraphicsDriver(std::string_view);
#endif
#ifdef LE_OPENGL_API
    Scope<GraphicsDriver> CreateOpenGLGraphicsDriver(std::string_view);
#endif
#ifdef LE_D3D11_API
    Scope<GraphicsDriver> CreateD3D11GraphicsDriver(std::string_view);
#endif
#ifdef LE_D3D12_API
    Scope<GraphicsDriver> CreateD3D12GraphicsDriver(std::string_view);
#endif
#ifdef LE_WEBGPU_API
    Scope<GraphicsDriver> CreateWebGPUGraphicsDriver(std::string_view);
#endif

    inline Scope<GraphicsDriver> GraphicsDriver::Create(const GraphicsAPI api,
        const std::string_view applicationName)
    {
        switch (api)
        {
#ifdef LE_VULKAN_API
            case GraphicsAPI::VULKAN: return CreateVulkanGraphicsDriver(applicationName);
#endif
#ifdef LE_OPENGL_API
            case GraphicsAPI::OPENGL: return CreateOpenGLGraphicsDriver(applicationName);
#endif
#ifdef LE_D3D11_API
            case GraphicsAPI::D3D11:  return CreateD3D11GraphicsDriver(applicationName);
#endif
#ifdef LE_D3D12_API
            case GraphicsAPI::D3D12:  return CreateD3D12GraphicsDriver(applicationName);
#endif
#ifdef LE_WEBGPU_API
            case GraphicsAPI::WEBGPU: return CreateWebGPUGraphicsDriver(applicationName);
#endif

                // Make clang tidy happy
            default: break;
        }

        LE_ASSERT(false, "Unknown graphics API. Was the program linked with the relevant library for it?");
        return nullptr;
    }
}

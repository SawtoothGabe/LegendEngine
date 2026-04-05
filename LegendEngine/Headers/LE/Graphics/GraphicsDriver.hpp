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

        virtual Scope<Renderer> CreateBackend() = 0;

        virtual uint64_t AllocateCommandBuffers(uint64_t pool) = 0;
        virtual uint64_t AllocateDescriptorSets() = 0;
        virtual uint64_t CreateBuffer(BufferUsageFlags flags, std::size_t size, bool createMapped) = 0;
        virtual uint64_t CreateCommandPool() = 0;
        virtual uint64_t CreateFence() = 0;
        virtual uint64_t CreateImage() = 0;
        virtual uint64_t CreateImageView() = 0;
        virtual uint64_t CreatePipeline() = 0;
        virtual uint64_t CreateSemaphore() = 0;
        virtual uint64_t CreateSwapchain() = 0;
        virtual uint64_t CreateSurface() = 0;
        virtual uint64_t CreateShaderModule() = 0;
        virtual uint64_t CreateDescriptorSetLayout() = 0;
        virtual uint64_t CreateSampler() = 0;

        virtual void FreeCommandBuffers() = 0;
        virtual void FreeDescriptorSets() = 0;
        virtual void DestroyBuffer(uint64_t resource) = 0;
        virtual void DestroyCommandBuffer(uint64_t resource) = 0;
        virtual void DestroyCommandPool(uint64_t resource) = 0;
        virtual void DestroyFence(uint64_t resource) = 0;
        virtual void DestroyImage(uint64_t resource) = 0;
        virtual void DestroyImageView(uint64_t resource) = 0;
        virtual void DestroyPipeline(uint64_t resource) = 0;
        virtual void DestroySemaphore(uint64_t resource) = 0;
        virtual void DestroySwapchain(uint64_t resource) = 0;
        virtual void DestroySurface(uint64_t resource) = 0;
        virtual void DestroyShaderModule(uint64_t resource) = 0;

        virtual void WaitForFences(size_t count, uint64_t* fences) = 0;
        virtual void WaitIdle() = 0;
        virtual void ResetFences(size_t count, uint64_t* fences) = 0;
        virtual void QueueSubmit() = 0;
        virtual void QueuePresent() = 0;
        virtual void ResetCommandBuffer(uint64_t buffer) = 0;
        virtual void BeginCommandBuffer(uint64_t buffer) = 0;
        virtual void EndCommandBuffer(uint64_t buffer) = 0;

        virtual void CmdCopyBuffer(uint64_t buffer) = 0;
        virtual void CmdCopyBufferToImage(uint64_t buffer) = 0;
        virtual void CmdPipelineBarrier(uint64_t buffer) = 0;
        virtual void CmdBeginRendering(uint64_t buffer) = 0;
        virtual void CmdSetViewport(uint64_t buffer) = 0;
        virtual void CmdSetScissor(uint64_t buffer) = 0;
        virtual void CmdBindPipeline(uint64_t buffer) = 0;
        virtual void CmdSetCullMode(uint64_t buffer) = 0;
        virtual void CmdPushConstants(uint64_t buffer) = 0;
        virtual void CmdBindDescriptorSets(uint64_t buffer) = 0;
        virtual void CmdBindVertexBuffers(uint64_t buffer) = 0;
        virtual void CmdBindIndexBuffer(uint64_t buffer) = 0;
        virtual void CmdDrawIndexed(uint64_t buffer) = 0;
        virtual void CmdEndRendering(uint64_t buffer) = 0;

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

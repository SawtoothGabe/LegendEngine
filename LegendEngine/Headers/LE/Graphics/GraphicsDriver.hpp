#pragma once

#include <string_view>
#include <LE/Common/Assert.hpp>
#include <LE/Common/Types.hpp>
#include <LE/Events/EventBus.hpp>

#include <LE/Graphics/Types.hpp>

namespace le
{
    class Renderer;
    class GraphicsResources;
    class GraphicsDriver
    {
    public:
        virtual ~GraphicsDriver() = default;

        [[nodiscard]] virtual Scope<GraphicsResources> CreateResources(EventBus& bus) = 0;
        [[nodiscard]] virtual Scope<Renderer> CreateRenderer(GraphicsResources& resources) = 0;

        [[nodiscard]] virtual SamplerID CreateSampler(const SamplerInfo& info) = 0;

        virtual void DestroySampler(SamplerID sampler) = 0;

        static Scope<GraphicsDriver> Create(GraphicsAPI api,
            std::string_view applicationName);
    };

    Scope<GraphicsDriver> CreateDummyGraphicsDriver();

#ifdef LE_VULKAN_API
    Scope<GraphicsDriver> CreateVulkanGraphicsDriver(std::string_view applicationName);
#endif
#ifdef LE_OPENGL_API
    Scope<GraphicsDriver> CreateOpenGLGraphicsDriver(std::string_view applicationName);
#endif
#ifdef LE_D3D11_API
    Scope<GraphicsDriver> CreateD3D11GraphicsDriver(std::string_view applicationName);
#endif
#ifdef LE_D3D12_API
    Scope<GraphicsDriver> CreateD3D12GraphicsDriver(std::string_view applicationName);
#endif
#ifdef LE_WEBGPU_API
    Scope<GraphicsDriver> CreateWebGPUGraphicsDriver(std::string_view applicationName);
#endif

    inline Scope<GraphicsDriver> GraphicsDriver::Create(const GraphicsAPI api,
        const std::string_view applicationName)
    {
        switch (api)
        {
            case GraphicsAPI::NONE: return CreateDummyGraphicsDriver();

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

            // Get rid of no default case warning
            default:;
        }

        LE_ASSERT(false, "Unknown graphics API. Was the program linked with the relevant library for it?");
        return nullptr;
    }
}

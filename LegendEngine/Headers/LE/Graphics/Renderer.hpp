#pragma once

#include <LE/TetherBindings.hpp>
#include <LE/Graphics/Types.hpp>
#include <LE/IO/TextureData.hpp>
#include <LE/Resources/MeshData.hpp>

namespace le
{
    class Scene;
    class Renderer
    {
    public:
        virtual ~Renderer() = default;

        virtual void StartFrame() = 0;
        virtual void RenderFrame(RenderTargetID& target, std::span<Scene*> scenes) = 0;
        virtual void EndFrame() = 0;

        static Scope<Renderer> Create(GraphicsAPI api,
            std::string_view applicationName);
    };

#ifdef LE_VULKAN_API
    Scope<Renderer> CreateVulkanRenderer(std::string_view);
#endif
#ifdef LE_OPENGL_API
    Scope<Renderer> CreateOpenGLRenderer(std::string_view);
#endif
#ifdef LE_D3D11_API
    Scope<Renderer> CreateD3D11Renderer(std::string_view);
#endif
#ifdef LE_D3D12_API
    Scope<Renderer> CreateD3D12Renderer(std::string_view);
#endif
#ifdef LE_WEBGPU_API
    Scope<Renderer> CreateWebGPURenderer(std::string_view);
#endif

    inline Scope<Renderer> Renderer::Create(const GraphicsAPI api,
        const std::string_view applicationName)
    {
        switch (api)
        {
#ifdef LE_VULKAN_API
            case GraphicsAPI::VULKAN: return CreateVulkanRenderer(applicationName);
#endif
#ifdef LE_OPENGL_API
            case GraphicsAPI::OPENGL: return CreateOpenGLRenderer(applicationName);
#endif
#ifdef LE_D3D11_API
            case GraphicsAPI::D3D11:  return CreateD3D11Renderer(applicationName);
#endif
#ifdef LE_D3D12_API
            case GraphicsAPI::D3D12:  return CreateD3D12Renderer(applicationName);
#endif
#ifdef LE_WEBGPU_API
            case GraphicsAPI::WEBGPU: return CreateWebGPURenderer(applicationName);
#endif

                // Make clang tidy happy
            default: break;
        }

        LE_ASSERT(false, "Unknown graphics API. Was the program linked with the relevant library for it?");
        return nullptr;
    }
}

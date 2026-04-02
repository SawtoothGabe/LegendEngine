#pragma once

#include <memory>
#include <LE/Common/Defs.hpp>
#include <LE/Common/Types.hpp>
#include <LE/Graphics/Renderer.hpp>
#include <LE/Graphics/RenderTarget.hpp>
#include <LE/Graphics/ShaderManager.hpp>
#include <LE/Graphics/API/Buffer.hpp>
#include <LE/Graphics/API/CommandBuffer.hpp>
#include <LE/Graphics/API/DescriptorSetLayout.hpp>
#include <LE/Graphics/API/DynamicUniforms.hpp>
#include <LE/Graphics/API/Image.hpp>
#include <LE/Graphics/API/Pipeline.hpp>
#include <LE/Graphics/API/Sampler.hpp>
#include <LE/Resources/Shader.hpp>
#include <Tether/Window.hpp>

namespace le
{
    class DescriptorPool;
    class Material;

    class GraphicsContext
    {
    public:
        using ShaderStages = std::span<Shader::Stage>;

        GraphicsContext() = default;
        virtual ~GraphicsContext() = 0;
        LE_NO_COPY(GraphicsContext);

        virtual Scope<Renderer> CreateRenderer(RenderTarget& renderTarget, GraphicsResources& resources) = 0;
        virtual Scope<RenderTarget> CreateHeadlessRenderTarget() = 0;
#ifndef LE_HEADLESS
        virtual Scope<RenderTarget> CreateWindowRenderTarget(Tether::Window& window) = 0;
#endif
        virtual Scope<Buffer> CreateSimpleBuffer(Buffer::UsageFlags usage, size_t size, bool createMapped) = 0;
        virtual Scope<Buffer> CreateSmartBuffer(Buffer::UsageFlags usage) = 0;
        virtual Scope<Buffer> CreatePerFrameBuffer(Buffer::UsageFlags usage, size_t size) = 0;
        virtual Scope<CommandBuffer> CreateCommandBuffer(bool transfer) = 0;
        virtual Scope<DescriptorSetLayout> CreateDescriptorSetLayout(std::span<DescriptorSetLayout::Binding> bindings) = 0;
        virtual Scope<DynamicUniforms> CreateDynamicUniforms(
            DynamicUniforms::UpdateFrequency frequency, DescriptorSetLayout& layout) = 0;
        virtual Scope<Pipeline> CreatePipeline(std::span<Shader::Stage> stages, std::span<DescriptorSetLayout*> layouts) = 0;
        virtual Scope<Image> CreateImage(const Image::Info& info) = 0;
        virtual Scope<Sampler> CreateSampler(const Sampler::Info& info) = 0;

        virtual void WaitIdle() = 0;

        virtual void RegisterShaders(ShaderManager& shaderManager) = 0;

        static Scope<GraphicsContext> Create(GraphicsAPI api,
            std::string_view applicationName);
    };

#ifdef LE_VULKAN_API
    Scope<GraphicsContext> CreateVulkanGraphicsContext(std::string_view);
#endif
#ifdef LE_OPENGL_API
    Scope<GraphicsContext> CreateOpenGLGraphicsContext(std::string_view);
#endif
#ifdef LE_D3D11_API
    Scope<GraphicsContext> CreateD3D11GraphicsContext(std::string_view);
#endif
#ifdef LE_D3D12_API
    Scope<GraphicsContext> CreateD3D12GraphicsContext(std::string_view);
#endif
#ifdef LE_WEBGPU_API
    Scope<GraphicsContext> CreateWebGPUGraphicsContext(std::string_view);
#endif

    inline Scope<GraphicsContext> GraphicsContext::Create(const GraphicsAPI api,
        const std::string_view applicationName)
    {
        switch (api)
        {
#ifdef LE_VULKAN_API
            case GraphicsAPI::VULKAN: return CreateVulkanGraphicsContext(applicationName);
#endif
#ifdef LE_OPENGL_API
            case GraphicsAPI::OPENGL: return CreateOpenGLGraphicsContext(applicationName);
#endif
#ifdef LE_D3D11_API
            case GraphicsAPI::D3D11:  return CreateD3D11GraphicsContext(applicationName);
#endif
#ifdef LE_D3D12_API
            case GraphicsAPI::D3D12:  return CreateD3D12GraphicsContext(applicationName);
#endif
#ifdef LE_WEBGPU_API
            case GraphicsAPI::WEBGPU: return CreateWebGPUGraphicsContext(applicationName);
#endif

                // Make clang tidy happy
            default: break;
        }

        LE_ASSERT(false, "Unknown graphics API. Was the program linked with the relevant library for it?");
        return nullptr;
    }
}

#pragma once

#include <LE/Graphics/GraphicsContext.hpp>
#include <Tether/Rendering/Vulkan/ContextCreator.hpp>
#include <Tether/Rendering/Vulkan/GraphicsContext.hpp>
#include <Tether/Rendering/Vulkan/Instance.hpp>

namespace le::vk
{
    namespace TetherVulkan = Tether::Rendering::Vulkan;

    class GraphicsContext final : public le::GraphicsContext
    {
    public:
        explicit GraphicsContext(std::string_view applicationName);
        ~GraphicsContext() override;

        Scope<Renderer> CreateRenderer(RenderTarget& renderTarget, GraphicsResources& resources) override;
        Scope<RenderTarget> CreateHeadlessRenderTarget() override;
#ifndef LE_HEADLESS
        Scope<RenderTarget> CreateWindowRenderTarget(Tether::Window& window) override;
#endif
        Scope<Buffer> CreateSimpleBuffer(Buffer::UsageFlags usage, size_t size, bool createMapped) override;
        Scope<Buffer> CreateSmartBuffer(Buffer::UsageFlags usage) override;
        Scope<Buffer> CreatePerFrameBuffer(Buffer::UsageFlags usage, size_t size) override;
        Scope<CommandBuffer> CreateCommandBuffer(bool transfer) override;
        Scope<DescriptorSetLayout> CreateDescriptorSetLayout(std::span<DescriptorSetLayout::Binding> bindings) override;
        Scope<DynamicUniforms> CreateDynamicUniforms(DynamicUniforms::UpdateFrequency frequency, DescriptorSetLayout& layout) override;
        Scope<le::Pipeline> CreatePipeline(std::span<Shader::Stage> stages, std::span<DescriptorSetLayout*> layouts) override;
        Scope<Image> CreateImage(const Image::Info& info) override;
        Scope<Sampler> CreateSampler(const Sampler::Info& info) override;

        void WaitIdle() override;

        VkQueue GetTransferQueue() const;
        VkCommandPool GetTransferPool() const;
        VkFormat GetDepthFormat() const;

        std::mutex& GetGraphicsQueueMutex();
        std::mutex& GetTransferQueueMutex() const;

        TetherVulkan::GraphicsContext& GetTetherGraphicsContext();
    private:
        static TetherVulkan::ContextCreator::Info GetContextInfo(std::string_view applicationName);

        VkFormat FindDepthFormat() const;

        std::mutex& FindTransferMutex();

        void RegisterShaders(ShaderManager& shaderManager) override;
        void CreateUniforms();
        void CreateTransferQueue();

        class DebugCallback final : public TetherVulkan::DebugCallback
        {
        public:
            explicit DebugCallback(GraphicsContext& context);

            void OnDebugLog(
                VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT messageType,
                const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData
            ) override;
        private:
            GraphicsContext& m_Context;
        };

        TetherVulkan::ContextCreator m_ContextCreator;
        TetherVulkan::GraphicsContext m_GraphicsContext;

        DebugCallback m_Callback;

        VkSurfaceFormatKHR m_SurfaceFormat{};

        std::mutex m_GraphicsQueueMutex;
        std::mutex m_TransferQueueMutex;
        std::mutex& m_ActualTransferMutex;

        VkQueue m_TransferQueue = nullptr;
        VkCommandPool m_TransferPool = nullptr;

        VkFormat m_DepthFormat;
    };
}

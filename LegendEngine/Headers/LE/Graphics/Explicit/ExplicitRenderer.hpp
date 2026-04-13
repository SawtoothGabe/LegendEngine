#pragma once

#include <LE/Graphics/Renderer.hpp>
#include <LE/Graphics/Explicit/ExplicitDriver.hpp>
#include <LE/Resources/Shader.hpp>

namespace le
{
    class ExplicitRenderer final : public Renderer
    {
    public:
        explicit ExplicitRenderer(Scope<ExplicitDriver> driver);
        ~ExplicitRenderer() override;

        [[nodiscard]] MaterialID CreateMaterial() override;
        [[nodiscard]] MeshID CreateMesh() override;
        [[nodiscard]] ShaderID CreateShader(const sh::ShaderInfo& shaderInfo) override;
        [[nodiscard]] Texture2DID CreateTexture2D(const TextureData& loader) override;
        [[nodiscard]] Texture2DArrayID CreateTexture2DArray(size_t width, size_t height, uint8_t channels,
            const std::span<TextureData*>& textureData) override;
        [[nodiscard]] RenderTargetID CreateRenderTarget(Window& window) override;

        void DestroyMaterial(MaterialID id) override;
        void DestroyMesh(MeshID id) override;
        void DestroyShader(ShaderID id) override;
        void DestroyTexture2D(Texture2DID id) override;
        void DestroyTexture2DArray(Texture2DArrayID id) override;
        void DestroyRenderTarget(RenderTargetID id) override;

        void StartFrame() override;
        void RenderFrame(RenderTargetID& target, std::span<Scene*> scenes) override;
        void EndFrame() override;

        void EnqueueDeletionFunc(const std::function<void()>& func);
        [[nodiscard]] PoolManagerID& GetMaterialPoolManager() const;

        [[nodiscard]] CommandPoolID GetGraphicsPool() const;
        [[nodiscard]] CommandPoolID GetTransferPool() const;

        [[nodiscard]] QueueID GetGraphicsQueue() const;
        [[nodiscard]] QueueID GetTransferQueue() const;
        [[nodiscard]] std::mutex& GetGraphicsMutex() const;
        [[nodiscard]] std::mutex& GetTransferMutex() const;

        ImageID GetTexture2DImage(Texture2DID texture) override;
        ImageViewID GetTexture2DImageView(Texture2DID texture) override;
        ImageID GetTexture2DArrayImage(Texture2DArrayID texture) override;
        ImageViewID GetTexture2DArrayImageView(Texture2DArrayID texture) override;

        [[nodiscard]] ExplicitDriver& GetDriver() const;
    private:
        static constexpr auto COLOR_FORMAT = Format::B8G8R8A8_SRGB;

        void CreateCommandBuffers();
        void CreateSyncObjects();
        void CreatePipelineLayout();

        void ProcessDeletionQueue();

        void BeginScene();
        void UseMaterial();
        void DrawMesh();

        Scope<ExplicitDriver> m_driver;
        CommandPoolID m_gfxPool;
        Format m_depthFormat;

        bool m_vsync = false;

        PipelineLayoutID m_pipelineLayout;

        std::vector<CommandBufferID> m_commandBuffers;
        std::vector<FenceID> m_inFlightFences;
        std::vector<SemaphoreID> m_renderFinishedSemaphores;

        std::vector<std::vector<std::function<void()>>> m_deletionQueues;

        DescriptorSetID m_sets[3] = {};
        bool m_haveSetsChanged = true;
        Ref<Shader> m_currentShader = nullptr;

        size_t m_currentFrame = 0;
    };
}

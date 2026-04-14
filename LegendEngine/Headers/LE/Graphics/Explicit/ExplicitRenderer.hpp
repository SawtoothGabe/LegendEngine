#pragma once

#include <LE/Graphics/Renderer.hpp>
#include <LE/Graphics/Explicit/ExplicitDriver.hpp>
#include <LE/Resources/Material.hpp>
#include <LE/Resources/MeshData.hpp>
#include <LE/Resources/Shader.hpp>

namespace le
{
    class ExplicitRenderer final : public Renderer
    {
    public:
        explicit ExplicitRenderer(Scope<ExplicitDriver> driver);
        ~ExplicitRenderer() override;

        [[nodiscard]] MaterialID CreateMaterial() override;
        [[nodiscard]] MeshID CreateMesh(std::span<MeshData::Vertex3> vertices, std::span<uint32_t> indices,
            MeshData::UpdateFrequency frequency) override;
        [[nodiscard]] MeshID CreateMesh(size_t initialVertexCount, size_t initialIndexCount,
            MeshData::UpdateFrequency frequency) override;
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
        [[nodiscard]] PoolManagerID GetMaterialPoolManager() const;

        [[nodiscard]] CommandPoolID GetGraphicsPool() const;
        [[nodiscard]] CommandPoolID GetTransferPool() const;

        [[nodiscard]] QueueID GetGraphicsQueue() const;
        [[nodiscard]] QueueID GetTransferQueue() const;
        [[nodiscard]] std::mutex& GetGraphicsMutex();
        [[nodiscard]] std::mutex& GetTransferMutex() const;

        ImageID GetTexture2DImage(Texture2DID texture) override;
        ImageViewID GetTexture2DImageView(Texture2DID texture) override;
        ImageID GetTexture2DArrayImage(Texture2DArrayID texture) override;
        ImageViewID GetTexture2DArrayImageView(Texture2DArrayID texture) override;

        [[nodiscard]] ExplicitDriver& GetDriver() const;
    private:
        static constexpr auto COLOR_FORMAT = Format::B8G8R8A8_SRGB;

        void CreateQueues();
        void CreateCommandBuffers();
        void CreateSyncObjects();
        void CreateDescriptorSetLayouts();
        void CreatePipelineLayout();

        void ProcessDeletionQueue();

        void UseMaterial(const Ref<Material>& material);
        void RenderScene(Scene& scene);
        void BeginScene(Scene& scene);
        static void UpdateCamera(Scene& scene, UID cameraID);
        void DrawMesh(const Mesh& mesh, const Transform& transform);

        Scope<ExplicitDriver> m_driver;

        QueueID m_graphicsQueue;
        QueueID m_transferQueue;
        CommandPoolID m_graphicsPool;
        CommandPoolID m_transferPool;
        std::mutex m_graphicsMutex;
        std::mutex m_transferMutex;

        QueueID* m_pTransferQueue = nullptr;
        CommandPoolID* m_pTransferPool = nullptr;
        std::mutex* m_pTransferMutex = nullptr;

        Format m_depthFormat;

        bool m_vsync = false;

        PipelineLayoutID m_pipelineLayout;

        std::vector<CommandBufferID> m_commandBuffers;
        std::vector<FenceID> m_inFlightFences;
        std::vector<SemaphoreID> m_renderFinishedSemaphores;
        std::vector<RenderTargetID> m_targetsRendered;

        std::vector<std::vector<std::function<void()>>> m_deletionQueues;
        std::vector<DescriptorSetLayoutID> m_descriptorSetLayouts;

        PoolManagerID m_materialPool;

        DescriptorSetID m_sets[3] = {};
        bool m_haveSetsChanged = true;
        Ref<Shader> m_currentShader = nullptr;

        Ref<Material> m_defaultMaterial;

        size_t m_currentFrame = 0;
    };
}

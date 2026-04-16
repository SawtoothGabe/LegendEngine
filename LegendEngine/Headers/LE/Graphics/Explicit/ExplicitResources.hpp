#pragma once

#include <LE/Graphics/GraphicsResources.hpp>
#include <LE/Graphics/Explicit/ExplicitDriver.hpp>

namespace le
{
    class ExplicitResources : public GraphicsResources
    {
    public:
        ExplicitResources(ExplicitDriver& driver);
        ~ExplicitResources();

        [[nodiscard]] MaterialID CreateMaterial() override;
        [[nodiscard]] MeshID CreateMesh(std::span<MeshData::Vertex3> vertices, std::span<uint32_t> indices,
            MeshData::UpdateFrequency frequency) override;
        [[nodiscard]] MeshID CreateMesh(size_t initialVertexCount, size_t initialIndexCount,
            MeshData::UpdateFrequency frequency) override;
        [[nodiscard]] SceneID CreateScene() override;
        [[nodiscard]] ShaderID CreateShader(const sh::ShaderInfo& shaderInfo) override;
        [[nodiscard]] Texture2DID CreateTexture2D(const TextureData& loader) override;
        [[nodiscard]] Texture2DArrayID CreateTexture2DArray(size_t width, size_t height, uint8_t channels,
            const std::span<TextureData*>& textureData) override;
        [[nodiscard]] RenderTargetID CreateRenderTarget(Window& window) override;

        void DestroyMaterial(MaterialID id) override;
        void DestroyMesh(MeshID id) override;
        void DestroyScene(SceneID id) override;
        void DestroyShader(ShaderID id) override;
        void DestroyTexture2D(Texture2DID id) override;
        void DestroyTexture2DArray(Texture2DArrayID id) override;
        void DestroyRenderTarget(RenderTargetID id) override;

        void UpdateMesh(MeshID id, std::span<MeshData::Vertex3> vertices, std::span<uint32_t> indices) override;
        void ResizeMesh(MeshID id, size_t vertexCount, size_t indexCount) override;
        void SetMaterialTexture(MaterialID id, Ref<Texture> texture) override;
        void SetMaterialColor(MaterialID id, Color color) override;
        void SetMaterialShader(MaterialID id, ShaderID shader) override;
        void SetSceneAmbientLight(SceneID id, float level) override;

        ImageID GetTexture2DImage(Texture2DID texture) override;
        ImageViewID GetTexture2DImageView(Texture2DID texture) override;
        ImageID GetTexture2DArrayImage(Texture2DArrayID texture) override;
        ImageViewID GetTexture2DArrayImageView(Texture2DArrayID texture) override;

        [[nodiscard]] PoolManagerID GetCameraPoolManager() const;
        [[nodiscard]] PoolManagerID GetScenePoolManager() const;
        [[nodiscard]] PoolManagerID GetMaterialPoolManager() const;

        [[nodiscard]] CommandPoolID GetGraphicsPool() const;
        [[nodiscard]] CommandPoolID GetTransferPool() const;

        [[nodiscard]] QueueID GetGraphicsQueue() const;
        [[nodiscard]] QueueID GetTransferQueue() const;
        [[nodiscard]] std::mutex& GetGraphicsMutex();
        [[nodiscard]] std::mutex& GetTransferMutex() const;

        void ProcessDeletionQueue();
        void EnqueueDeletionFunc(const std::function<void()>& func);
    private:
        static constexpr auto COLOR_FORMAT = Format::B8G8R8A8_SRGB;

        ExplicitDriver& m_driver;

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

        std::vector<DescriptorSetLayoutID> m_descriptorSetLayouts;

        PoolManagerID m_cameraPool;
        PoolManagerID m_scenePool;
        PoolManagerID m_materialPool;

        PipelineLayoutID m_pipelineLayout;

        size_t m_currentFrame = 0;
        std::vector<std::vector<std::function<void()>>> m_deletionQueues;
    };
}

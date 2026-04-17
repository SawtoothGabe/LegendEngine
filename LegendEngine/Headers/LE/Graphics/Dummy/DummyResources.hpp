#pragma once
#include <LE/Graphics/GraphicsResources.hpp>

namespace le
{
    class DummyResources : public GraphicsResources
    {
    public:
        [[nodiscard]] Scope<RenderTarget> CreateRenderTarget(Tether::Window& window) override;

        [[nodiscard]] MaterialID CreateMaterial() override;
        [[nodiscard]] MeshID CreateMesh(std::span<MeshData::Vertex3> vertices, std::span<uint32_t> indices,
            MeshData::UpdateFrequency frequency) override;
        [[nodiscard]] MeshID CreateMesh(size_t initialVertexCount, size_t initialIndexCount,
            MeshData::UpdateFrequency frequency) override;
        [[nodiscard]] SceneID CreateScene() override;
        [[nodiscard]] ShaderID CreateShader(const ShaderInfo& shaderInfo) override;
        [[nodiscard]] Texture2DID CreateTexture2D(const TextureData& loader) override;
        [[nodiscard]] Texture2DArrayID CreateTexture2DArray(size_t width, size_t height, uint8_t channels,
            const std::span<TextureData*>& textureData) override;

        void DestroyMaterial(MaterialID id) override;
        void DestroyMesh(MeshID id) override;
        void DestroyScene(SceneID id) override;
        void DestroyShader(ShaderID id) override;
        void DestroyTexture2D(Texture2DID id) override;
        void DestroyTexture2DArray(Texture2DArrayID id) override;

        void UpdateMesh(MeshID id, std::span<MeshData::Vertex3> vertices, std::span<uint32_t> indices) override;
        void ResizeMesh(MeshID id, size_t vertexCount, size_t indexCount) override;
        void SetMaterialTexture(MaterialID id, Ref<Texture> texture) override;
        void SetMaterialColor(MaterialID id, Color color) override;
        void SetSceneAmbientLight(SceneID id, float level) override;

        ImageID GetTexture2DImage(Texture2DID texture) override;
        ImageViewID GetTexture2DImageView(Texture2DID texture) override;
        ImageID GetTexture2DArrayImage(Texture2DArrayID texture) override;
        ImageViewID GetTexture2DArrayImageView(Texture2DArrayID texture) override;
    };
}

#pragma once

#include <LE/TetherBindings.hpp>
#include <LE/Graphics/Types.hpp>
#include <LE/IO/TextureData.hpp>
#include <LE/Resources/MeshData.hpp>
#include <LE/Resources/Shader.hpp>
#include <LE/Resources/Texture.hpp>

namespace le
{
    class GraphicsResources
    {
    public:
        virtual ~GraphicsResources() = default;

        [[nodiscard]] virtual MaterialID CreateMaterial() = 0;
        [[nodiscard]] virtual MeshID CreateMesh(std::span<MeshData::Vertex3> vertices, std::span<uint32_t> indices,
            MeshData::UpdateFrequency frequency) = 0;
        [[nodiscard]] virtual MeshID CreateMesh(size_t initialVertexCount, size_t initialIndexCount,
            MeshData::UpdateFrequency frequency) = 0;
        [[nodiscard]] virtual SceneID CreateScene() = 0;
        [[nodiscard]] virtual ShaderID CreateShader(const sh::ShaderInfo& shaderInfo) = 0;
        [[nodiscard]] virtual Texture2DID CreateTexture2D(const TextureData& loader) = 0;
        [[nodiscard]] virtual Texture2DArrayID CreateTexture2DArray(size_t width, size_t height, uint8_t channels,
            const std::span<TextureData*>& textureData) = 0;
        [[nodiscard]] virtual RenderTargetID CreateRenderTarget(Window& window) = 0;

        virtual void DestroyMaterial(MaterialID id) = 0;
        virtual void DestroyMesh(MeshID id) = 0;
        virtual void DestroyScene(SceneID id) = 0;
        virtual void DestroyShader(ShaderID id) = 0;
        virtual void DestroyTexture2D(Texture2DID id) = 0;
        virtual void DestroyTexture2DArray(Texture2DArrayID id) = 0;
        virtual void DestroyRenderTarget(RenderTargetID id) = 0;

        virtual void UpdateMesh(MeshID id, std::span<MeshData::Vertex3> vertices, std::span<uint32_t> indices) = 0;
        virtual void ResizeMesh(MeshID id, size_t vertexCount, size_t indexCount) = 0;
        virtual void SetMaterialTexture(MaterialID id, Ref<Texture> texture) = 0;
        virtual void SetMaterialColor(MaterialID id, Color color) = 0;
        virtual void SetMaterialShader(MaterialID id, Ref<Shader> shader) = 0;
        virtual void SetSceneAmbientLight(SceneID id, float level) = 0;

        virtual ImageID GetTexture2DImage(Texture2DID texture) = 0;
        virtual ImageViewID GetTexture2DImageView(Texture2DID texture) = 0;
        virtual ImageID GetTexture2DArrayImage(Texture2DArrayID texture) = 0;
        virtual ImageViewID GetTexture2DArrayImageView(Texture2DArrayID texture) = 0;
    };
}

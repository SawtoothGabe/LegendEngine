#include <LE/Graphics/Dummy/DummyResources.hpp>

namespace le
{
    MaterialID DummyResources::CreateMaterial()
    {
        return {};
    }

    MeshID DummyResources::CreateMesh(std::span<MeshData::Vertex3> vertices, std::span<uint32_t> indices,
        MeshData::UpdateFrequency frequency)
    {
        return {};
    }

    MeshID DummyResources::CreateMesh(size_t initialVertexCount, size_t initialIndexCount,
        MeshData::UpdateFrequency frequency)
    {
        return {};
    }

    SceneID DummyResources::CreateScene()
    {
        return {};
    }

    ShaderID DummyResources::CreateShader(const ShaderInfo& shaderInfo)
    {
        return {};
    }

    Texture2DID DummyResources::CreateTexture2D(const TextureData& loader)
    {
        return {};
    }

    Texture2DArrayID DummyResources::CreateTexture2DArray(size_t width, size_t height, uint8_t channels,
        const std::span<TextureData*>& textureData)
    {
        return {};
    }

    RenderTargetID DummyResources::CreateRenderTarget(Window& window)
    {
        return {};
    }

    void DummyResources::DestroyMaterial(MaterialID id) {}
    void DummyResources::DestroyMesh(MeshID id) {}
    void DummyResources::DestroyScene(SceneID id) {}
    void DummyResources::DestroyShader(ShaderID id) {}
    void DummyResources::DestroyTexture2D(Texture2DID id) {}
    void DummyResources::DestroyTexture2DArray(Texture2DArrayID id) {}
    void DummyResources::DestroyRenderTarget(RenderTargetID id) {}
    void DummyResources::UpdateMesh(MeshID id, std::span<MeshData::Vertex3> vertices, std::span<uint32_t> indices) {}
    void DummyResources::ResizeMesh(MeshID id, size_t vertexCount, size_t indexCount) {}
    void DummyResources::SetMaterialTexture(MaterialID id, Ref<Texture> texture) {}
    void DummyResources::SetMaterialColor(MaterialID id, Color color) {}
    void DummyResources::SetMaterialShader(MaterialID id, Ref<Shader> shader) {}
    void DummyResources::SetSceneAmbientLight(SceneID id, float level) {}

    ImageID DummyResources::GetTexture2DImage(Texture2DID texture)
    {
        return {};
    }

    ImageViewID DummyResources::GetTexture2DImageView(Texture2DID texture)
    {
        return {};
    }

    ImageID DummyResources::GetTexture2DArrayImage(Texture2DArrayID texture)
    {
        return {};
    }

    ImageViewID DummyResources::GetTexture2DArrayImageView(Texture2DArrayID texture)
    {
        return {};
    }
}

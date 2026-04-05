#pragma once

#include <LE/Graphics/RenderTarget.hpp>
#include <LE/Graphics/Types.hpp>
#include <LE/Resources/Material.hpp>
#include <LE/Resources/MeshData.hpp>
#include <LE/Resources/Texture2D.hpp>
#include <LE/Resources/Texture2DArray.hpp>
#include <LE/World/Scene.hpp>

namespace le
{
    class Renderer
    {
    public:
        virtual ~Renderer() = default;

        virtual Ref<Material> CreateMaterial() = 0;
        virtual Ref<MeshData> CreateMeshData() = 0;
        virtual Ref<Shader> CreateShader() = 0;
        virtual Ref<Texture2D> CreateTexture2D() = 0;
        virtual Ref<Texture2DArray> CreateTexture2DArray() = 0;
        virtual Ref<RenderTarget> CreateRenderTarget() = 0;

        virtual void StartFrame() = 0;
        virtual void RenderFrame(RenderTargetID& target, std::span<Scene*> scenes) = 0;
        virtual void EndFrame() = 0;
    };
}

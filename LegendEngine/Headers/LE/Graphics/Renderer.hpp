#pragma once

#include <LE/Graphics/Types.hpp>
#include <LE/World/Scene.hpp>

namespace le
{
    class Renderer
    {
    public:
        virtual ~Renderer() = default;

        virtual MaterialID CreateMaterial() = 0;
        virtual MeshID CreateMesh() = 0;
        virtual ShaderID CreateShader() = 0;
        virtual Texture2DID CreateTexture2D() = 0;
        virtual Texture2DArrayID CreateTexture2DArray() = 0;
        virtual RenderTargetID CreateRenderTarget() = 0;

        virtual void DestroyMaterial(MaterialID id) = 0;
        virtual void DestroyMesh(MeshID id) = 0;
        virtual void DestroyShader(ShaderID id) = 0;
        virtual void DestroyTexture2D(Texture2DID id) = 0;
        virtual void DestroyTexture2DArray(Texture2DArrayID id) = 0;
        virtual void DestroyRenderTarget(RenderTargetID id) = 0;

        virtual void StartFrame() = 0;
        virtual void RenderFrame(RenderTargetID& target, std::span<Scene*> scenes) = 0;
        virtual void EndFrame() = 0;
    };
}

#pragma once

#include <LE/Graphics/Types.hpp>
#include <LE/Graphics/API/RenderTarget.hpp>
#include <LE/World/Scene.hpp>

namespace le
{
    class Renderer
    {
    public:
        virtual ~Renderer() = default;

        virtual MaterialHandle CreateMaterial() = 0;
        virtual MeshHandle CreateMesh() = 0;
        virtual ShaderHandle CreateShader() = 0;
        virtual Texture2DHandle CreateTexture2D() = 0;
        virtual Texture2DArrayHandle CreateTexture2DArray() = 0;
        virtual RenderTargetHandle CreateRenderTarget() = 0;

        virtual void StartFrame() = 0;
        virtual void RenderFrame(RenderTarget& target, std::span<Scene*> scenes) = 0;
        virtual void EndFrame() = 0;
    };
}

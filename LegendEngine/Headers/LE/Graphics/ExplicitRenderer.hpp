#pragma once

#include <LE/Graphics/Renderer.hpp>

namespace le
{
    class ExplicitRenderer : public Renderer
    {
    public:
        MaterialHandle CreateMaterial() override;
        MeshHandle CreateMesh() override;
        ShaderHandle CreateShader() override;
        Texture2DHandle CreateTexture2D() override;
        Texture2DArrayHandle CreateTexture2DArray() override;
        RenderTargetHandle CreateRenderTarget() override;

        void StartFrame() override;
        void RenderFrame(RenderTarget& target, std::span<Scene*> scenes) override;
        void EndFrame() override;
    private:

    };
}

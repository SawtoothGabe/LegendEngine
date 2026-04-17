#pragma once
#include <LE/Graphics/Renderer.hpp>

namespace le
{
    class DummyRenderer : public Renderer
    {
    public:
        void StartFrame() override;
        void RenderFrame(RenderTarget& target, std::span<Scene*> scenes) override;
        void EndFrame() override;
    };
}

#include <LE/Graphics/Dummy/DummyRenderer.hpp>

namespace le
{
    void DummyRenderer::StartFrame() {}
    void DummyRenderer::RenderFrame(RenderTargetID& target, std::span<Scene*> scenes) {}
    void DummyRenderer::EndFrame() {}
}

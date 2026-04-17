#include <LE/Graphics/Dummy/DummyRenderer.hpp>

namespace le
{
    void DummyRenderer::StartFrame() {}
    void DummyRenderer::RenderFrame(RenderTarget& target, std::span<Scene*> scenes) {}
    void DummyRenderer::EndFrame() {}
}

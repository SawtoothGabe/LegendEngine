#pragma once
#include <LE/Graphics/GraphicsDriver.hpp>

namespace le
{
    class DummyGraphicsDriver : public GraphicsDriver
    {
    public:
        [[nodiscard]] Scope<Renderer> CreateRenderer(GraphicsResources& resources) override;
        [[nodiscard]] Scope<GraphicsResources> CreateResources() override;
    };
}

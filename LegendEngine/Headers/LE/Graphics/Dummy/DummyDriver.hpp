#pragma once
#include <LE/Graphics/GraphicsDriver.hpp>

namespace le
{
    class DummyDriver : public GraphicsDriver
    {
    public:
        [[nodiscard]] Scope<GraphicsResources> CreateResources() override;
        [[nodiscard]] Scope<Renderer> CreateRenderer(GraphicsResources& resources) override;

        [[nodiscard]] SamplerID CreateSampler(const SamplerInfo& info) override;

        void DestroySampler(SamplerID sampler) override;
    };
}

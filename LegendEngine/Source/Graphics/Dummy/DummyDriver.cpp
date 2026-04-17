#include <LE/Graphics/Dummy/DummyDriver.hpp>
#include <LE/Graphics/Dummy/DummyRenderer.hpp>
#include <LE/Graphics/Dummy/DummyResources.hpp>

namespace le
{
    Scope<GraphicsDriver> CreateDummyGraphicsDriver()
    {
        return std::make_unique<DummyDriver>();
    }

    Scope<Renderer> DummyDriver::CreateRenderer(GraphicsResources&)
    {
        return std::make_unique<DummyRenderer>();
    }

    Scope<GraphicsResources> DummyDriver::CreateResources(EventBus&)
    {
        return std::make_unique<DummyResources>();
    }

    SamplerID DummyDriver::CreateSampler(const SamplerInfo& info)
    {
        return {};
    }

    void DummyDriver::DestroySampler(SamplerID sampler) {}
}

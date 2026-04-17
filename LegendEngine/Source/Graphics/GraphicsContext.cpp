#include <LE/Graphics/GraphicsContext.hpp>

namespace le
{
    GraphicsContext::GraphicsContext(Scope<GraphicsDriver> driver)
        :
        m_driver(std::move(driver))
    {
        m_resources = m_driver->CreateResources();
        m_renderer = m_driver->CreateRenderer(*m_resources);
    }

    GraphicsContext::~GraphicsContext()
    {
        m_driver->DestroySampler(m_albedoSampler);
    }

    Renderer& GraphicsContext::GetRenderer() const
    {
        return *m_renderer;
    }

    GraphicsResources& GraphicsContext::GetResources() const
    {
        return *m_resources;
    }

    ShaderManager& GraphicsContext::GetShaderManager()
    {
        return m_shaderManager;
    }

    SamplerID GraphicsContext::GetAlbedoSampler() const
    {
        return m_albedoSampler;
    }

    void GraphicsContext::CreateSamplers()
    {
        m_albedoSampler = m_driver->CreateSampler({});
    }
}

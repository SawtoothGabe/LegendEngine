#include <LE/Graphics/GraphicsContext.hpp>

namespace le
{
    GraphicsContext::GraphicsContext(Scope<GraphicsDriver> driver)
        :
        m_driver(std::move(driver))
    {
        m_gfxPool = m_driver->CreateCommandPool(QueueFamily::GRAPHICS);
        m_renderer = m_driver->CreateRenderer(m_gfxPool);
    }

    GraphicsContext::~GraphicsContext()
    {
        m_driver->DestroyCommandPool(m_gfxPool);
    }

    GraphicsDriver& GraphicsContext::GetDriver() const
    {
        return *m_driver;
    }

    Renderer& GraphicsContext::GetRenderer() const
    {
        return *m_renderer;
    }

    ShaderManager& GraphicsContext::GetShaderManager()
    {
        return m_shaderManager;
    }
}

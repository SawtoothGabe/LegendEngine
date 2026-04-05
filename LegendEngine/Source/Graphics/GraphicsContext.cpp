#include <LE/Graphics/GraphicsContext.hpp>

namespace le
{
    GraphicsContext::GraphicsContext(Scope<GraphicsDriver> driver)
        :
        m_driver(std::move(driver))
    {
        m_renderer = m_driver->CreateRenderer();
    }

    GraphicsContext::~GraphicsContext()
    {

    }

    GraphicsDriver& GraphicsContext::GetDriver() const
    {
        return *m_driver;
    }

    Renderer& GraphicsContext::GetRenderer() const
    {
        return *m_renderer;
    }
}

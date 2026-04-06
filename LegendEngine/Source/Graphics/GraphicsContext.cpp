#include <LE/Graphics/GraphicsContext.hpp>

namespace le
{
    GraphicsContext::GraphicsContext(Scope<Renderer> renderer)
        :
        m_renderer(std::move(renderer))
    {
    }

    GraphicsContext::~GraphicsContext()
    {

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

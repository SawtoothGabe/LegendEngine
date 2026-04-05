#pragma once

#include <LE/Common/Defs.hpp>
#include <LE/Common/Types.hpp>
#include <LE/Graphics/Renderer.hpp>
#include <LE/Graphics/GraphicsDriver.hpp>
#include <LE/Graphics/ShaderManager.hpp>

namespace le
{
    class GraphicsContext final
    {
    public:
        explicit GraphicsContext(Scope<GraphicsDriver> driver);
        ~GraphicsContext();
        LE_NO_COPY(GraphicsContext);

        [[nodiscard]] GraphicsDriver& GetDriver() const;
        [[nodiscard]] Renderer& GetRenderer() const;
        [[nodiscard]] ShaderManager& GetShaderManager();
    private:
        Scope<GraphicsDriver> m_driver;
        Scope<Renderer> m_renderer;

        ShaderManager m_shaderManager;

        CommandPoolID m_gfxPool;
    };
}

#pragma once

#include <LE/Common/Defs.hpp>
#include <LE/Common/Types.hpp>
#include <LE/Graphics/Renderer.hpp>
#include <LE/Graphics/GraphicsDriver.hpp>

namespace le
{
    class GraphicsContext
    {
    public:
        explicit GraphicsContext(Scope<GraphicsDriver> driver);
        ~GraphicsContext();
        LE_NO_COPY(GraphicsContext);

        [[nodiscard]] GraphicsDriver& GetDriver() const;
        [[nodiscard]] Renderer& GetRenderer() const;
    private:
        Scope<GraphicsDriver> m_driver;
        Scope<Renderer> m_renderer;
    };
}

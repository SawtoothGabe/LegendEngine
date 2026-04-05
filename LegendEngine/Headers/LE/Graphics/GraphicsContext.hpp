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
        GraphicsContext();
        ~GraphicsContext();
        LE_NO_COPY(GraphicsContext);

        GraphicsDriver& GetDriver();
        Renderer& GetBackend();
    private:
        Scope<GraphicsDriver> m_driver;
        Scope<Renderer> m_backend;
    };
}

#pragma once

#include <LE/Common/Defs.hpp>
#include <LE/Common/Types.hpp>
#include <LE/Graphics/GraphicsBackend.hpp>
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
        GraphicsBackend& GetBackend();
    private:
        Scope<GraphicsDriver> m_driver;
        Scope<GraphicsBackend> m_backend;
    };
}

#pragma once

#include <LE/Graphics/GraphicsDriver.hpp>
#include <LE/Graphics/Types.hpp>

namespace le
{
    class RenderTarget final
    {
    public:
        explicit RenderTarget(GraphicsDriver& driver);
        ~RenderTarget();
    private:
        GraphicsDriver& m_driver;

        SurfaceID m_surface;
    };
}

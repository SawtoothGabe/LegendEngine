#pragma once

#include <LE/Graphics/GraphicsDriver.hpp>
#include <LE/Graphics/RenderTarget.hpp>
#include <LE/Graphics/Types.hpp>

namespace le
{
    class ExplicitRenderTarget final : public RenderTarget
    {
    public:
        explicit ExplicitRenderTarget(GraphicsDriver& driver);
        ~ExplicitRenderTarget();
    private:
        GraphicsDriver& m_driver;

        SurfaceID m_surface;
    };
}

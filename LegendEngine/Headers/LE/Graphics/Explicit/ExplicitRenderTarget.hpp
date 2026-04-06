#pragma once

#include <LE/Graphics/Explicit/ExplicitDriver.hpp>
#include <LE/Graphics/RenderTarget.hpp>
#include <LE/Graphics/Types.hpp>

namespace le
{
    class ExplicitRenderTarget final : public RenderTarget
    {
    public:
        explicit ExplicitRenderTarget(ExplicitDriver& driver, Window& window);
        ~ExplicitRenderTarget();
    private:
        ExplicitDriver& m_driver;

        SurfaceID m_surface;
    };
}

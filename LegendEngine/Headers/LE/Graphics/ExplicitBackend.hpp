#pragma once

#include <LE/Graphics/GraphicsBackend.hpp>

namespace le
{
    class ExplicitBackend : public GraphicsBackend
    {
    public:
        Handle CreateMaterial() override;
        Handle CreateMeshData() override;
        Handle CreateShader() override;
        Handle CreateTexture2D() override;
        Handle CreateTexture2DArray() override;
    private:

    };
}

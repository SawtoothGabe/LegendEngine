#pragma once
#include <LE/Graphics/Types.hpp>

namespace le
{
    class GraphicsBackend
    {
    public:
        virtual ~GraphicsBackend() = default;

        virtual Handle CreateMaterial() = 0;
        virtual Handle CreateMeshData() = 0;
        virtual Handle CreateShader() = 0;
        virtual Handle CreateTexture2D() = 0;
        virtual Handle CreateTexture2DArray() = 0;
    };
}

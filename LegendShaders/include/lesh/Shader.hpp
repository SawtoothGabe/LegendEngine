#pragma once

#include <cstdint>

namespace le::sh
{
    enum class ShaderStage
    {
        NONE,
        VERTEX,
        HULL,
        DOMAIN,
        GEOMETRY,
        FRAGMENT,
        COMPUTE,
        RAY_GENERATION,
        INTERSECTION,
        ANY_HIT,
        CLOSEST_HIT,
        MISS,
        CALLABLE,
        MESH,
        AMPLIFICATION,
        DISPATCH,
        COUNT
    };

    enum class Features
    {
        TEXTURED = 1 << 0
    };

    struct Entrypoint
    {
        ShaderStage stage = ShaderStage::VERTEX;
        const char* pName = nullptr;
        size_t dxilCodeSize = 0;
        uint8_t* pDxilCode = nullptr;
    };

    struct ShaderInfo
    {
        uint64_t features = 0;
        size_t entrypointCount = 0;
        Entrypoint* pEntrypoints = nullptr;
        size_t spirvCodeSize = 0;
        uint8_t* pSpirvCode = nullptr;

        // null terminated
        const char* pGlslCode = nullptr;
        const char* pWgslCode = nullptr;
    };
}

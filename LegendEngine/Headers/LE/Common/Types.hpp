#pragma once

#include <memory>

namespace le
{
    template<typename T>
    using Ref = std::shared_ptr<T>;

    template<typename T>
    using Scope = std::unique_ptr<T>;

    enum class GraphicsAPI
    {
        VULKAN,
        OPENGL,
        D3D11,
        D3D12,
        WEBGPU,
    };

    enum class ShaderType
    {
        VERTEX,
        GEOM, // ! CURRENTLY UNSUPPORTED
        COMPUTE, // ! CURRENTLY UNSUPPORTED
        FRAG,
    };
}
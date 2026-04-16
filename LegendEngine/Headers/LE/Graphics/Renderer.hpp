#pragma once

#include <LE/TetherBindings.hpp>
#include <LE/Graphics/Types.hpp>
#include <LE/IO/TextureData.hpp>
#include <LE/Resources/MeshData.hpp>

namespace le
{
    class Scene;
    class Renderer
    {
    public:
        virtual ~Renderer() = default;

        virtual void StartFrame() = 0;
        virtual void RenderFrame(RenderTargetID& target, std::span<Scene*> scenes) = 0;
        virtual void EndFrame() = 0;
    };
}

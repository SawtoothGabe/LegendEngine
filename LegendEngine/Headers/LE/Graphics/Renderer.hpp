#pragma once

#include <LE/Graphics/RenderTarget.hpp>
#include <LE/Resources/MeshData.hpp>

namespace le
{
    class Scene;
    class Renderer
    {
    public:
        virtual ~Renderer() = default;

        virtual void StartFrame() = 0;
        virtual void RenderFrame(RenderTarget& target, std::span<Scene*> scenes) = 0;
        virtual void EndFrame() = 0;
    };
}

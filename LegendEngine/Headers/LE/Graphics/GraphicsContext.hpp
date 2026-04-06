#pragma once

#include <LE/Common/Defs.hpp>
#include <LE/Common/Types.hpp>
#include <LE/Graphics/Renderer.hpp>
#include <LE/Graphics/ShaderManager.hpp>

namespace le
{
    class GraphicsContext final
    {
    public:
        explicit GraphicsContext(Scope<Renderer> renderer);
        ~GraphicsContext();
        LE_NO_COPY(GraphicsContext);

        [[nodiscard]] Renderer& GetRenderer() const;
        [[nodiscard]] ShaderManager& GetShaderManager();
    private:
        Scope<Renderer> m_renderer;

        ShaderManager m_shaderManager;

        CommandPoolID m_gfxPool;
    };
}

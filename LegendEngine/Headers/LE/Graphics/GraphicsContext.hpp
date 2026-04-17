#pragma once

#include <LE/Common/Defs.hpp>
#include <LE/Common/Types.hpp>
#include <LE/Graphics/Renderer.hpp>
#include <LE/Graphics/GraphicsResources.hpp>
#include <LE/Graphics/GraphicsDriver.hpp>
#include <LE/Graphics/ShaderManager.hpp>

namespace le
{
    class GraphicsContext final
    {
    public:
        explicit GraphicsContext(Scope<GraphicsDriver> driver);
        ~GraphicsContext();
        LE_NO_COPY(GraphicsContext);

        [[nodiscard]] Renderer& GetRenderer() const;
        [[nodiscard]] GraphicsResources& GetResources() const;
        [[nodiscard]] ShaderManager& GetShaderManager();

        [[nodiscard]] SamplerID GetAlbedoSampler() const;
    private:
        void CreateSamplers();

        Scope<GraphicsDriver> m_driver;
        Scope<Renderer> m_renderer;
        Scope<GraphicsResources> m_resources;

        ShaderManager m_shaderManager;

        SamplerID m_albedoSampler;
    };
}

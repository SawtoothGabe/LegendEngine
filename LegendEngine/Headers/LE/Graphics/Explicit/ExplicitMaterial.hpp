#pragma once

#include <LE/Graphics/Explicit/ExplicitRenderer.hpp>
#include <LE/Resources/Material.hpp>

namespace le
{
    class ExplicitMaterial : public Material
    {
    public:
        explicit ExplicitMaterial(ExplicitRenderer& renderer);
        ~ExplicitMaterial() override;
    private:
        ExplicitRenderer& m_renderer;
        GraphicsDriver& m_driver;

        std::vector<BufferID> m_uniformBuffers;
        std::vector<DescriptorSetID> m_sets;
    };
}
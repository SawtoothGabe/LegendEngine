#pragma once

#include <LE/Graphics/Explicit/ExplicitRenderer.hpp>
#include <LE/Graphics/Explicit/PerFrameBuffer.hpp>
#include <LE/Resources/Material.hpp>

namespace le
{
    class ExplicitMaterial final
    {
    public:
        explicit ExplicitMaterial(ExplicitRenderer& renderer);
        ~ExplicitMaterial();

        void UpdateUniforms(size_t frame);
        [[nodiscard]] DescriptorSetID GetSet(size_t frame) const;
    private:
        ExplicitRenderer& m_renderer;
        ExplicitDriver& m_driver;

        DescriptorPoolID m_descriptorPool;

        PerFrameBuffer m_uniformBuffer;
        std::vector<DescriptorSetID> m_sets;
    };
}
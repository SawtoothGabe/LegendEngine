#pragma once

#include <LE/Graphics/Explicit/ExplicitRenderer.hpp>
#include <LE/Graphics/Explicit/PerFrameBuffer.hpp>

namespace le
{
    class ExplicitMaterial final
    {
    public:
        explicit ExplicitMaterial(ExplicitRenderer& renderer);
        ~ExplicitMaterial();

        void UpdateUniforms(size_t frame);
        [[nodiscard]] DescriptorSetID GetSet(size_t frame) const;

        void SetTexture(const Ref<Texture>& texture);
        void SetColor(Color color);
        void SetShader(const ShaderID& shader);

        [[nodiscard]] ShaderID GetShader() const;
    private:
        Material::Uniforms m_uniforms;
        bool m_shouldUpdate = true;

        ExplicitRenderer& m_renderer;
        ExplicitDriver& m_driver;

        DescriptorPoolID m_descriptorPool;

        PerFrameBuffer m_uniformBuffer;
        std::vector<DescriptorSetID> m_sets;

        Ref<Texture> m_texture;
        ShaderID m_customShader;
    };
}
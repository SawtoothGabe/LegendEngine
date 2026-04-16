#pragma once

#include <LE/Graphics/Explicit/ExplicitResources.hpp>
#include <LE/Graphics/Explicit/PerFrameBuffer.hpp>

namespace le
{
    class ExplicitMaterial final
    {
    public:
        explicit ExplicitMaterial(ExplicitResources& resources);
        ~ExplicitMaterial();

        void UpdateUniforms(size_t frame);
        [[nodiscard]] DescriptorSetID GetSet(size_t frame) const;

        void SetTexture(const Ref<Texture>& texture);
        void SetColor(Color color);
        void SetShader(const ShaderID& shader);

        ShaderID GetShader() const;
        private:
        Material::Uniforms m_uniforms;
        bool m_shouldUpdate = true;

        ExplicitResources& m_resources;
        ExplicitDriver& m_driver;

        DescriptorPoolID m_descriptorPool;


        PerFrameBuffer m_uniformBuffer;
        std::vector<DescriptorSetID> m_sets;

        Ref<Texture> m_texture;
        ShaderID m_customShader;
    };
}
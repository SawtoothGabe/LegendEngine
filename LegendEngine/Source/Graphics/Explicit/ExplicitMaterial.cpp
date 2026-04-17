#include <LE/Application.hpp>
#include <LE/Graphics/Explicit/ExplicitMaterial.hpp>
#include <LE/Resources/Material.hpp>

namespace le
{
    ExplicitMaterial::ExplicitMaterial(ExplicitResources& resources)
        :
        m_resources(resources),
        m_driver(resources.GetDriver()),
        m_uniformBuffer(resources, BufferUsageFlagBits::UNIFORM_BUFFER, sizeof(Material::Uniforms), true)
    {
        m_sets = m_driver.AllocateDescriptorSets(
            resources.GetMaterialPoolManager(),
            m_descriptorPool,
            Application::FRAMES_IN_FLIGHT
        );

        m_framesUntilValid = Application::FRAMES_IN_FLIGHT;
    }

    ExplicitMaterial::~ExplicitMaterial()
    {
        m_driver.FreeDescriptorSets(
            m_resources.GetMaterialPoolManager(),
            m_descriptorPool,
            m_sets.size(),
            m_sets.data()
        );
    }

    void ExplicitMaterial::UpdateUniforms(const size_t frame)
    {
        if (m_framesUntilValid)
        {
            DescriptorBufferInfo bufferInfo {
                .buffer = m_uniformBuffer.GetDesc(frame).buffer,
                .range = sizeof(m_uniforms),
            };

            DescriptorImageInfo imageInfo{};

            size_t writeCount = 1;

            WriteDescriptorSet writes[2]{};
            writes[0].dstSet = m_sets[frame];
            writes[0].pBufferInfo = &bufferInfo;

            if (m_texture)
            {
                writeCount++;

                imageInfo.sampler = m_texture->GetSampler();
                imageInfo.imageView = m_texture->GetImageView();
                imageInfo.imageLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL;

                writes[1].dstSet = m_sets[frame],
                writes[1].dstBinding = 1,
                writes[1].descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER,
                writes[1].pImageInfo = &imageInfo;
            }

            m_driver.UpdateDescriptorSets(std::span(writes, writeCount));

            m_framesUntilValid--;
        }

        m_uniformBuffer.Update(sizeof(m_uniforms), 0, &m_uniforms, frame);
    }

    DescriptorSetID ExplicitMaterial::GetSet(const size_t frame) const
    {
        return m_sets[frame];
    }

    void ExplicitMaterial::SetTexture(const Ref<Texture>& texture)
    {
        m_texture = texture;
        m_framesUntilValid = Application::FRAMES_IN_FLIGHT;
    }

    void ExplicitMaterial::SetColor(const Color color)
    {
        m_uniforms.color = color;
    }
}

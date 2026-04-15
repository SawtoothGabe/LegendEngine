#include <LE/Application.hpp>
#include <LE/Graphics/Explicit/ExplicitMaterial.hpp>
#include <LE/Resources/Material.hpp>

namespace le
{
    ExplicitMaterial::ExplicitMaterial(ExplicitRenderer& renderer)
        :
        m_renderer(renderer),
        m_driver(renderer.GetDriver()),
        m_uniformBuffer(renderer, BufferUsageFlagBits::UNIFORM_BUFFER, sizeof(Material::Uniforms))
    {
        m_sets = m_driver.AllocateDescriptorSets(
            renderer.GetMaterialPoolManager(),
            m_descriptorPool,
            Application::FRAMES_IN_FLIGHT
        );
    }

    ExplicitMaterial::~ExplicitMaterial()
    {
        m_driver.FreeDescriptorSets(
            m_renderer.GetMaterialPoolManager(),
            m_descriptorPool,
            m_sets.size(),
            m_sets.data()
        );
    }

    void ExplicitMaterial::UpdateUniforms(size_t frame)
    {

    }

    DescriptorSetID ExplicitMaterial::GetSet(const size_t frame) const
    {
        return m_sets[frame];
    }
}

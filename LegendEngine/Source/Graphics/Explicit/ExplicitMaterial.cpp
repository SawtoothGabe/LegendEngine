#include <LE/Application.hpp>
#include <LE/Graphics/Explicit/ExplicitMaterial.hpp>
#include <LE/Resources/Material.hpp>

namespace le
{
    ExplicitMaterial::ExplicitMaterial(ExplicitRenderer& renderer)
        :
        m_renderer(renderer),
        m_driver(renderer.GetDriver())
    {
        m_uniformBuffers.resize(Application::FRAMES_IN_FLIGHT);
        m_sets = m_driver.AllocateDescriptorSets(
            renderer.GetMaterialPoolManager(),
            m_descriptorPool,
            Application::FRAMES_IN_FLIGHT
        );

        for (auto & m_uniformBuffer : m_uniformBuffers)
            m_uniformBuffer = m_driver.CreateBuffer(BufferUsageFlagBits::UNIFORM_BUFFER,
                sizeof(Uniforms), true);
    }

    ExplicitMaterial::~ExplicitMaterial()
    {
        m_driver.FreeDescriptorSets(
            m_renderer.GetMaterialPoolManager(),
            m_descriptorPool,
            m_sets.size(),
            m_sets.data()
        );

        for (const auto & m_uniformBuffer : m_uniformBuffers)
            m_driver.DestroyBuffer(m_uniformBuffer);
    }

    DescriptorSetID ExplicitMaterial::GetSet(const size_t frame) const
    {
        return m_sets[frame];
    }
}

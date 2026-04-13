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

        for (size_t i = 0; i < m_uniformBuffers.size(); ++i)
            m_uniformBuffers[i] = m_driver.CreateBuffer(BufferUsageFlagBits::UNIFORM_BUFFER,
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

        for (size_t i = 0; i < m_uniformBuffers.size(); ++i)
            m_driver.DestroyBuffer(m_uniformBuffers[i]);
    }
}

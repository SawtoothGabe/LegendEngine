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
        m_sets.resize(Application::FRAMES_IN_FLIGHT);

        m_driver.AllocateDescriptorSets();
        for (size_t i = 0; i < m_uniformBuffers.size(); ++i)
            m_uniformBuffers[i] = m_driver.CreateBuffer(BufferUsageFlags::UNIFORM_BUFFER,
                sizeof(Uniforms), true);
    }

    ExplicitMaterial::~ExplicitMaterial()
    {
        m_driver.FreeDescriptorSets();
        for (size_t i = 0; i < m_uniformBuffers.size(); ++i)
            m_driver.DestroyBuffer(m_uniformBuffers[i]);
    }
}

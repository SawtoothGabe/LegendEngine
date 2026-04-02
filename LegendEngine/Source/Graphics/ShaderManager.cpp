#include <LE/Common/Assert.hpp>
#include <LE/Graphics/ShaderManager.hpp>

namespace le
{
    ShaderManager::~ShaderManager()
    {}

    Resource::ID<Shader> ShaderManager::GetByID(const std::string_view shaderID) const
    {
        LE_ASSERT(m_shaders.contains(shaderID.data()), "Invalid shader ID. Was it registered?");
        return m_shaders.at(shaderID.data());
    }

    void ShaderManager::RegisterShader(const std::string_view id, const Resource::ID<Shader> shader)
    {
        LE_ASSERT(!m_shaders.contains(id.data()), "Shader registered twice");
        m_shaders[id.data()] = shader;
    }
}

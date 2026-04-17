#include <LE/Common/Assert.hpp>
#include <LE/Graphics/ShaderManager.hpp>

namespace le
{
    Ref<Shader> ShaderManager::TryCreate(const ShaderInfo* pInfo)
    {
        if (m_shaders.contains(pInfo->name))
            return m_shaders.at(pInfo->name);

        return m_shaders[pInfo->name] = Shader::Create(*pInfo);
    }

    Ref<Shader> ShaderManager::FromID(const std::string_view id)
    {
        return m_shaders.at(id.data());
    }
}

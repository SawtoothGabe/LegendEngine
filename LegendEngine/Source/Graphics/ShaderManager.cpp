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

    Ref<Shader> ShaderManager::TryCreateFromId(const std::string_view id)
    {
        if (!m_shaders.contains(id.data()))
        {
            ShaderRegistry& reg = ShaderRegistry::Get();
            const ShaderInfo* pInfo = reg.GetShader(id.data());
            LE_ASSERT(pInfo, "No shader found with ID {}", id);

            // ReSharper disable once CppDFANullDereference
            return m_shaders[pInfo->name] = Shader::Create(*pInfo);
        }

        return m_shaders.at(id.data());
    }
}

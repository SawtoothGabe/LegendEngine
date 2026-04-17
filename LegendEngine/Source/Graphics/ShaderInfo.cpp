#include <LE/Common/Assert.hpp>
#include <LE/Graphics/ShaderInfo.hpp>

namespace le
{
    void ShaderRegistry::Register(ShaderInfo* pInfo)
    {
        LE_ASSERT(!m_shaders.contains(pInfo->name), "Shader with ID already exists");

        m_shaders[pInfo->name] = pInfo;

        if (pInfo->features == 0)
            return;

        m_featuredShaders.try_emplace(pInfo->features, pInfo);
    }

    ShaderInfo* ShaderRegistry::GetShader(const std::string_view name)
    {
        return m_shaders.contains(name.data()) ? m_shaders[name.data()] : nullptr;
    }

    ShaderInfo* ShaderRegistry::FromFeatures(const uint64_t features)
    {
        return m_featuredShaders.contains(features) ? m_featuredShaders[features] : nullptr;
    }

    ShaderRegistry& ShaderRegistry::Get()
    {
        static ShaderRegistry registry;
        return registry;
    }
}

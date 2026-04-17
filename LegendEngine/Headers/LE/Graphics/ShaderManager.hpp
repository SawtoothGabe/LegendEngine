#pragma once

#include <unordered_map>
#include <LE/Common/Defs.hpp>
#include <LE/Resources/Shader.hpp>

namespace le
{
    class ShaderManager final
    {
    public:
        ShaderManager() = default;
        LE_NO_COPY(ShaderManager);

        Ref<Shader> TryCreate(const ShaderInfo* pInfo);
        Ref<Shader> TryCreateFromId(std::string_view id);
    private:
        std::unordered_map<std::string, Ref<Shader>> m_shaders;
    };
}
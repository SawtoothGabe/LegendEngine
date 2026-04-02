#pragma once

#include <string_view>
#include <unordered_map>
#include <LE/Common/Defs.hpp>
#include <LE/Resources/Shader.hpp>

namespace le
{
    class ShaderManager final
    {
    public:
        ShaderManager() = default;
        ~ShaderManager();
        LE_NO_COPY(ShaderManager);

        Resource::ID<Shader> GetByID(std::string_view shaderID) const;

        // The shader object MUST exist for the life of the base class
        void RegisterShader(std::string_view id, Resource::ID<Shader> shader);
    private:
        std::unordered_map<std::string, Resource::ID<Shader>> m_shaders;
    };
}
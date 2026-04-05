#pragma once

#include <span>
#include <LE/Common/Types.hpp>
#include <LE/Resources/Resource.hpp>

namespace le
{
    class Shader : public Resource
    {
    public:
        enum class CullMode
        {
            NONE,
            BACK,
            FRONT,
        };

        struct Stage
        {
            ShaderType type = ShaderType::VERTEX;
            const uint8_t* compiledShaderCode = nullptr;
            size_t compiledShaderSize = 0;
        };

        explicit Shader(std::span<Stage> stages);

        void SetCullMode(CullMode cullMode);
        [[nodiscard]] CullMode GetCullMode() const;
    protected:
        CullMode m_CullMode = CullMode::BACK;
    };
}
#include <LE/Application.hpp>
#include <LE/Resources/Shader.hpp>

namespace le
{
    Shader::Shader(const std::span<Stage> stages)
    {
    }

    void Shader::SetCullMode(const CullMode cullMode)
    {
        m_CullMode = cullMode;
    }

    CullMode Shader::GetCullMode() const
    {
        return m_CullMode;
    }
}

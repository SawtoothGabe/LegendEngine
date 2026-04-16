#include <LE/Application.hpp>
#include <LE/Resources/Shader.hpp>

namespace le
{
    Shader::Shader(GraphicsResources& resources, const ShaderID handle, Passkey)
        :
        m_resources(resources),
        m_handle(handle)
    {}

    Shader::~Shader()
    {
        m_resources.DestroyShader(m_handle);
    }

    void Shader::SetCullMode(const CullMode cullMode)
    {
        m_cullMode = cullMode;
    }

    CullMode Shader::GetCullMode() const
    {
        return m_cullMode;
    }

    ShaderID Shader::GetHandle() const
    {
        return m_handle;
    }

    Ref<Shader> Shader::Create(const sh::ShaderInfo& info)
    {
        GraphicsResources& resources = Application::Get().GetGraphicsContext().GetResources();
        return std::make_shared<Shader>(resources, resources.CreateShader(info), Passkey{});
    }
}

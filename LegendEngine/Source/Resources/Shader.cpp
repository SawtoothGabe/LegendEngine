#include <LE/Application.hpp>
#include <LE/Resources/Shader.hpp>

namespace le
{
    Shader::Shader(const std::span<Stage> stages)
    {
        const std::span<DescriptorSetLayout*> layouts =
            Application::Get().GetGraphicsResources().GetLayouts();
        m_pipeline = Application::Get().GetGraphicsContext().CreatePipeline(stages, layouts);
    }

    void Shader::SetCullMode(const CullMode cullMode)
    {
        m_CullMode = cullMode;
    }

    Shader::CullMode Shader::GetCullMode() const
    {
        return m_CullMode;
    }

    Pipeline& Shader::GetPipeline() const
    {
        return *m_pipeline;
    }
}

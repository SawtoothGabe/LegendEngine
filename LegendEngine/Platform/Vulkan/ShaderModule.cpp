#include <ShaderModule.hpp>
#include <VkDefs.hpp>

namespace le::vk
{
    using namespace Tether::Rendering::Vulkan;

    ShaderModule::ShaderModule(GraphicsContext& context,
        const Shader::Stage& stage)
        :
        m_Context(context)
    {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = stage.compiledShaderSize;
        createInfo.pCode = reinterpret_cast<const uint32_t*>(
            stage.compiledShaderCode);

        LE_CHECK_VK(vkCreateShaderModule(m_Context.GetDevice(), &createInfo, nullptr,
            &m_ShaderModule));
    }

    ShaderModule::ShaderModule(ShaderModule&& other) noexcept
        :
        m_Context(other.m_Context),
        m_ShaderModule(other.m_ShaderModule)
    {
        other.m_Moved = true;
    }

    ShaderModule::~ShaderModule()
    {
        if (m_Moved)
            return;

        vkDestroyShaderModule(m_Context.GetDevice(), m_ShaderModule, nullptr);
    }

    VkShaderModule ShaderModule::Get() const
    {
        return m_ShaderModule;
    }
}

#pragma once

#include <LE/Resources/Shader.hpp>
#include <Tether/Rendering/Vulkan/GraphicsContext.hpp>

namespace le::vk
{
    class ShaderModule final
    {
    public:
        ShaderModule(Tether::Rendering::Vulkan::GraphicsContext& context,
            const Shader::Stage& stage);
        ShaderModule(ShaderModule&& other) noexcept;
        ShaderModule(const ShaderModule&) = delete;
        ~ShaderModule();

        VkShaderModule Get() const;
    private:
        bool m_Moved = false;

        Tether::Rendering::Vulkan::GraphicsContext& m_Context;
        VkShaderModule m_ShaderModule = nullptr;
    };
}

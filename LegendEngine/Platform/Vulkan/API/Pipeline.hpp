#pragma once

#include <LE/Common/Defs.hpp>
#include <LE/Graphics/API/DescriptorSetLayout.hpp>
#include <LE/Graphics/API/Pipeline.hpp>
#include <LE/Math/Types.hpp>
#include <Tether/Rendering/Vulkan/GraphicsContext.hpp>

#include <vulkan/vulkan.h>

namespace le::vk
{
    class Pipeline final : public le::Pipeline
    {
    public:
        struct Info
        {
            uint32_t stageCount;
            VkPipelineShaderStageCreateInfo* pStages;
            std::span<le::DescriptorSetLayout*> setLayouts;
            uint32_t dynamicStateCount;
            VkDynamicState* pDynamicStates;
            VkSurfaceFormatKHR surfaceFormat;
            VkFormat depthFormat;
        };

        struct ObjectTransform
        {
            Matrix4x4f transform;
        };

        Pipeline(Tether::Rendering::Vulkan::GraphicsContext& context,
            const Info& info);
        ~Pipeline() override;
        LE_NO_COPY(Pipeline);

        [[nodiscard]] VkPipeline Get() const;
        [[nodiscard]] VkPipelineLayout GetPipelineLayout() const;
    private:
        Tether::Rendering::Vulkan::GraphicsContext& m_Context;

        VkPipelineLayout m_PipelineLayout = nullptr;
        VkPipeline m_Pipeline = nullptr;
    };
}
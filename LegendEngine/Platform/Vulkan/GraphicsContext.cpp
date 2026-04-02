#include "GraphicsContext.hpp"

#include "API/Buffer.hpp"
#include "API/CommandBuffer.hpp"
#include "API/DescriptorSetLayout.hpp"
#include "API/DynamicUniforms.hpp"
#include "API/Image.hpp"
#include "API/Pipeline.hpp"
#include "API/Sampler.hpp"

#include "solid.frag.spv.h"
#include "solid.vert.spv.h"
#include "textured.frag.spv.h"
#include "textured.vert.spv.h"

#include <Renderer.hpp>
#include <RenderTarget.hpp>
#include <ShaderModule.hpp>
#include <VkDefs.hpp>
#include <API/PerFrameBuffer.hpp>
#include <API/SimpleBuffer.hpp>
#include <API/SmartBuffer.hpp>
#include <LE/Application.hpp>
#include <LE/Common/Assert.hpp>
#include <LE/IO/Logger.hpp>

namespace le
{
    Scope<GraphicsContext> CreateVulkanGraphicsContext(std::string_view applicationName)
    {
        return std::make_unique<vk::GraphicsContext>(applicationName);
    }
}

namespace le::vk
{
    static const char* EXTENSIONS[] =
    {
        VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
        VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME,
        VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
    };

    static VkPhysicalDeviceSynchronization2FeaturesKHR SYNCHRONIZATION_2
    {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES_KHR,
        .synchronization2 = true
    };

    static VkPhysicalDeviceTimelineSemaphoreFeaturesKHR TIMELINE_SEMAPHORE
    {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES_KHR,
        .pNext = static_cast<void*>(&SYNCHRONIZATION_2),
        .timelineSemaphore = true,
    };

    static constexpr VkPhysicalDeviceDynamicRenderingFeaturesKHR DYNAMIC_RENDERING
    {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR,
        .pNext = static_cast<void*>(&TIMELINE_SEMAPHORE),
        .dynamicRendering = true,
    };

    GraphicsContext::GraphicsContext(const std::string_view applicationName)
        :
        m_ContextCreator(GetContextInfo(applicationName)),
        m_GraphicsContext(m_ContextCreator),
        m_Callback(*this),
        m_ActualTransferMutex(FindTransferMutex())
    {
        m_ContextCreator.AddDebugMessenger(&m_Callback);

        const TetherVulkan::DeviceLoader& loader = m_GraphicsContext.GetDeviceLoader();
        LE_ASSERT(loader.vkCmdBeginRenderingKHR && loader.vkCmdEndRenderingKHR, "Couldn't load dynamic rendering funcs");

        CreateUniforms();
        CreateTransferQueue();

        m_DepthFormat = FindDepthFormat();

        VkFenceCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    }

    GraphicsContext::~GraphicsContext()
    {
        m_ContextCreator.RemoveDebugMessenger(&m_Callback);
    }

    Scope<le::Renderer> GraphicsContext::CreateRenderer(le::RenderTarget& renderTarget, GraphicsResources& resources)
    {
        constexpr VkSurfaceFormatKHR surfaceFormat =
        {
            .format = VK_FORMAT_B8G8R8A8_SRGB,
            .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
        };

        return std::make_unique<Renderer>(
            *this, renderTarget, resources, surfaceFormat
        );
    }

    Scope<le::RenderTarget> GraphicsContext::CreateHeadlessRenderTarget()
    {
        return std::make_unique<RenderTarget>(m_GraphicsContext);
    }

    GraphicsContext::DebugCallback::DebugCallback(GraphicsContext& context)
        :
        m_Context(context)
    {}

    void GraphicsContext::DebugCallback::OnDebugLog(
        const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData
    )
    {
        std::stringstream ss;
        ss << "Vulkan Validation Layer: " << pCallbackData->pMessage;

        Logger::Level level;
        switch (messageSeverity)
        {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                level = Logger::Level::INFO;
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                level = Logger::Level::WARN;
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                level = Logger::Level::ERROR;
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
                level = Logger::Level::DEBUG;
                break;
            default: level = Logger::Level::INFO;
        }

        Logger::GetGlobalLogger().Log(level, ss.str());
    }

#ifndef LE_HEADLESS
    Scope<le::RenderTarget> GraphicsContext::CreateWindowRenderTarget(Tether::Window& window)
    {
        return std::make_unique<RenderTarget>(m_GraphicsContext, window);
    }
#endif

    VkFormat GraphicsContext::GetDepthFormat() const
    {
        return m_DepthFormat;
    }

    std::mutex& GraphicsContext::GetGraphicsQueueMutex()
    {
        return m_GraphicsQueueMutex;
    }

    std::mutex& GraphicsContext::GetTransferQueueMutex() const
    {
        return m_ActualTransferMutex;
    }

    VkQueue GraphicsContext::GetTransferQueue() const
    {
        return m_TransferQueue;
    }

    VkCommandPool GraphicsContext::GetTransferPool() const
    {
        return m_TransferPool;
    }

    TetherVulkan::GraphicsContext& GraphicsContext::GetTetherGraphicsContext()
    {
        return m_GraphicsContext;
    }

    TetherVulkan::ContextCreator::Info GraphicsContext::GetContextInfo(std::string_view applicationName)
    {
        TetherVulkan::ContextCreator::Info info;
        info.deviceExtensions = EXTENSIONS;
        info.applicationName = applicationName;
        info.engineName = "LegendEngine";
        info.devicePNext = &DYNAMIC_RENDERING;
        info.deviceExtensions = EXTENSIONS;
        info.createTransferQueue = true;
        info.framesInFlight = Application::FRAMES_IN_FLIGHT;

        return info;
    }

    VkFormat GraphicsContext::FindDepthFormat() const
    {
        constexpr VkFormat candidates[] = {
            VK_FORMAT_D32_SFLOAT,
            VK_FORMAT_D32_SFLOAT_S8_UINT,
            VK_FORMAT_D24_UNORM_S8_UINT
        };

        constexpr VkFormatFeatureFlags features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;

        for (const VkFormat format : candidates)
        {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(m_GraphicsContext.GetPhysicalDevice(), format, &props);

            if ((props.optimalTilingFeatures & features) == features)
                return format;
        }

        return candidates[0];
    }

    std::mutex& GraphicsContext::FindTransferMutex()
    {
        const Tether::Rendering::Vulkan::QueueFamilyIndices indices = m_ContextCreator.GetQueueFamilyIndices();
        return indices.graphicsFamilyIndex == indices.transferFamilyIndex ? m_GraphicsQueueMutex : m_TransferQueueMutex;
    }

    void GraphicsContext::CreateUniforms()
    {
        const uint32_t framesInFlight = m_GraphicsContext.GetFramesInFlight();

        VkDescriptorPoolSize uniformsSize{};
        uniformsSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uniformsSize.descriptorCount = framesInFlight * 2;

        VkDescriptorPoolSize samplersSize{};
        samplersSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplersSize.descriptorCount = framesInFlight;

        VkDescriptorPoolSize sizes[] =
        {
            uniformsSize,
            samplersSize
        };
    }

    void GraphicsContext::CreateTransferQueue()
    {
        const Tether::Rendering::Vulkan::QueueFamilyIndices indices = m_ContextCreator.GetQueueFamilyIndices();
        if (indices.graphicsFamilyIndex == indices.transferFamilyIndex)
        {
            m_TransferQueue = m_ContextCreator.GetQueue();
            m_TransferPool = m_ContextCreator.GetCommandPool();
            return;
        }

        m_TransferQueue = m_ContextCreator.GetDeviceQueue(indices.transferFamilyIndex, 0);

        VkCommandPoolCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        info.queueFamilyIndex = indices.transferFamilyIndex;

        LE_CHECK_VK(vkCreateCommandPool(m_ContextCreator.GetDevice(), &info, nullptr, &m_TransferPool));
    }

    Scope<le::Buffer> GraphicsContext::CreateSimpleBuffer(Buffer::UsageFlags usage, size_t size, bool createMapped)
    {
        return std::make_unique<SimpleBuffer>(*this, usage, size, createMapped);
    }

    Scope<le::Buffer> GraphicsContext::CreateSmartBuffer(Buffer::UsageFlags usage)
    {
        return std::make_unique<SmartBuffer>(*this, usage);
    }

    Scope<le::Buffer> GraphicsContext::CreatePerFrameBuffer(Buffer::UsageFlags usage, size_t size)
    {
        return std::make_unique<PerFrameBuffer>(*this, usage, size);
    }

    Scope<le::CommandBuffer> GraphicsContext::CreateCommandBuffer(const bool transfer)
    {
        if (transfer)
            return std::make_unique<CommandBuffer>(*this, m_TransferPool, m_TransferQueue, m_TransferQueueMutex);

        return std::make_unique<CommandBuffer>(*this, m_GraphicsContext.GetCommandPool(), m_GraphicsContext.GetQueue(), m_GraphicsQueueMutex);
    }

    Scope<le::DescriptorSetLayout> GraphicsContext::CreateDescriptorSetLayout(
        std::span<DescriptorSetLayout::Binding> bindings)
    {
        return std::make_unique<DescriptorSetLayout>(*this, bindings);
    }

    Scope<le::DynamicUniforms> GraphicsContext::CreateDynamicUniforms(
        DynamicUniforms::UpdateFrequency frequency, le::DescriptorSetLayout& layout)
    {
        return std::make_unique<DynamicUniforms>(*this, frequency, layout);
    }

    Scope<le::Pipeline> GraphicsContext::CreatePipeline(std::span<Shader::Stage> stages, std::span<le::DescriptorSetLayout*> layouts)
    {
        std::vector<VkPipelineShaderStageCreateInfo> vkStages;
        std::vector<ShaderModule> shaderModules;
        vkStages.reserve(stages.size());
        shaderModules.reserve(stages.size());

        for (Shader::Stage stage : stages)
        {
            shaderModules.emplace_back(m_GraphicsContext, stage);

            VkPipelineShaderStageCreateInfo vkStage{};
            vkStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            vkStage.module = shaderModules.back().Get();
            vkStage.pName = "main";

            switch (stage.type)
            {
                case ShaderType::VERTEX: vkStage.stage = VK_SHADER_STAGE_VERTEX_BIT; break;
                case ShaderType::FRAG:  vkStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT; break;
                case ShaderType::COMPUTE: vkStage.stage = VK_SHADER_STAGE_COMPUTE_BIT; break;
                case ShaderType::GEOM:  vkStage.stage = VK_SHADER_STAGE_GEOMETRY_BIT; break;
            }

            vkStages.push_back(vkStage);
        }

        VkDynamicState dynamicStates[] =
        {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR,
            VK_DYNAMIC_STATE_CULL_MODE
        };

        Pipeline::Info pipelineInfo{};
        pipelineInfo.stageCount = vkStages.size();
        pipelineInfo.pStages = vkStages.data();
        pipelineInfo.pDynamicStates = dynamicStates;
        pipelineInfo.dynamicStateCount = std::size(dynamicStates);
        pipelineInfo.pDynamicStates = dynamicStates;
        pipelineInfo.setLayouts = layouts;
        pipelineInfo.depthFormat = m_DepthFormat;

        return std::make_unique<Pipeline>(m_GraphicsContext, pipelineInfo);
    }

    Scope<le::Image> GraphicsContext::CreateImage(const Image::Info& info)
    {
        return std::make_unique<Image>(*this, info);
    }

    Scope<le::Sampler> GraphicsContext::CreateSampler(const Sampler::Info& info)
    {
        return std::make_unique<Sampler>(*this, info);
    }

    void GraphicsContext::WaitIdle()
    {
        vkDeviceWaitIdle(m_GraphicsContext.GetDevice());
    }

#define STAGES(id) \
    static Shader::Stage STAGES_##id[] = { \
        { ShaderType::VERTEX, _binary_##id##_vert_spv, \
            sizeof(_binary_##id##_vert_spv) }, \
        { ShaderType::FRAG, _binary_##id##_frag_spv, \
            sizeof(_binary_##id##_frag_spv) } \
    }

    STAGES(solid);
    STAGES(textured);

    void GraphicsContext::RegisterShaders(ShaderManager& shaderManager)
    {
        ResourceManager& manager = Application::Get().GetResourceManager();

        shaderManager.RegisterShader("solid", manager.CreateResource<Shader>(std::span(STAGES_solid))->id);
        shaderManager.RegisterShader("textured", manager.CreateResource<Shader>(std::span(STAGES_textured))->id);
    }
}

#include "VulkanDriver.hpp"

#include <PoolManager.hpp>
#include <set>
#include <vk_mem_alloc.h>

#include <LE/Graphics/Explicit/ExplicitRenderer.hpp>
#include <LE/Graphics/Explicit/ExplicitResources.hpp>

#include "PlatformUtils.hpp"
#include "VkDefs.hpp"
#include "VulkanBuffer.hpp"
#include "VulkanImage.hpp"
#include "VulkanTypes.hpp"

namespace le
{
#define VULKAN_CAST(type, identifier) vk::type(reinterpret_cast<Vk##type>(identifier.id))
#define RAW_CAST(type, identifier) reinterpret_cast<Vk##type>(identifier.id)

    Scope<GraphicsDriver> CreateVulkanGraphicsDriver(std::string_view applicationName)
    {
        return std::make_unique<VulkanDriver>(applicationName);
    }

    Scope<Renderer> VulkanDriver::CreateRenderer(GraphicsResources& resources)
    {
        return std::make_unique<ExplicitRenderer>(static_cast<ExplicitResources&>(resources));
    }

    Scope<GraphicsResources> VulkanDriver::CreateResources()
    {
        return std::make_unique<ExplicitResources>(*this);
    }

    static const std::vector VALIDATION_LAYERS = {
        "VK_LAYER_KHRONOS_validation"
    };

	static const std::vector DEVICE_EXTENSIONS =
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

    static VKAPI_ATTR vk::Bool32 VKAPI_CALL OnValidationMessage(
        const vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
        vk::DebugUtilsMessageTypeFlagsEXT,
        const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData, void*)
    {
        switch (severity)
        {
            case vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose:
                LE_DEBUG("Vulkan validation layers: {}", pCallbackData->pMessage);
                break;
            case vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo:
                LE_INFO("Vulkan validation layers: {}", pCallbackData->pMessage);
                break;
            case vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning:
                LE_WARN("Vulkan validation layers: {}", pCallbackData->pMessage);
                break;
            case vk::DebugUtilsMessageSeverityFlagBitsEXT::eError:
                LE_ERROR("Vulkan validation layers: {}", pCallbackData->pMessage);
                break;
        }

        return vk::False;
    }

    VulkanDriver::VulkanDriver(const std::string_view applicationName)
    {
	    VULKAN_HPP_DEFAULT_DISPATCHER.init();

	    CreateInstance(applicationName);
	    CreateDevice();
	    CreateAllocator();
    }

    VulkanDriver::~VulkanDriver()
    {
	    vmaDestroyAllocator(m_allocator);
	    m_device.destroy();
	    m_instance.destroy();
    }

    std::vector<CommandBufferID> VulkanDriver::AllocateCommandBuffers(const CommandPoolID pool, const size_t count)
    {
	    std::vector<CommandBufferID> ids;
	    const std::vector<vk::CommandBuffer> buffers
			    = m_device.allocateCommandBuffers({VULKAN_CAST(CommandPool, pool), {}, static_cast<uint32_t>(count)});

	    ids.reserve(buffers.size());
	    for (vk::CommandBuffer buffer : buffers)
		    ids.emplace_back(buffer);

	    return ids;
    }

    CommandBufferID VulkanDriver::AllocateCommandBuffer(const CommandPoolID pool)
    {
    	vk::CommandBufferAllocateInfo info;
    	info.commandBufferCount = 1;
    	info.commandPool = VULKAN_CAST(CommandPool, pool);

    	vk::CommandBuffer buffer;
    	LE_CHECK_RESULT(m_device.allocateCommandBuffers(&info, &buffer));

    	return CommandBufferID(buffer);
    }

    std::vector<DescriptorSetID> VulkanDriver::AllocateDescriptorSets(const PoolManagerID manager, DescriptorPoolID& outPool, const size_t count)
    {
	    const auto pManager = reinterpret_cast<PoolManager*>(manager.id);
	    return pManager->Allocate(outPool, count);
    }

    BufferID VulkanDriver::CreateBuffer(BufferUsageFlags flags, const std::size_t size, const bool createMapped)
    {
	    VkBufferCreateInfo bufferInfo{};
	    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	    bufferInfo.size = size;
	    bufferInfo.usage = static_cast<uint32_t>(flags);

	    VmaAllocationCreateInfo allocInfo{};
	    allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
	    allocInfo.flags = VulkanTypes::GetVmaFlags(flags, createMapped);

	    const auto buffer = new VulkanBuffer;

	    LE_CHECK_VK(vmaCreateBuffer(m_allocator, &bufferInfo, &allocInfo,
		    &buffer->buffer, &buffer->allocation, &buffer->allocationInfo));
	    return BufferID(buffer);
    }

    CommandPoolID VulkanDriver::CreateCommandPool(const QueueFamily family)
    {
	    uint32_t index = 0;
	    switch (family)
	    {
		    case QueueFamily::GRAPHICS: index = m_indices.graphicsFamilyIndex; break;
		    case QueueFamily::COMPUTE: index = m_indices.computeFamilyIndex; break;
		    case QueueFamily::TRANSFER: index = m_indices.transferFamilyIndex; break;
	    }

	    const vk::CommandPoolCreateInfo info({}, index);
	    return CommandPoolID(m_device.createCommandPool(info, nullptr));
    }

    PoolManagerID VulkanDriver::CreateLayoutPoolManager(DescriptorSetLayoutID layout,
    	std::span<DescriptorSetLayoutBinding> bindings)
    {
	    return PoolManagerID(new PoolManager(m_device, layout, bindings, 32));
    }

    FenceID VulkanDriver::CreateFence(const bool signaled)
    {
	    vk::FenceCreateFlags flags = signaled ? vk::FenceCreateFlagBits::eSignaled : vk::FenceCreateFlags();
	    return FenceID(m_device.createFence({flags}));
    }

    ImageID VulkanDriver::CreateImage(const ImageInfo& info)
    {
	    VkImageCreateInfo imageInfo{};
	    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	    imageInfo.imageType = VK_IMAGE_TYPE_2D;
	    imageInfo.extent.width  = static_cast<uint32_t>(info.width);
	    imageInfo.extent.height = static_cast<uint32_t>(info.height);
	    imageInfo.extent.depth = 1;
	    imageInfo.mipLevels = 1;
	    imageInfo.arrayLayers = static_cast<uint32_t>(info.arrayLayers);
	    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	    imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	    imageInfo.format = VulkanTypes::GetFormat(info.format);

	    VmaAllocationCreateInfo allocInfo{};
	    allocInfo.usage = VMA_MEMORY_USAGE_AUTO;

	    const auto image = new VulkanImage;

	    LE_CHECK_VK(vmaCreateImage(m_allocator, &imageInfo, &allocInfo,
		    &image->image, &image->allocation, nullptr));

	    return ImageID(image);
    }

    ImageViewID VulkanDriver::CreateImageView(const ImageID image, const Format format, const ImageViewType type)
    {
	    vk::ImageViewType viewType = {};
	    switch (type)
	    {
		    case ImageViewType::TYPE_2D: viewType = vk::ImageViewType::e2D; break;
		    case ImageViewType::TYPE_2D_ARRAY: viewType = vk::ImageViewType::e2DArray; break;

		    default: LE_ASSERT(false, "Unknown image view type");
	    }

	    const vk::ImageView view = m_device.createImageView(
		    {{}, VULKAN_CAST(Image, image), viewType, VulkanTypes::GetVkFormat(format)});
	    return ImageViewID(view);
    }

    PipelineID VulkanDriver::CreatePipeline(const PipelineInfo& info)
    {
    	if (!info.pShaderInfo)
    		return {};

	    std::vector<vk::Format> formats;
	    std::vector<vk::PipelineShaderStageCreateInfo> stages;
	    std::vector<vk::VertexInputBindingDescription> bindings;
	    std::vector<vk::VertexInputAttributeDescription> attributes;
	    formats.reserve(info.colorAttachmentFormats.size());
	    bindings.reserve(info.vertexBindings.size());
	    attributes.reserve(info.vertexAttributes.size());

	    for (const Format format : info.colorAttachmentFormats)
		    formats.push_back(VulkanTypes::GetVkFormat(format));

    	vk::ShaderModuleCreateInfo moduleInfo(
    		{}, info.pShaderInfo->spirvCodeSize, reinterpret_cast<uint32_t*>(info.pShaderInfo->pSpirvCode)
    	);

    	vk::ShaderModule module = m_device.createShaderModule(moduleInfo);

	    for (size_t i = 0; i < info.pShaderInfo->entrypointCount; i++)
	    {
		    const Entrypoint& entrypoint = info.pShaderInfo->pEntrypoints[i];

	    	vk::PipelineShaderStageCreateInfo stage = {
	    		{},
	    		VulkanTypes::GetShaderStageFlag(entrypoint.stage),
	    		module,
	    		entrypoint.pName
	    	};

	    	stages.push_back(stage);
	    }

	    for (auto [binding, stride, inputRate] : info.vertexBindings)
	    {
		    bindings.emplace_back(
			    static_cast<uint32_t>(binding),
			    static_cast<uint32_t>(stride),
			    VulkanTypes::GetVertexInputRate(inputRate)
		    );
	    }

	    for (auto [location, binding, offset, format] : info.vertexAttributes)
	    {
		    attributes.emplace_back(
			    static_cast<uint32_t>(location),
			    static_cast<uint32_t>(binding),
			    VulkanTypes::GetVkFormat(format),
			    static_cast<uint32_t>(offset)
		    );
	    }

	    const vk::PipelineVertexInputStateCreateInfo vertexInput(
		    {},
		    static_cast<uint32_t>(bindings.size()), bindings.data(),
		    static_cast<uint32_t>(attributes.size()), attributes.data()
	    );

	    constexpr vk::PipelineInputAssemblyStateCreateInfo inputAssembly(
		    {}, vk::PrimitiveTopology::eTriangleList, false
	    );

	    constexpr vk::Viewport viewport;
	    constexpr vk::Rect2D scissor;

	    const vk::PipelineViewportStateCreateInfo viewportState(
		    {}, 1, &viewport, 1, &scissor
	    );

	    constexpr vk::PipelineRasterizationStateCreateInfo rasterizerState;
	    constexpr vk::PipelineMultisampleStateCreateInfo multisampleState;

	    const vk::PipelineDepthStencilStateCreateInfo depthStencilState(
		    {}, true, true, vk::CompareOp::eLess
	    );

	    constexpr vk::PipelineColorBlendAttachmentState attachment(
		    false, vk::BlendFactor::eZero,
		    vk::BlendFactor::eZero, vk::BlendOp::eAdd,
		    vk::BlendFactor::eZero, vk::BlendFactor::eZero,
		    vk::BlendOp::eAdd,
		    vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG
		    | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
	    );

	    const vk::PipelineColorBlendStateCreateInfo colorBlendState(
		    {}, false, vk::LogicOp::eCopy, 1, &attachment
	    );

	    vk::DynamicState dynamicStates[] = {
		    vk::DynamicState::eViewport,
		    vk::DynamicState::eCullMode
	    };

	    const vk::PipelineDynamicStateCreateInfo dynamicState(
		    {}, std::size(dynamicStates), dynamicStates
	    );

	    const vk::PipelineRenderingCreateInfo rendering(
		    0,
		    static_cast<uint32_t>(formats.size()),
		    formats.data(),
		    VulkanTypes::GetVkFormat(info.depthFormat)
	    );

	    const vk::GraphicsPipelineCreateInfo createInfo(
		    {}, static_cast<uint32_t>(stages.size()), stages.data(), &vertexInput, &inputAssembly,
		    nullptr, &viewportState, &rasterizerState, &multisampleState,
		    &depthStencilState, &colorBlendState, &dynamicState,
		    VULKAN_CAST(PipelineLayout, info.layout), nullptr, 0,
		    nullptr, 0, &rendering
	    );

    	vk::Pipeline pipeline = m_device.createGraphicsPipeline({}, createInfo).value;

    	m_device.destroyShaderModule(module);

	    return PipelineID(pipeline);
    }

    PipelineLayoutID VulkanDriver::CreatePipelineLayout(std::span<PushConstantRange> ranges,
                                                        const std::span<DescriptorSetLayoutID> layouts)
    {
	    std::vector<vk::PushConstantRange> vkRanges;
	    vkRanges.reserve(ranges.size());

	    for (auto [size, offset, stage] : ranges)
	    {
		    vk::ShaderStageFlags vkStage;
		    switch (stage)
		    {
			    case ShaderStageFlagBits::VERTEX: vkStage = vk::ShaderStageFlagBits::eVertex; break;
			    case ShaderStageFlagBits::FRAGMENT: vkStage = vk::ShaderStageFlagBits::eFragment; break;
			    case ShaderStageFlagBits::ALL: vkStage = vk::ShaderStageFlagBits::eAll; break;
		    }

		    vkRanges.emplace_back(
			    vkStage,
			    static_cast<uint32_t>(offset),
			    static_cast<uint32_t>(size)
		    );
	    }

	    const vk::PipelineLayoutCreateInfo createInfo(
		    {},
		    static_cast<uint32_t>(layouts.size()), reinterpret_cast<vk::DescriptorSetLayout*>(layouts.data()),
		    static_cast<uint32_t>(vkRanges.size()), vkRanges.data()
	    );

	    return PipelineLayoutID(m_device.createPipelineLayout(createInfo));
    }



    SemaphoreID VulkanDriver::CreateSemaphore()
    {
	    return SemaphoreID(m_device.createSemaphore({}));
    }

    SwapchainID VulkanDriver::CreateSwapchain(const SwapchainInfo& info)
    {
	    const auto surface = VULKAN_CAST(SurfaceKHR, info.surface);

	    vk::SurfaceCapabilitiesKHR capabilities;
	    LE_CHECK_RESULT(m_physicalDevice.getSurfaceCapabilitiesKHR(surface, &capabilities));

	    uint32_t imageCount = capabilities.minImageCount + 1;
	    if (capabilities.maxImageCount > 0 &&
	        imageCount > capabilities.maxImageCount)
		    imageCount = capabilities.maxImageCount;

	    const vk::PresentModeKHR presentMode = PickPresentMode(surface, info.vsync);

	    const vk::SwapchainCreateInfoKHR createInfo(
		    {}, surface, imageCount,
		    VulkanTypes::GetVkFormat(info.format), vk::ColorSpaceKHR::eSrgbNonlinear,
		    { info.extent.width, info.extent.height }, 1,
		    vk::ImageUsageFlagBits::eColorAttachment, vk::SharingMode::eExclusive,
		    0, nullptr, capabilities.currentTransform,
		    vk::CompositeAlphaFlagBitsKHR::eOpaque, presentMode,
		    true
	    );

	    return SwapchainID(m_device.createSwapchainKHR(createInfo));
    }

    SurfaceID VulkanDriver::CreateSurface(Window& window)
    {
	    return SurfaceID(PlatformUtils::CreateSurface(m_instance, window));
    }

    DescriptorSetLayoutID VulkanDriver::CreateDescriptorSetLayout(std::span<DescriptorSetLayoutBinding> bindings)
    {
    	std::vector<vk::DescriptorSetLayoutBinding> vkBindings;
    	vkBindings.reserve(bindings.size());

    	for (const DescriptorSetLayoutBinding& binding : bindings)
    	{
    		vk::DescriptorSetLayoutBinding vkBinding{};
    		vkBinding.binding = static_cast<uint32_t>(binding.binding);
    		vkBinding.descriptorCount = static_cast<uint32_t>(binding.descriptorCount);
    		vkBinding.descriptorType = VulkanTypes::GetDescriptorType(binding.descriptorType);
    		vkBinding.stageFlags = VulkanTypes::GetShaderStageFlags(binding.stageFlags);

    		vkBindings.push_back(vkBinding);
    	}

    	const vk::DescriptorSetLayoutCreateInfo info(
			{}, static_cast<uint32_t>(vkBindings.size()), vkBindings.data()
		);

	    return DescriptorSetLayoutID(m_device.createDescriptorSetLayout(info));
    }

    SamplerID VulkanDriver::CreateSampler(const SamplerInfo& info)
    {
	    vk::PhysicalDeviceProperties properties;
	    m_physicalDevice.getProperties(&properties);

	    const vk::Filter filter = VulkanTypes::GetFilter(info.filter);
	    const vk::SamplerAddressMode addressMode = VulkanTypes::GetSamplerAddressMode(info.addressMode);
	    const vk::BorderColor borderColor = VulkanTypes::GetBorderColor(info.borderColor);

	    const vk::SamplerCreateInfo createInfo(
		    {}, filter, filter, vk::SamplerMipmapMode::eNearest,
		    addressMode, addressMode, addressMode,
		    0.0f, false, properties.limits.maxSamplerAnisotropy,
		    false, vk::CompareOp::eAlways, 0.0f, 1.0f,
		    borderColor
	    );

	    return SamplerID(m_device.createSampler(createInfo));
    }

    QueueID VulkanDriver::GetQueue(const QueueFamily family)
    {
	    uint32_t index = 0;
	    switch (family)
	    {
		    case QueueFamily::GRAPHICS: index = m_indices.graphicsFamilyIndex; break;
		    case QueueFamily::COMPUTE: index = m_indices.computeFamilyIndex; break;
		    case QueueFamily::TRANSFER: index = m_indices.transferFamilyIndex; break;
	    }

	    return QueueID(m_device.getQueue(index, 0));
    }

    void VulkanDriver::FreeCommandBuffers(const CommandPoolID pool, const size_t count, CommandBufferID* buffers)
    {
    	m_device.freeCommandBuffers(VULKAN_CAST(CommandPool, pool), static_cast<uint32_t>(count),
    		reinterpret_cast<vk::CommandBuffer*>(buffers));
    }

    void VulkanDriver::FreeDescriptorSets(const PoolManagerID manager, const DescriptorPoolID pool, const size_t count,
                                          DescriptorSetID* sets)
    {
	    reinterpret_cast<PoolManager*>(manager.id)->Free(VULKAN_CAST(DescriptorPool, pool), count, sets);
    }

    void VulkanDriver::DestroyBuffer(const BufferID buffer)
    {
	    const auto vkBuffer = reinterpret_cast<VulkanBuffer*>(buffer.id);
	    vmaDestroyBuffer(m_allocator, vkBuffer->buffer, vkBuffer->allocation);
	    delete vkBuffer;
    }

    void VulkanDriver::DestroyCommandPool(const CommandPoolID pool)
    {
	    m_device.destroyCommandPool(VULKAN_CAST(CommandPool, pool));
    }

    void VulkanDriver::DestroyLayoutPoolManager(const PoolManagerID manager)
    {
	    delete reinterpret_cast<PoolManager*>(manager.id);
    }

    void VulkanDriver::DestroyFence(const FenceID fence)
    {
	    m_device.destroyFence(VULKAN_CAST(Fence, fence));
    }

    void VulkanDriver::DestroyImage(const ImageID image)
    {
	    const auto vkImage = reinterpret_cast<VulkanImage*>(image.id);
	    vmaDestroyImage(m_allocator, vkImage->image, vkImage->allocation);
	    delete vkImage;
    }

    void VulkanDriver::DestroyImageView(const ImageViewID view)
    {
	    m_device.destroyImageView(VULKAN_CAST(ImageView, view));
    }

    void VulkanDriver::DestroyPipeline(const PipelineID pipeline)
    {
	    m_device.destroyPipeline(VULKAN_CAST(Pipeline, pipeline));
    }

    void VulkanDriver::DestroyPipelineLayout(const PipelineLayoutID pipelineLayout)
    {
	    m_device.destroyPipelineLayout(VULKAN_CAST(PipelineLayout, pipelineLayout));
    }

    void VulkanDriver::DestroySemaphore(const SemaphoreID semaphore)
    {
	    m_device.destroySemaphore(VULKAN_CAST(Semaphore, semaphore));
    }

    void VulkanDriver::DestroySwapchain(const SwapchainID swapchain)
    {
	    m_device.destroySwapchainKHR(VULKAN_CAST(SwapchainKHR, swapchain));
    }

    void VulkanDriver::DestroySurface(const SurfaceID surface)
    {
	    m_instance.destroySurfaceKHR(VULKAN_CAST(SurfaceKHR, surface));
    }

    void VulkanDriver::DestroyShaderModule(const ShaderModuleID shaderModule)
    {
	    m_device.destroyShaderModule(VULKAN_CAST(ShaderModule, shaderModule));
    }

    void VulkanDriver::DestroyDescriptorSetLayout(const DescriptorSetLayoutID layout)
    {
	    m_device.destroyDescriptorSetLayout(VULKAN_CAST(DescriptorSetLayout, layout));
    }

    void VulkanDriver::DestroySampler(const SamplerID sampler)
    {
	    m_device.destroySampler(VULKAN_CAST(Sampler, sampler));
    }

    void VulkanDriver::ResetAllPools(const PoolManagerID manager)
    {
	    reinterpret_cast<PoolManager*>(manager.id)->ResetAllPools();
    }

    void VulkanDriver::WaitForFences(const size_t count, FenceID* fences)
    {
    	LE_CHECK_RESULT(m_device.waitForFences(static_cast<uint32_t>(count), reinterpret_cast<vk::Fence*>(fences), true,
    		UINT64_MAX));
    }

    void VulkanDriver::WaitIdle()
    {
	    m_device.waitIdle();
    }

    void VulkanDriver::ResetFences(const size_t count, FenceID* fences)
    {
	    LE_CHECK_RESULT(m_device.resetFences(static_cast<uint32_t>(count), reinterpret_cast<vk::Fence*>(fences)));
    }

    void VulkanDriver::QueueSubmit(const QueueID queue, const SubmitInfo& info)
    {
	    const auto vkQueue = VULKAN_CAST(Queue, queue);
	    const auto vkFence = VULKAN_CAST(Fence, info.fence);

	    std::vector<vk::PipelineStageFlags> waitStages;
	    waitStages.reserve(info.waitSemaphores.size());

	    for (const PipelineStage stage : info.waitDstStageMask)
		    waitStages.emplace_back(VulkanTypes::GetPipelineStage(stage));

	    const auto commandBuffer = VULKAN_CAST(CommandBuffer, info.commandBuffer);

	    const vk::SubmitInfo submit(
		    static_cast<uint32_t>(info.waitSemaphores.size()),
		    reinterpret_cast<const vk::Semaphore*>(info.waitSemaphores.data()),
		    waitStages.data(),
		    1, &commandBuffer,
		    static_cast<uint32_t>(info.signalSemaphores.size()),
		    reinterpret_cast<const vk::Semaphore*>(info.signalSemaphores.data())
	    );

	    LE_CHECK_RESULT(vkQueue.submit(1, &submit, vkFence));
    }

    void VulkanDriver::QueuePresent(const QueueID queue, const PresentInfo& info)
    {
	    const auto vkQueue = VULKAN_CAST(Queue, queue);

	    const vk::PresentInfoKHR presentInfo(
		    static_cast<uint32_t>(info.waitSemaphores.size()),
		    reinterpret_cast<const vk::Semaphore*>(info.waitSemaphores.data()),
		    static_cast<uint32_t>(info.swapchains.size()),
		    reinterpret_cast<vk::SwapchainKHR*>(info.swapchains.data()),
		    info.imageIndices.data()
	    );

	    LE_CHECK_RESULT(vkQueue.presentKHR(presentInfo));
    }

    void VulkanDriver::QueueWaitIdle(const QueueID queue)
    {
	    VULKAN_CAST(Queue, queue).waitIdle();
    }

    void VulkanDriver::ResetCommandBuffer(const CommandBufferID buffer)
    {
	    VULKAN_CAST(CommandBuffer, buffer).reset();
    }

    void VulkanDriver::BeginCommandBuffer(const CommandBufferID buffer, bool singleUse)
    {
	    const vk::CommandBufferBeginInfo beginInfo(
		    singleUse ? vk::CommandBufferUsageFlagBits::eOneTimeSubmit : vk::CommandBufferUsageFlags{}
	    );

	    VULKAN_CAST(CommandBuffer, buffer).begin(beginInfo);
    }

    void VulkanDriver::EndCommandBuffer(const CommandBufferID buffer)
    {
	    VULKAN_CAST(CommandBuffer, buffer).end();
    }

    void* VulkanDriver::GetMappedBufferData(const BufferID buffer)
    {
	    return reinterpret_cast<VulkanBuffer*>(buffer.id)->allocationInfo.pMappedData;
    }

    size_t VulkanDriver::GetBufferSize(const BufferID buffer)
    {
    	return reinterpret_cast<VulkanBuffer*>(buffer.id)->allocationInfo.size;
    }

    bool VulkanDriver::IsFenceSignaled(const FenceID fence)
    {
	    return m_device.getFenceStatus(VULKAN_CAST(Fence, fence)) == vk::Result::eSuccess;
    }

    SurfaceCapabilities VulkanDriver::GetSurfaceCapabilities(const SurfaceID surface)
    {
    	const vk::SurfaceCapabilitiesKHR surfaceCapabilities =
    		m_physicalDevice.getSurfaceCapabilitiesKHR(VULKAN_CAST(SurfaceKHR, surface));

    	return {
    		.currentExtent = {
    			surfaceCapabilities.currentExtent.width,
    			surfaceCapabilities.currentExtent.height,
    		}
    	};
    }

    Format VulkanDriver::FindDepthFormat()
    {
    	constexpr vk::Format candidates[] = {
    		vk::Format::eD32Sfloat,
			vk::Format::eD32SfloatS8Uint,
			vk::Format::eD24UnormS8Uint,
		};

    	constexpr vk::FormatFeatureFlags features = vk::FormatFeatureFlagBits::eDepthStencilAttachment;

    	for (const vk::Format format : candidates)
    	{
    		vk::FormatProperties props;
    		m_physicalDevice.getFormatProperties(format, &props);

    		if ((props.optimalTilingFeatures & features) == features)
    			return VulkanTypes::ToFormat(format);
    	}

    	return VulkanTypes::ToFormat(candidates[0]);
    }

    bool VulkanDriver::HasTransferQueue()
    {
	    return m_indices.hasTransferFamily;
    }

    void VulkanDriver::UpdateDescriptorSets(const std::span<WriteDescriptorSet> writes)
    {
		size_t bufferInfoIndex = 0;
    	size_t imageInfoIndex = 0;
    	std::vector<vk::DescriptorBufferInfo> bufferInfos;
    	std::vector<vk::DescriptorImageInfo> imageInfos;

    	bufferInfos.reserve(writes.size());
    	imageInfos.reserve(writes.size());

    	for (const WriteDescriptorSet& write : writes)
    	{
    		if (write.pBufferInfo)
    		{
    			bufferInfos.push_back({
    				VULKAN_CAST(Buffer, write.pBufferInfo->buffer),
    				write.pBufferInfo->offset,
    				write.pBufferInfo->range
    			});
    		}

    		if (write.pImageInfo)
    		{
    			imageInfos.emplace_back(
    				VULKAN_CAST(Sampler, write.pImageInfo->sampler),
    				VULKAN_CAST(ImageView, write.pImageInfo->imageView),
    				VulkanTypes::GetImageLayout(write.pImageInfo->imageLayout)
    			);
    		}
    	}

    	std::vector<vk::WriteDescriptorSet> vkWrites;
    	vkWrites.reserve(writes.size());
    	for (const WriteDescriptorSet& write : writes)
    	{
    		vkWrites.emplace_back(
    			VULKAN_CAST(DescriptorSet, write.dstSet),
    			static_cast<uint32_t>(write.dstBinding),
    			static_cast<uint32_t>(write.dstArrayElement),
    			static_cast<uint32_t>(write.descriptorCount),
    			VulkanTypes::GetDescriptorType(write.descriptorType),
    			write.pImageInfo ? &imageInfos[imageInfoIndex++] : nullptr,
    			write.pBufferInfo ? &bufferInfos[bufferInfoIndex++] : nullptr
    		);
    	}

    	m_device.updateDescriptorSets(vkWrites, {});
    }

    bool VulkanDriver::AcquireNextImage(const SwapchainID swapchain, const SemaphoreID waitSemaphore, uint32_t& outIndex)
    {
	    return m_device.acquireNextImageKHR(VULKAN_CAST(SwapchainKHR, swapchain), UINT64_MAX,
	    	VULKAN_CAST(Semaphore, waitSemaphore),
	    	{}, &outIndex
	    ) == vk::Result::eSuccess;
    }

    void VulkanDriver::CmdCopyBuffer(const CommandBufferID buffer, const BufferID src, const BufferID dst, std::span<BufferCopy> regions)
    {
	    std::vector<vk::BufferCopy> vkRegions(regions.size());
	    for (size_t i = 0; i < vkRegions.size(); i++)
	    {
		    const auto& [srcOffset, dstOffset, size] = regions[i];

		    vk::BufferCopy regionCopy{};
		    regionCopy.size = size;
		    regionCopy.srcOffset = srcOffset;
		    regionCopy.dstOffset = dstOffset;

		    vkRegions[i] = regionCopy;
	    }

	    const auto vkSrc = VULKAN_CAST(Buffer, src);
	    const auto vkDst = VULKAN_CAST(Buffer, dst);
	    VULKAN_CAST(CommandBuffer, buffer).copyBuffer(vkSrc, vkDst, static_cast<uint32_t>(vkRegions.size()), vkRegions.data());
    }

    void VulkanDriver::CmdCopyBufferToImage(const CommandBufferID buffer, const BufferID src,
                                            const ImageID dst, const ImageLayout layout, const std::span<BufferImageCopy> regions)
    {
	    std::vector<vk::BufferImageCopy> vkRegions(regions.size());
	    for (size_t i = 0; i < vkRegions.size(); i++)
	    {
		    const BufferImageCopy& region = regions[i];
		    vk::BufferImageCopy& regionCopy = vkRegions[i];

		    regionCopy.bufferOffset = region.bufferOffset;
		    regionCopy.bufferOffset = region.bufferOffset;
		    regionCopy.bufferRowLength = region.bufferRowLength;
		    regionCopy.bufferImageHeight = region.bufferImageHeight;
		    regionCopy.imageSubresource.aspectMask     = VulkanTypes::GetImageAspectFlags(region.imageSubresource.aspect);
		    regionCopy.imageSubresource.mipLevel       = region.imageSubresource.baseMipLevel;
		    regionCopy.imageSubresource.baseArrayLayer = region.imageSubresource.baseArrayLayer;
		    regionCopy.imageSubresource.layerCount     = region.imageSubresource.layerCount;
		    regionCopy.imageOffset.x = region.imageOffset.x;
		    regionCopy.imageOffset.y = region.imageOffset.y;
		    regionCopy.imageOffset.z = region.imageOffset.z;
		    regionCopy.imageExtent.width = region.imageExtent.width;
		    regionCopy.imageExtent.height = region.imageExtent.height;
		    regionCopy.imageExtent.depth = region.imageExtent.depth;
	    }

	    const auto vkBuffer = VULKAN_CAST(Buffer, src);
	    const auto vkImage = VULKAN_CAST(Image, dst);
	    const vk::ImageLayout vkLayout = VulkanTypes::GetImageLayout(layout);
	    VULKAN_CAST(CommandBuffer, buffer).copyBufferToImage(vkBuffer, vkImage, vkLayout,
	                                                         static_cast<uint32_t>(vkRegions.size()), vkRegions.data());
    }

    void VulkanDriver::CmdPipelineBarrier(const CommandBufferID buffer,
                                          const PipelineStage srcStage, const PipelineStage dstStage,
                                          const std::span<ImageMemoryBarrier> imageMemoryBarriers)
    {
	    const vk::PipelineStageFlags srcStageMask = VulkanTypes::GetPipelineStage(srcStage);
	    const vk::PipelineStageFlags dstStageMask = VulkanTypes::GetPipelineStage(dstStage);
	    std::vector<vk::ImageMemoryBarrier> vkImageMemoryBarriers(imageMemoryBarriers.size());

	    for (size_t i = 0; i < vkImageMemoryBarriers.size(); i++)
	    {
		    const ImageMemoryBarrier& barrier = imageMemoryBarriers[i];
		    vk::ImageMemoryBarrier& vkBarrier = vkImageMemoryBarriers[i];

		    vkBarrier.oldLayout = VulkanTypes::GetImageLayout(barrier.oldLayout);
		    vkBarrier.newLayout = VulkanTypes::GetImageLayout(barrier.newLayout);
	    	vkBarrier.srcAccessMask = VulkanTypes::GetAccessFlags(barrier.srcAccessMask);
	    	vkBarrier.dstAccessMask = VulkanTypes::GetAccessFlags(barrier.dstAccessMask);
		    vkBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		    vkBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		    vkBarrier.image = VULKAN_CAST(Image, barrier.image);
		    vkBarrier.subresourceRange.aspectMask = VulkanTypes::GetImageAspectFlags(barrier.subresourceRange.aspect);
		    vkBarrier.subresourceRange.levelCount = barrier.subresourceRange.baseMipLevel;
		    vkBarrier.subresourceRange.baseArrayLayer = barrier.subresourceRange.baseArrayLayer;
		    vkBarrier.subresourceRange.layerCount = barrier.subresourceRange.layerCount;
	    }

	    VULKAN_CAST(CommandBuffer, buffer).pipelineBarrier(
		    srcStageMask, dstStageMask,
		    {}, 0, nullptr,
		    0, nullptr,
		    static_cast<uint32_t>(vkImageMemoryBarriers.size()), vkImageMemoryBarriers.data()
	    );
    }

    void VulkanDriver::CmdBeginRendering(const CommandBufferID buffer, const RenderingInfo& info)
    {
	    std::vector<vk::RenderingAttachmentInfo> attachments;
	    attachments.reserve(info.colorAttachments.size());

	    for (const auto& [imageView, imageLayout, clearValue] : info.colorAttachments)
	    {
		    vk::RenderingAttachmentInfo attachmentInfo;
		    attachmentInfo.imageView = VULKAN_CAST(ImageView, imageView);
		    attachmentInfo.imageLayout = VulkanTypes::GetImageLayout(imageLayout);
		    attachmentInfo.clearValue.color =
		    {
			    clearValue.x,
			    clearValue.y,
			    clearValue.z,
			    clearValue.w
		    };
		    attachmentInfo.loadOp = vk::AttachmentLoadOp::eClear;
		    attachmentInfo.storeOp = vk::AttachmentStoreOp::eStore;

		    attachments.emplace_back(attachmentInfo);
	    }

	    vk::RenderingAttachmentInfo depth;
	    depth.imageView = VULKAN_CAST(ImageView, info.depthAttachment.imageView);
	    depth.imageLayout = VulkanTypes::GetImageLayout(info.depthAttachment.imageLayout);
	    depth.clearValue.color =
	    {
		    info.depthAttachment.clearValue.x,
		    info.depthAttachment.clearValue.y,
		    info.depthAttachment.clearValue.z,
		    info.depthAttachment.clearValue.w
	    };
	    depth.loadOp = vk::AttachmentLoadOp::eClear;
	    depth.storeOp = vk::AttachmentStoreOp::eStore;

	    const vk::Rect2D extent(
	    	{},
	    	{
	    		static_cast<uint32_t>(info.extent.width),
				static_cast<uint32_t>(info.extent.height)
	    	}
	    );

	    const vk::RenderingInfoKHR renderingInfo(
		    {}, extent,
		    1, {},
		    static_cast<uint32_t>(attachments.size()), attachments.data(),
		    &depth
	    );

	    VULKAN_CAST(CommandBuffer, buffer).beginRenderingKHR(renderingInfo);
    }

    void VulkanDriver::CmdSetViewport(const CommandBufferID buffer, const Extent2D size)
    {
	    const vk::Viewport viewport(
		    0, 0,
		    static_cast<float>(size.width),
		    static_cast<float>(size.height)
	    );

	    VULKAN_CAST(CommandBuffer, buffer).setViewport(0, 1,
	                                                   &viewport);
    }

    void VulkanDriver::CmdSetScissor(const CommandBufferID buffer, Rect2D rect)
    {
	    const vk::Rect2D scissor(
		    { rect.offset.x, rect.offset.y },
		    { rect.extent.width, rect.extent.height }
	    );

	    VULKAN_CAST(CommandBuffer, buffer).setScissor(0, 1,
	                                                  &scissor);
    }

    void VulkanDriver::CmdBindPipeline(const CommandBufferID buffer,
                                       const PipelineBindPoint bindPoint, const PipelineID pipeline)
    {
	    VULKAN_CAST(CommandBuffer, buffer).bindPipeline(
		    VulkanTypes::GetPipelineBindPoint(bindPoint),
		    VULKAN_CAST(Pipeline, pipeline));
    }

    void VulkanDriver::CmdSetCullMode(const CommandBufferID buffer, CullMode cullMode)
    {
	    VULKAN_CAST(CommandBuffer, buffer).setCullMode(VulkanTypes::GetCullModeFlags(cullMode));
    }

    void VulkanDriver::CmdPushConstants(const CommandBufferID buffer, const PipelineLayoutID layout,
                                        const ShaderStageFlagBits stage, const size_t offset, const size_t size, const void* values)
    {
	    VULKAN_CAST(CommandBuffer, buffer).pushConstants(
		    VULKAN_CAST(PipelineLayout, layout),
		    VulkanTypes::GetShaderStageFlags(stage),
		    static_cast<uint32_t>(offset), static_cast<uint32_t>(size), values
	    );
    }

    void VulkanDriver::CmdBindDescriptorSets(const CommandBufferID buffer, const PipelineBindPoint bindPoint,
                                             const PipelineLayoutID layout, const size_t firstSet, const std::span<DescriptorSetID> sets)
    {
	    VULKAN_CAST(CommandBuffer, buffer).bindDescriptorSets(
		    VulkanTypes::GetPipelineBindPoint(bindPoint), VULKAN_CAST(PipelineLayout, layout),
		    static_cast<uint32_t>(firstSet),
		    static_cast<uint32_t>(sets.size()), reinterpret_cast<vk::DescriptorSet*>(sets.data()),
		    0, nullptr
	    );
    }

    void VulkanDriver::CmdBindVertexBuffers(const CommandBufferID buffer, const uint32_t firstBinding,
                                            const std::span<BufferID> buffers)
    {
	    std::vector<vk::Buffer> vkBuffers;
	    const std::vector<uint64_t> offsets(buffers.size(), 0);

	    vkBuffers.reserve(buffers.size());
	    for (const auto& id : buffers)
	    {
		    vkBuffers.emplace_back(reinterpret_cast<VulkanBuffer*>(id.id)->buffer);
	    }

	    VULKAN_CAST(CommandBuffer, buffer).bindVertexBuffers(firstBinding, vkBuffers, offsets);
    }

    void VulkanDriver::CmdBindIndexBuffer(const CommandBufferID buffer, const BufferID indexBuffer,
                                          const uint64_t offset)
    {
	    VULKAN_CAST(CommandBuffer, buffer).bindIndexBuffer(
		    reinterpret_cast<VulkanBuffer*>(indexBuffer.id)->buffer,
		    offset,
		    vk::IndexType::eUint32
	    );
    }

    void VulkanDriver::CmdDrawIndexed(const CommandBufferID buffer, const uint32_t indexCount,
                                      const uint32_t instanceCount, const uint32_t firstIndex, const int32_t vertexOffset,
                                      const uint32_t firstInstance)
    {
	    VULKAN_CAST(CommandBuffer, buffer).drawIndexed(indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
    }

    void VulkanDriver::CmdEndRendering(const CommandBufferID buffer)
    {
	    VULKAN_CAST(CommandBuffer, buffer).endRenderingKHR();
    }

    void VulkanDriver::TransitionImageLayout(const CommandBufferID buffer,
                                             const ImageID image, const ImageLayout oldLayout,
                                             const ImageLayout newLayout, const ImageAspect aspect)
    {
	    const auto vkImage = VULKAN_CAST(Image, image);
	    const vk::ImageLayout vkOldLayout = VulkanTypes::GetImageLayout(oldLayout);
	    const vk::ImageLayout vkNewLayout = VulkanTypes::GetImageLayout(newLayout);
	    const vk::ImageAspectFlags aspectMask = VulkanTypes::GetImageAspectFlags(aspect);

	    vk::ImageMemoryBarrier barrier{};
	    barrier.oldLayout = vkOldLayout;
	    barrier.newLayout = vkNewLayout;
	    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	    barrier.image = vkImage;
	    barrier.subresourceRange.aspectMask = aspectMask;
	    barrier.subresourceRange.baseMipLevel = 0;
	    barrier.subresourceRange.levelCount = 1;
	    barrier.subresourceRange.baseArrayLayer = 0;
	    barrier.subresourceRange.layerCount = 1;

	    vk::PipelineStageFlags sourceStage;
	    vk::PipelineStageFlags destinationStage;

	    if (vkOldLayout == vk::ImageLayout::eUndefined && vkNewLayout == vk::ImageLayout::eTransferDstOptimal) {
		    barrier.srcAccessMask = {};
		    barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

		    sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
		    destinationStage = vk::PipelineStageFlagBits::eTransfer;
	    }
	    else if (vkOldLayout == vk::ImageLayout::eTransferDstOptimal && vkNewLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
		    barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
		    barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

		    sourceStage = vk::PipelineStageFlagBits::eTransfer;
		    destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
	    }
	    else if (vkOldLayout == vk::ImageLayout::eUndefined
	             && vkNewLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
	    {
		    barrier.srcAccessMask = {};
		    barrier.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentRead
		                            | vk::AccessFlagBits::eDepthStencilAttachmentWrite;

		    sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
		    destinationStage = vk::PipelineStageFlagBits::eEarlyFragmentTests;
	    }
	    else
	    {
		    LE_ASSERT(false, "Unsupported layout transition");
	    }

	    VULKAN_CAST(CommandBuffer, buffer).pipelineBarrier(
		    sourceStage, destinationStage,
		    {},
		    0, nullptr,
		    0, nullptr,
		    1, &barrier
	    );
    }

    void VulkanDriver::CreateInstance(const std::string_view applicationName)
    {
	    const vk::ApplicationInfo appInfo(
		    applicationName.data(),
		    1,
		    "LegendEngine",
		    1,
		    VK_API_VERSION_1_3
	    );

	    LE_ASSERT(IsValidationSupported(), "Validation layers not supported");

	    const std::vector<const char*> extensions = PlatformUtils::GetRequiredExtensions();

	    const vk::InstanceCreateInfo instanceCreateInfo(
		    {}, &appInfo,
#ifndef NDEBUG
		    static_cast<uint32_t>(VALIDATION_LAYERS.size()), VALIDATION_LAYERS.data(),
#else
		    0, nullptr,
#endif
		    static_cast<uint32_t>(extensions.size()),
		    extensions.data()
	    );
	    m_instance = vk::createInstance(instanceCreateInfo);
	    PlatformUtils::LoadVulkanFuncs(m_instance);

	    constexpr vk::DebugUtilsMessengerCreateInfoEXT messengerInfo(
		    {},
		    vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
		    vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
		    vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
		    vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning,
		    vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
		    vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
		    vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
		    OnValidationMessage
	    );

	    m_messenger = m_instance.createDebugUtilsMessengerEXT(messengerInfo);
    }

    void VulkanDriver::CreateDevice()
    {
	    m_physicalDevice = PickDevice();

	    constexpr float priority = 1.0f;
	    std::vector<vk::DeviceQueueCreateInfo> queueInfos;

	    LE_ASSERT(m_indices.hasGraphicsFamily, "No graphics family found");

	    queueInfos.push_back(vk::DeviceQueueCreateInfo({}, m_indices.graphicsFamilyIndex,
	                                                   1, &priority));

	    if (m_indices.hasTransferFamily)
		    queueInfos.push_back(vk::DeviceQueueCreateInfo({}, m_indices.transferFamilyIndex,
		                                                   1, &priority));

	    const char* extensions[] =
	    {
		    vk::KHRSwapchainExtensionName,
		    vk::KHRDynamicRenderingExtensionName
	    };

	    constexpr vk::PhysicalDeviceDynamicRenderingFeaturesKHR dynamicRendering(true);

	    vk::DeviceCreateInfo deviceInfo;
	    deviceInfo.queueCreateInfoCount = static_cast<uint32_t>(queueInfos.size());
	    deviceInfo.pQueueCreateInfos = queueInfos.data();
	    deviceInfo.enabledExtensionCount = std::size(extensions);
	    deviceInfo.ppEnabledExtensionNames = extensions;
	    deviceInfo.pNext = &dynamicRendering;

	    m_device = m_physicalDevice.createDevice(deviceInfo);
    }

    void VulkanDriver::CreateAllocator()
    {
	    VmaVulkanFunctions functions{};
	    functions.vkGetInstanceProcAddr = VULKAN_HPP_DEFAULT_DISPATCHER.vkGetInstanceProcAddr;
	    functions.vkGetDeviceProcAddr = VULKAN_HPP_DEFAULT_DISPATCHER.vkGetDeviceProcAddr;

	    VmaAllocatorCreateInfo allocatorInfo{};
	    allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_3;
	    allocatorInfo.pVulkanFunctions = &functions;
	    allocatorInfo.device = m_device;
	    allocatorInfo.instance = m_instance;
	    allocatorInfo.physicalDevice = m_physicalDevice;

	    LE_CHECK_VK(vmaCreateAllocator(&allocatorInfo, &m_allocator));
    }

    vk::PhysicalDevice VulkanDriver::PickDevice()
    {
	    uint32_t deviceCount = 0;
	    LE_CHECK_RESULT(m_instance.enumeratePhysicalDevices(&deviceCount, nullptr));

	    LE_ASSERT(deviceCount > 0, "No suitable device found");

	    std::vector<vk::PhysicalDevice> devices(deviceCount);
	    LE_CHECK_RESULT(m_instance.enumeratePhysicalDevices(&deviceCount, devices.data()));

	    for (const vk::PhysicalDevice device : devices)
		    if (IsDeviceSuitable(device))
			    return device;

	    LE_ASSERT(false, "No suitable device found");
	    return {};
    }

    void VulkanDriver::FindQueueFamilies(const vk::PhysicalDevice device)
    {
	    uint32_t familyCount = 0;
	    device.getQueueFamilyProperties(&familyCount, nullptr);

	    LE_ASSERT(familyCount > 0, "No suitable queue families");

	    std::vector<vk::QueueFamilyProperties> families(familyCount);
	    device.getQueueFamilyProperties(&familyCount, families.data());

	    for (size_t i = 0; i < families.size(); i++)
	    {
		    const vk::QueueFamilyProperties& queueFamily = families[i];

		    if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics
		        && !m_indices.hasGraphicsFamily)
		    {
			    m_indices.hasGraphicsFamily = true;
			    m_indices.graphicsFamilyIndex = static_cast<uint32_t>(i);
		    }

		    if (queueFamily.queueFlags & vk::QueueFlagBits::eTransfer
		        && !m_indices.hasTransferFamily)
		    {
			    m_indices.hasTransferFamily = true;
			    m_indices.transferFamilyIndex = static_cast<uint32_t>(i);
		    }

		    if (queueFamily.queueFlags & vk::QueueFlagBits::eCompute
		        && !m_indices.hasComputeFamily)
		    {
			    m_indices.hasComputeFamily = true;
			    m_indices.computeFamilyIndex = static_cast<uint32_t>(i);
		    }
	    }

	    // Try to find a dedicated transfer queue
	    for (size_t i = 0; i < families.size(); i++)
	    {
		    if (!(families[i].queueFlags & vk::QueueFlagBits::eTransfer))
			    continue;

		    if (m_indices.hasGraphicsFamily && m_indices.graphicsFamilyIndex == i)
			    continue;

		    m_indices.transferFamilyIndex = static_cast<uint32_t>(i);
		    break;
	    }
    }

    bool VulkanDriver::IsDeviceSuitable(const vk::PhysicalDevice device)
    {
	    vk::PhysicalDeviceProperties deviceProperties;
	    vk::PhysicalDeviceFeatures deviceFeatures;
	    device.getProperties(&deviceProperties);
	    device.getFeatures(&deviceFeatures);

	    const bool extensionsSupported = CheckDeviceExtensionSupport(device,
	                                                                 DEVICE_EXTENSIONS.data(), DEVICE_EXTENSIONS.size());

	    // kinda hacky. if this physical device isn't chosen, this function will
	    // run again and overwrite m_indices, so it will still choose the
	    // correct queue family indices.
	    // TLDR; it works, it just doesn't look like it does.
	    FindQueueFamilies(device);

	    return
			    deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu
			    && m_indices.hasGraphicsFamily
			    && extensionsSupported;
    }

    vk::PresentModeKHR VulkanDriver::PickPresentMode(const vk::SurfaceKHR surface, const bool vsync) const
    {
	    const std::vector<vk::PresentModeKHR> presentModes =
			    m_physicalDevice.getSurfacePresentModesKHR(surface);

	    // Fifo is used for vsync. Fifo relaxed might work too.
	    // An option to use fifo relaxed may be added later.
	    if (vsync)
		    return vk::PresentModeKHR::eFifo;

	    // Mailbox is preferred for vsync disabled, but immediate works too.
	    // If neither are supported, fifo is used.

	    bool immediateSupported = false;
	    for (const auto& presentMode : presentModes)
	    {
		    if (presentMode == vk::PresentModeKHR::eMailbox)
			    return presentMode;

		    if (presentMode == vk::PresentModeKHR::eImmediate)
			    immediateSupported = true;
	    }

	    if (immediateSupported)
		    return vk::PresentModeKHR::eImmediate;

	    return vk::PresentModeKHR::eFifo;
    }

	bool VulkanDriver::CheckDeviceExtensionSupport(const vk::PhysicalDevice device,
	                                               const char* const* deviceExtensions, const uint64_t extensionCount)
	{
		// The device requires some extensions (such as the swap chain)
		// We need to check for those here before we can use the device.

		uint32_t count;
		LE_CHECK_RESULT(device.enumerateDeviceExtensionProperties(nullptr, &count, nullptr));

		std::vector<vk::ExtensionProperties> availableExtensions(count);
		LE_CHECK_RESULT(device.enumerateDeviceExtensionProperties(nullptr, &count,
			availableExtensions.data()));

		std::vector<std::string> requiredExtensions(extensionCount);
		for (uint64_t i = 0; i < extensionCount; i++)
			requiredExtensions[i] = std::string(deviceExtensions[i]);

		std::set requiredExtensionSet(requiredExtensions.begin(),
		                              requiredExtensions.end());

		for (auto& availableExtension : availableExtensions)
			requiredExtensionSet.erase(availableExtension.extensionName);

		return requiredExtensionSet.empty();
	}

	bool VulkanDriver::IsValidationSupported()
	{
		uint32_t layerCount = 0;
		LE_CHECK_RESULT(vk::enumerateInstanceLayerProperties(&layerCount, nullptr));

		std::vector<vk::LayerProperties> availableLayers(layerCount);
		LE_CHECK_RESULT(vk::enumerateInstanceLayerProperties(&layerCount, availableLayers.data()));

		for (const char* layerName : VALIDATION_LAYERS)
		{
			bool layerFound = false;
			for (const auto& layerProperties : availableLayers)
				if (std::strcmp(layerName, layerProperties.layerName) == 0)
				{
					layerFound = true;
					break;
				}

			if (!layerFound)
				return false;
		}

		return true;
	}
}

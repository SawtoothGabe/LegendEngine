#include "VulkanDriver.hpp"

#include <set>
#include <vk_mem_alloc.h>
#include <VulkanBuffer.hpp>
#include <VulkanImage.hpp>
#include <VulkanTypes.hpp>

#include <LE/Graphics/Explicit/ExplicitRenderer.hpp>
#include "PlatformUtils.hpp"

#include "VkDefs.hpp"

namespace le
{
#define VULKAN_CAST(type, identifier) vk::type(reinterpret_cast<Vk##type>(identifier.id))

    Scope<Renderer> CreateVulkanRenderer(std::string_view applicationName)
    {
        return std::make_unique<ExplicitRenderer>(std::make_unique<VulkanDriver>(applicationName));
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
		    ids.push_back(CommandBufferID(buffer));

	    return ids;
    }

    std::vector<DescriptorSetID> VulkanDriver::AllocateDescriptorSets()
    {
	    // TODO: pools

	    std::vector<DescriptorSetID> ids;
	    const std::vector<vk::DescriptorSet> sets
			    = m_device.allocateDescriptorSets({});

	    ids.reserve(sets.size());
	    for (vk::DescriptorSet set : sets)
		    ids.push_back(DescriptorSetID(set));

	    return ids;
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
	    const vk::CommandPool pool = m_device.createCommandPool(info, nullptr);
	    return CommandPoolID(pool);
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
	    imageInfo.extent.width = info.width;
	    imageInfo.extent.height = info.height;
	    imageInfo.extent.depth = 1;
	    imageInfo.mipLevels = 1;
	    imageInfo.arrayLayers = info.arrayLayers;
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
    	std::vector<vk::Format> formats;
    	std::vector<vk::PipelineShaderStageCreateInfo> stages;
    	std::vector<vk::VertexInputBindingDescription> bindings;
    	std::vector<vk::VertexInputAttributeDescription> attributes;
    	formats.reserve(info.colorAttachmentFormats.size());
    	stages.reserve(info.stages.size());
    	bindings.reserve(info.vertexBindings.size());
    	attributes.reserve(info.vertexAttributes.size());

    	for (const Format format : info.colorAttachmentFormats)
    		formats.push_back(VulkanTypes::GetVkFormat(format));

    	for (const auto [name, module, stage] : info.stages)
    		stages.push_back({{}, VulkanTypes::GetShaderStageFlag(stage),
    			VULKAN_CAST(ShaderModule, module), name.c_str()});

    	for (auto [binding, stride, inputRate] : info.vertexBindings)
    	{
    		bindings.push_back({
    			static_cast<uint32_t>(binding),
    			static_cast<uint32_t>(stride),
    			VulkanTypes::GetVertexInputRate(inputRate)
    		});
    	}

    	for (auto [location, binding, offset, format] : info.vertexAttributes)
    	{
    		attributes.push_back({
    			static_cast<uint32_t>(location),
    			static_cast<uint32_t>(binding),
    			VulkanTypes::GetVkFormat(format),
    			static_cast<uint32_t>(offset)
    		});
    	}

    	const vk::PipelineVertexInputStateCreateInfo vertexInput(
			{},
			bindings.size(), bindings.data(),
			attributes.size(), attributes.data()
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
    		{}, stages.size(), stages.data(), &vertexInput, &inputAssembly,
    		nullptr, &viewportState, &rasterizerState, &multisampleState,
    		&depthStencilState, &colorBlendState, &dynamicState,
    		VULKAN_CAST(PipelineLayout, info.layout), nullptr, 0,
    		nullptr, 0, &rendering
    	);

		return PipelineID(m_device.createGraphicsPipeline({}, createInfo).value);
    }

    PipelineLayoutID VulkanDriver::CreatePipelineLayout(std::span<PushConstantRange> ranges,
    	const std::span<DescriptorSetLayoutID> layouts)
    {
    	std::vector<vk::PushConstantRange> vkRanges;
    	std::vector<vk::DescriptorSetLayout> vkLayouts;
    	vkRanges.reserve(ranges.size());
    	vkLayouts.reserve(layouts.size());

    	for (auto [size, offset, stage] : ranges)
    	{
    		vk::ShaderStageFlags vkStage;
    		switch (stage)
    		{
    			case ShaderStageFlags::VERTEX: vkStage = vk::ShaderStageFlagBits::eVertex; break;
			    case ShaderStageFlags::FRAGMENT: vkStage = vk::ShaderStageFlagBits::eFragment; break;
			    case ShaderStageFlags::ALL: vkStage = vk::ShaderStageFlagBits::eAll; break;
		    }

    		vkRanges.push_back({
    			vkStage,
    			static_cast<uint32_t>(offset),
    			static_cast<uint32_t>(size)
    		});
    	}

    	for (const DescriptorSetLayoutID setLayout : layouts)
    		vkLayouts.push_back(VULKAN_CAST(DescriptorSetLayout, setLayout));

    	const vk::PipelineLayoutCreateInfo createInfo(
    		{},
    		vkLayouts.size(), vkLayouts.data(),
    		vkRanges.size(), vkRanges.data()
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

    ShaderModuleID VulkanDriver::CreateShaderModule() {}
    DescriptorSetLayoutID VulkanDriver::CreateDescriptorSetLayout() {}
    SamplerID VulkanDriver::CreateSampler() {}
    void VulkanDriver::FreeCommandBuffers() {}
    void VulkanDriver::FreeDescriptorSets() {}
    void VulkanDriver::DestroyBuffer(BufferID buffer) {}

    void VulkanDriver::DestroyCommandPool(CommandPoolID pool)
    {

    }

    void VulkanDriver::DestroyFence(FenceID fence) {}
    void VulkanDriver::DestroyImage(ImageID image) {}

    void VulkanDriver::DestroyImageView(ImageViewID view) {}

    void VulkanDriver::DestroyPipeline(PipelineID pipeline) {}
    void VulkanDriver::DestroySemaphore(SemaphoreID semaphore) {}
    void VulkanDriver::DestroySwapchain(SwapchainID swapchain) {}
    void VulkanDriver::DestroySurface(SurfaceID surface) {}
    void VulkanDriver::DestroyShaderModule(ShaderModuleID shaderModule) {}
    void VulkanDriver::DestroyDescriptorSetLayout(DescriptorSetLayoutID layout) {}
    void VulkanDriver::DestroySampler(SamplerID sampler) {}
    void VulkanDriver::WaitForFences(const size_t count, FenceID* fences)
    {
	    for (size_t i = 0; i < count; i++)
	    {
		    const auto fence = VULKAN_CAST(Fence, fences[i]);
		    LE_CHECK_RESULT(m_device.waitForFences(1, &fence, true, UINT64_MAX));
	    }
    }

    void VulkanDriver::WaitIdle()
    {
	    m_device.waitIdle();
    }

    void VulkanDriver::ResetFences(size_t count, uint64_t* fences) {}

    void VulkanDriver::QueueSubmit() {}

    void VulkanDriver::QueuePresent() {}

    void VulkanDriver::ResetCommandBuffer(CommandBufferID buffer) {}
    void VulkanDriver::BeginCommandBuffer(CommandBufferID buffer) {}
    void VulkanDriver::EndCommandBuffer(CommandBufferID buffer) {}
    void VulkanDriver::CmdCopyBuffer(CommandBufferID buffer) {}
    void VulkanDriver::CmdCopyBufferToImage(CommandBufferID buffer) {}
    void VulkanDriver::CmdPipelineBarrier(CommandBufferID buffer) {}

    void VulkanDriver::CmdBeginRendering(CommandBufferID buffer) {}
    void VulkanDriver::CmdSetViewport(CommandBufferID buffer) {}
    void VulkanDriver::CmdSetScissor(CommandBufferID buffer) {}
    void VulkanDriver::CmdBindPipeline(CommandBufferID buffer) {}
    void VulkanDriver::CmdSetCullMode(CommandBufferID buffer) {}
    void VulkanDriver::CmdPushConstants(CommandBufferID buffer) {}
    void VulkanDriver::CmdBindDescriptorSets(CommandBufferID buffer) {}
    void VulkanDriver::CmdBindVertexBuffers(CommandBufferID buffer) {}
    void VulkanDriver::CmdBindIndexBuffer(CommandBufferID buffer) {}
    void VulkanDriver::CmdDrawIndexed(CommandBufferID buffer) {}
    void VulkanDriver::CmdEndRendering(CommandBufferID buffer) {}

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
		    VALIDATION_LAYERS.size(), VALIDATION_LAYERS.data(),
#else
		    0, nullptr,
#endif
		    extensions.size(), extensions.data()
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

	    float priority = 1.0f;
	    std::vector<vk::DeviceQueueCreateInfo> queueInfos;

	    LE_ASSERT(m_indices.hasGraphicsFamily, "No graphics family found");

	    queueInfos.push_back(vk::DeviceQueueCreateInfo({}, m_indices.graphicsFamilyIndex,
	                                                   1, &priority));

	    if (m_indices.hasTransferFamily)
		    queueInfos.push_back(vk::DeviceQueueCreateInfo({}, m_indices.transferFamilyIndex,
		                                                   1, &priority));

	    const vk::DeviceCreateInfo deviceInfo({},
	                                          queueInfos.size(), queueInfos.data());
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

	    for (const VkPhysicalDevice device : devices)
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
}

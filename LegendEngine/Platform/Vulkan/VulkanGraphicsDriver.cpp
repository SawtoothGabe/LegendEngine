#include "VulkanGraphicsDriver.hpp"

#include <set>
#include <vk_mem_alloc.h>
#include <VulkanBuffer.hpp>
#include <VulkanImage.hpp>

#include <LE/Graphics/Explicit/ExplicitRenderer.hpp>
#include "InstanceUtils.hpp"

namespace le
{
#define VULKAN_CAST(type, identifier) vk::type(reinterpret_cast<Vk##type>(identifier.id))

    Scope<GraphicsDriver> CreateVulkanGraphicsDriver(std::string_view applicationName)
    {
        return std::make_unique<VulkanGraphicsDriver>(applicationName);
    }

    static const std::vector VALIDATION_LAYERS = {
        "VK_LAYER_KHRONOS_validation"
    };

	static const std::vector DEVICE_EXTENSIONS =
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

    static VKAPI_ATTR vk::Bool32 VKAPI_CALL OnValidationMessage(
        vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
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

    VulkanGraphicsDriver::VulkanGraphicsDriver(const std::string_view applicationName)
    {
	    VULKAN_HPP_DEFAULT_DISPATCHER.init();

	    CreateInstance(applicationName);
	    CreateDevice();
	    CreateAllocator();
    }

    VulkanGraphicsDriver::~VulkanGraphicsDriver()
    {
	    vmaDestroyAllocator(m_allocator);
	    m_device.destroy();
	    m_instance.destroy();
    }

    Scope<Renderer> VulkanGraphicsDriver::CreateRenderer(CommandPoolID pool)
    {
	    return std::make_unique<ExplicitRenderer>(*this, pool);
    }

    std::vector<CommandBufferID> VulkanGraphicsDriver::AllocateCommandBuffers(const CommandPoolID pool)
    {
	    std::vector<CommandBufferID> ids;
	    const std::vector<vk::CommandBuffer> buffers
			    = m_device.allocateCommandBuffers({VULKAN_CAST(CommandPool, pool)});

	    ids.reserve(buffers.size());
	    for (vk::CommandBuffer buffer : buffers)
		    ids.push_back(CommandBufferID(buffer));

	    return ids;
    }

    std::vector<DescriptorSetID> VulkanGraphicsDriver::AllocateDescriptorSets()
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

    BufferID VulkanGraphicsDriver::CreateBuffer(BufferUsageFlags flags, const std::size_t size, const bool createMapped)
    {
	    VkBufferCreateInfo bufferInfo{};
	    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	    bufferInfo.size = size;
	    bufferInfo.usage = static_cast<uint32_t>(flags);

	    VmaAllocationCreateInfo allocInfo{};
	    allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
	    allocInfo.flags = VulkanBuffer::ToVmaFlags(flags, createMapped);

	    const auto buffer = new VulkanBuffer;

	    LE_CHECK_VK(vmaCreateBuffer(m_allocator, &bufferInfo, &allocInfo,
		    &buffer->buffer, &buffer->allocation, &buffer->allocationInfo));
	    return BufferID(buffer);
    }

    CommandPoolID VulkanGraphicsDriver::CreateCommandPool(const QueueFamily family)
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

    FenceID VulkanGraphicsDriver::CreateFence(const bool signaled)
    {
	    vk::FenceCreateFlags flags = signaled ? vk::FenceCreateFlagBits::eSignaled : vk::FenceCreateFlags();
	    return FenceID(m_device.createFence({flags}));
    }

    ImageID VulkanGraphicsDriver::CreateImage(const ImageInfo& info)
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
	    imageInfo.format = VulkanImage::GetFormat(info.format);

	    VmaAllocationCreateInfo allocInfo{};
	    allocInfo.usage = VMA_MEMORY_USAGE_AUTO;

	    const auto image = new VulkanImage;

	    LE_CHECK_VK(vmaCreateImage(m_allocator, &imageInfo, &allocInfo,
		    &image->image, &image->allocation, nullptr));

	    return ImageID(image);
    }

    ImageViewID VulkanGraphicsDriver::CreateImageView(const ImageID image, const Format format, const ImageViewType type)
    {
	    vk::ImageViewType viewType = {};
	    switch (type)
	    {
		    case ImageViewType::TYPE_2D: viewType = vk::ImageViewType::e2D; break;
		    case ImageViewType::TYPE_2D_ARRAY: viewType = vk::ImageViewType::e2DArray; break;

		    default: LE_ASSERT(false, "Unknown image view type");
	    }

	    const auto vkFormat =
			    static_cast<vk::Format>(VulkanImage::GetFormat(format));

	    const vk::ImageView view = m_device.createImageView(
		    {{}, VULKAN_CAST(Image, image), viewType, vkFormat});
	    return ImageViewID(view);
    }

    PipelineID VulkanGraphicsDriver::CreatePipeline() {}

    SemaphoreID VulkanGraphicsDriver::CreateSemaphore() {}

    SwapchainID VulkanGraphicsDriver::CreateSwapchain() {}

    SurfaceID VulkanGraphicsDriver::CreateSurface() {}
    ShaderModuleID VulkanGraphicsDriver::CreateShaderModule() {}
    DescriptorSetLayoutID VulkanGraphicsDriver::CreateDescriptorSetLayout() {}
    SamplerID VulkanGraphicsDriver::CreateSampler() {}
    void VulkanGraphicsDriver::FreeCommandBuffers() {}
    void VulkanGraphicsDriver::FreeDescriptorSets() {}
    void VulkanGraphicsDriver::DestroyBuffer(BufferID buffer) {}

    void VulkanGraphicsDriver::DestroyCommandPool(CommandPoolID pool)
    {

    }
	
    void VulkanGraphicsDriver::DestroyFence(FenceID fence) {}
    void VulkanGraphicsDriver::DestroyImage(ImageID image) {}

    void VulkanGraphicsDriver::DestroyImageView(ImageViewID view) {}

    void VulkanGraphicsDriver::DestroyPipeline(PipelineID pipeline) {}
    void VulkanGraphicsDriver::DestroySemaphore(SemaphoreID semaphore) {}
    void VulkanGraphicsDriver::DestroySwapchain(SwapchainID swapchain) {}
    void VulkanGraphicsDriver::DestroySurface(SurfaceID surface) {}
    void VulkanGraphicsDriver::DestroyShaderModule(ShaderModuleID shaderModule) {}
    void VulkanGraphicsDriver::DestroyDescriptorSetLayout(DescriptorSetLayoutID layout) {}
    void VulkanGraphicsDriver::DestroySampler(SamplerID sampler) {}
    void VulkanGraphicsDriver::WaitForFences(const size_t count, FenceID* fences)
    {
	    for (size_t i = 0; i < count; i++)
	    {
		    const auto fence = VULKAN_CAST(Fence, fences[i]);
		    LE_CHECK_RESULT(m_device.waitForFences(1, &fence, true, UINT64_MAX));
	    }
    }

    void VulkanGraphicsDriver::WaitIdle()
    {
	    m_device.waitIdle();
    }

    void VulkanGraphicsDriver::ResetFences(size_t count, uint64_t* fences) {}

    void VulkanGraphicsDriver::QueueSubmit() {}

    void VulkanGraphicsDriver::QueuePresent() {}

    void VulkanGraphicsDriver::ResetCommandBuffer(CommandBufferID buffer) {}
    void VulkanGraphicsDriver::BeginCommandBuffer(CommandBufferID buffer) {}
    void VulkanGraphicsDriver::EndCommandBuffer(CommandBufferID buffer) {}
    void VulkanGraphicsDriver::CmdCopyBuffer(CommandBufferID buffer) {}
    void VulkanGraphicsDriver::CmdCopyBufferToImage(CommandBufferID buffer) {}
    void VulkanGraphicsDriver::CmdPipelineBarrier(CommandBufferID buffer) {}

    void VulkanGraphicsDriver::CmdBeginRendering(CommandBufferID buffer) {}
    void VulkanGraphicsDriver::CmdSetViewport(CommandBufferID buffer) {}
    void VulkanGraphicsDriver::CmdSetScissor(CommandBufferID buffer) {}
    void VulkanGraphicsDriver::CmdBindPipeline(CommandBufferID buffer) {}
    void VulkanGraphicsDriver::CmdSetCullMode(CommandBufferID buffer) {}
    void VulkanGraphicsDriver::CmdPushConstants(CommandBufferID buffer) {}
    void VulkanGraphicsDriver::CmdBindDescriptorSets(CommandBufferID buffer) {}
    void VulkanGraphicsDriver::CmdBindVertexBuffers(CommandBufferID buffer) {}
    void VulkanGraphicsDriver::CmdBindIndexBuffer(CommandBufferID buffer) {}
    void VulkanGraphicsDriver::CmdDrawIndexed(CommandBufferID buffer) {}
    void VulkanGraphicsDriver::CmdEndRendering(CommandBufferID buffer) {}
    void VulkanGraphicsDriver::CreateInstance(const std::string_view applicationName)
    {
	    const vk::ApplicationInfo appInfo(
		    applicationName.data(),
		    1,
		    "LegendEngine",
		    1,
		    VK_API_VERSION_1_3
	    );

	    LE_ASSERT(IsValidationSupported(), "Validation layers not supported");

	    const std::vector<const char*> extensions = InstanceUtils::GetRequiredExtensions();

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
	    VULKAN_HPP_DEFAULT_DISPATCHER.init(m_instance);

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
    void VulkanGraphicsDriver::CreateDevice()
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
    void VulkanGraphicsDriver::CreateAllocator()
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

    vk::PhysicalDevice VulkanGraphicsDriver::PickDevice()
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

    void VulkanGraphicsDriver::FindQueueFamilies(const vk::PhysicalDevice device)
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

    bool VulkanGraphicsDriver::IsDeviceSuitable(const vk::PhysicalDevice device)
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

    bool VulkanGraphicsDriver::CheckDeviceExtensionSupport(const vk::PhysicalDevice device,
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

	bool VulkanGraphicsDriver::IsValidationSupported()
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

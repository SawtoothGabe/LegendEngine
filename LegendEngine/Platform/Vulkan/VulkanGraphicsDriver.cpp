#include "VulkanGraphicsDriver.hpp"

#include <set>

#include "InstanceUtils.hpp"
#include <VkDefs.hpp>

namespace le
{
    Scope<GraphicsDriver> CreateVulkanGraphicsDriver(std::string_view applicationName)
    {
        return std::make_unique<VulkanGraphicsDriver>(applicationName);
    }

    const std::vector VALIDATION_LAYERS = {
        "VK_LAYER_KHRONOS_validation"
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
        CreateInstance(applicationName);
        CreateDevice();
    }

    VulkanGraphicsDriver::~VulkanGraphicsDriver()
    {
        m_device.destroy();
        m_instance.destroy();
    }

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
            1, VALIDATION_LAYERS.data(),
            extensions.size(), extensions.data()
        );
        m_instance = vk::createInstance(instanceCreateInfo);

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

    }

    Scope<Renderer> VulkanGraphicsDriver::CreateRenderer(CommandPoolID pool) {}
    void VulkanGraphicsDriver::AllocateCommandBuffers(CommandPoolID pool) {}
    void VulkanGraphicsDriver::AllocateDescriptorSets() {}
    BufferID VulkanGraphicsDriver::CreateBuffer(BufferUsageFlags flags, std::size_t size, bool createMapped) {}
    CommandPoolID VulkanGraphicsDriver::CreateCommandPool()
    {
        return CommandPoolID();
    }

    FenceID VulkanGraphicsDriver::CreateFence() {}
    ImageID VulkanGraphicsDriver::CreateImage() {}
    ImageViewID VulkanGraphicsDriver::CreateImageView() {}
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
    void VulkanGraphicsDriver::DestroyCommandPool(CommandPoolID pool) {}
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

    void VulkanGraphicsDriver::WaitForFences(size_t count, FenceID* fences) {}

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

    vk::PhysicalDevice VulkanGraphicsDriver::PickDevice()
    {
        uint32_t deviceCount = 0;
        LE_CHECK_RESULT(m_instance.enumeratePhysicalDevices(&deviceCount, nullptr));

        LE_ASSERT(deviceCount > 0, "No suitable device found");

        std::vector<vk::PhysicalDevice> devices(deviceCount);
        LE_CHECK_RESULT(m_instance.enumeratePhysicalDevices(&deviceCount, devices.data()));

        for (VkPhysicalDevice device : devices)
            if (IsDeviceSuitable(device))
                return device;

        LE_ASSERT(false, "No suitable device found");
        return {};
    }

    bool VulkanGraphicsDriver::IsDeviceSuitable(vk::PhysicalDevice device)
    {
        vk::PhysicalDeviceProperties deviceProperties;
        vk::PhysicalDeviceFeatures deviceFeatures;
        device.getProperties(&deviceProperties);
        device.getFeatures(&deviceFeatures);

        bool extensionsSupported = CheckDeviceExtensionSupport(device,
            deviceExtensions.data(), deviceExtensions.size());

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

		for (size_t i = 0; i < availableExtensions.size(); i++)
			requiredExtensionSet.erase(availableExtensions[i].extensionName);

		return requiredExtensionSet.empty();
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
			const VkQueueFamilyProperties& queueFamily = families[i];

			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT
				&& !m_indices.hasGraphicsFamily)
			{
				m_indices.hasGraphicsFamily = true;
				m_indices.graphicsFamilyIndex = static_cast<uint32_t>(i);
			}

			if (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT
				&& !m_indices.hasTransferFamily)
			{
				m_indices.hasTransferFamily = true;
				m_indices.transferFamilyIndex = static_cast<uint32_t>(i);
			}

			if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT
				&& !m_indices.hasComputeFamily)
			{
				m_indices.hasComputeFamily = true;
				m_indices.computeFamilyIndex = static_cast<uint32_t>(i);
			}
		}

		// Try to find a dedicated transfer queue
		for (size_t i = 0; i < families.size(); i++)
		{
			if (!(families[i].queueFlags & VK_QUEUE_TRANSFER_BIT))
				continue;

			if (m_indices.hasGraphicsFamily && m_indices.graphicsFamilyIndex == i)
				continue;

			m_indices.transferFamilyIndex = static_cast<uint32_t>(i);
			break;
		}
	}
}

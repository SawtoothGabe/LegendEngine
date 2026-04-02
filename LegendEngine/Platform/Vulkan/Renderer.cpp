#include "Renderer.hpp"
#include "GraphicsContext.hpp"

#include <VkDefs.hpp>
#include <LE/Application.hpp>
#include <LE/Components/Mesh.hpp>
#include <Tether/Rendering/Vulkan/SingleUseCommandBuffer.hpp>

#include <RenderTarget.hpp>
#include <API/Buffer.hpp>
#include <API/DynamicUniforms.hpp>
#include <API/Pipeline.hpp>

// #ifdef VMA_VULKAN_VERSION
// #undef VMA_VULKAN_VERSION
// #endif
//
// #define VMA_VULKAN_VERSION 1000000
// #define VMA_IMPLEMENTATION
// #include <vk_mem_alloc.h>

namespace le::vk
{
    Renderer::Renderer(
            GraphicsContext& context,
            le::RenderTarget& renderTarget,
            GraphicsResources& resources,
            const VkSurfaceFormatKHR surfaceFormat
            )
        :
        le::Renderer(renderTarget, context, resources),
        m_context(context),
        m_TetherCtx(context.GetTetherGraphicsContext()),
        m_DeviceLoader(m_TetherCtx.GetDeviceLoader()),
        m_Surface(dynamic_cast<RenderTarget&>(renderTarget).GetSurface()),
        m_SurfaceFormat(surfaceFormat),
        m_Device(m_TetherCtx.GetDevice()),
        m_PhysicalDevice(m_TetherCtx.GetPhysicalDevice()),
        m_DepthFormat(context.GetDepthFormat()),
        m_GraphicsQueueMutex(context.GetGraphicsQueueMutex())
    {
        LE_ASSERT(!dynamic_cast<RenderTarget&>(renderTarget).IsHeadless(),
            "Renderers can't be created with a headless surface");

        // Application::Get() doesn't work here

        m_DepthImages.resize(m_TetherCtx.GetFramesInFlight());
        m_DepthAllocs.resize(m_TetherCtx.GetFramesInFlight());
        m_DepthImageViews.resize(m_TetherCtx.GetFramesInFlight());

        const TetherVulkan::SwapchainDetails details = QuerySwapchainSupport();

        CreateSwapchain(details);
        CreateDepthImages();
        CreateCommandBuffers();
        CreateSyncObjects();
    }

    Renderer::~Renderer()
    {
        LE_DEBUG("Destroying renderer");

        vkDeviceWaitIdle(m_Device);

        DestroySwapchain();

        for (uint64_t i = 0; i < m_TetherCtx.GetFramesInFlight(); i++)
        {
            vkDestroySemaphore(m_Device, m_ImageAvailableSemaphores[i], nullptr);
            vkDestroyFence(m_Device, m_InFlightFences[i], nullptr);
        }

        for (uint32_t i = 0; i < m_SwapchainImageCount; i++)
        {
            vkDestroySemaphore(m_Device, m_RenderFinishedSemaphores[i], nullptr);
        }

        LE_DEBUG("Destroyed renderer");
    }

    void Renderer::SetVSyncEnabled(const bool vsync)
    {
        m_VSync = vsync;
        RecreateSwapchain();
    }

    void Renderer::NotifyWindowResized()
    {
        m_ShouldRecreateSwapchain = true;
    }

    bool Renderer::StartFrame()
    {
        if (m_ShouldRecreateSwapchain)
            RecreateSwapchain();

        m_CurrentFrame = Application::Get().GetCurrentFrame();

        // in flight frame = a frame that is being rendered while still rendering
        // more frames

        // If this frame is still in flight, wait for it to finish rendering before
        // rendering another frame.
        vkWaitForFences(m_Device, 1, &m_InFlightFences[m_CurrentFrame], true, UINT64_MAX);

        // The swapchain has one more than the minimum images, so the index
        // might not be the same as m_CurrentFrame
        const VkResult acquireResult = vkAcquireNextImageKHR(
            m_Device, m_Swapchain->Get(),
            UINT64_MAX,
            m_ImageAvailableSemaphores[m_CurrentFrame],
            VK_NULL_HANDLE,
            &m_CurrentImageIndex
        );
        if (acquireResult != VK_SUCCESS)
        {
            m_ShouldRecreateSwapchain = true;
            return false;
        }

        BeginCommandBuffer();

        return true;
    }

    void Renderer::BeginCommandBuffer()
    {
        const VkCommandBuffer buffer = m_CommandBuffers[m_CurrentFrame];
        const VkExtent2D swapchainExtent = m_Swapchain->GetExtent();

        vkResetCommandBuffer(buffer, 0);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        LE_CHECK_VK(vkBeginCommandBuffer(buffer, &beginInfo));

        VkImageMemoryBarrier colorBarrier{};
        colorBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        colorBarrier.image = m_SwapchainImages[m_CurrentImageIndex];
        colorBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        colorBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        colorBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

        vkCmdPipelineBarrier(buffer,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            0, 0, nullptr,
            0, nullptr,
            1, &colorBarrier);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(swapchainExtent.width);
        viewport.height = static_cast<float>(swapchainExtent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor{};
        scissor.offset.x = 0;
        scissor.offset.y = 0;
        scissor.extent.width = swapchainExtent.width;
        scissor.extent.height = swapchainExtent.height;

        VkRenderingAttachmentInfoKHR colorAttachmentInfo{};
        colorAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
        colorAttachmentInfo.imageView = m_SwapchainImageViews[m_CurrentImageIndex];
        colorAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        colorAttachmentInfo.clearValue =
            {
                m_ClearColor.x,
                m_ClearColor.y,
                m_ClearColor.z,
                m_ClearColor.w
            };
        colorAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

        VkRenderingAttachmentInfoKHR depthAttachmentInfo{};
        depthAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
        depthAttachmentInfo.imageView = m_DepthImageViews[m_CurrentFrame];
        depthAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        depthAttachmentInfo.clearValue = {1.0f, 0.0f };
        depthAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

        VkRenderingInfoKHR renderInfo{};
        renderInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
        renderInfo.colorAttachmentCount = 1;
        renderInfo.pColorAttachments = &colorAttachmentInfo;
        renderInfo.layerCount = 1;
        renderInfo.pDepthAttachment = &depthAttachmentInfo;
        renderInfo.pStencilAttachment = nullptr;
        renderInfo.renderArea.offset = { 0, 0 };
        renderInfo.renderArea.extent = swapchainExtent;

        m_DeviceLoader.vkCmdBeginRenderingKHR(buffer, &renderInfo);

        vkCmdSetViewport(buffer, 0, 1, &viewport);
        vkCmdSetScissor(buffer, 0, 1, &scissor);

        const auto& vkUniforms = static_cast<DynamicUniforms&>(*m_cameraUniforms);
        m_Sets[0] = vkUniforms.GetDescriptorSet();
        m_HaveSetsChanged = true;
        m_currentShaderID = 0;
    }

    void Renderer::BeginScene(Scene& scene)
    {
        const auto& vkUniforms = static_cast<DynamicUniforms&>(scene.GetUniforms());
        m_Sets[1] = vkUniforms.GetDescriptorSet();
    }

    void Renderer::UseMaterial(const Material& material, const Ref<Shader> shader)
    {
        const VkCommandBuffer buffer = m_CommandBuffers[m_CurrentFrame];
        const le::DynamicUniforms& dynamicUniforms = material.GetUniforms();
        const auto& vkUniforms = static_cast<const DynamicUniforms&>(
            dynamicUniforms);

        m_Sets[2] = vkUniforms.GetDescriptorSet();
        m_HaveSetsChanged = true;

        if (material.GetShader() == m_currentShaderID)
            return;

        const le::Pipeline& pipeline = shader->GetPipeline();
        const auto& vkPipeline = static_cast<const Pipeline&>(pipeline);

        VkCullModeFlags cullMode;
        switch (shader->GetCullMode())
        {
            case Shader::CullMode::BACK: cullMode = VK_CULL_MODE_BACK_BIT; break;
            case Shader::CullMode::FRONT: cullMode = VK_CULL_MODE_FRONT_BIT; break;
            default: cullMode = VK_CULL_MODE_NONE;
        }

        vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
            vkPipeline.Get());
        vkCmdSetCullMode(buffer, cullMode);

        m_currentPipelineLayout = vkPipeline.GetPipelineLayout();
        m_currentShaderID = material.GetShader();
    }

    void Renderer::DrawMesh(const Mesh& mesh, const Transform& transform, const Ref<MeshData> meshData)
    {
        auto& vertexBuffer = static_cast<Buffer&>(meshData->GetVertexBuffer());
        auto& indexBuffer  = static_cast<Buffer&>(meshData->GetIndexBuffer());

        if (!vertexBuffer.GetDesc().buffer || !indexBuffer.GetDesc().buffer)
            return;

        const VkCommandBuffer buffer = m_CommandBuffers[m_CurrentFrame];

        Pipeline::ObjectTransform objectTransform;
        objectTransform.transform = transform.transformMat;

        vkCmdPushConstants(buffer, m_currentPipelineLayout,
            VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(objectTransform),
            &objectTransform);

        if (m_HaveSetsChanged)
        {
            vkCmdBindDescriptorSets(
                buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                m_currentPipelineLayout,
                0, std::size(m_Sets),
                m_Sets,
                0, nullptr
            );
            m_HaveSetsChanged = false;
        }

        const VkBuffer vBuffers[] = { vertexBuffer.GetDesc().buffer };
        constexpr VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(buffer, 0, 1, vBuffers, offsets);

        vkCmdBindIndexBuffer(
            buffer,
            indexBuffer.GetDesc().buffer,
            0,
            VK_INDEX_TYPE_UINT32
        );

        vkCmdDrawIndexed(buffer, meshData->GetIndexCount(),
            1, 0, 0, 0);
    }

    void Renderer::EndCommandBuffer() const
    {
        const VkCommandBuffer buffer = m_CommandBuffers[m_CurrentFrame];

        vkCmdEndRendering(buffer);

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.image = m_SwapchainImages[m_CurrentImageIndex];
        barrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        barrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

        vkCmdPipelineBarrier(buffer,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
            0, 0,
            nullptr, 0,
            nullptr, 1, &barrier);
    }

    void Renderer::EndFrame()
    {
        const VkCommandBuffer buffer = m_CommandBuffers[m_CurrentFrame];

        EndCommandBuffer();

        const VkResult result = vkEndCommandBuffer(buffer);
        LE_ASSERT(result == VK_SUCCESS, "Failed to end command buffer");

        VkSemaphoreSubmitInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
        semaphoreInfo.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
        semaphoreInfo.semaphore = m_ImageAvailableSemaphores[m_CurrentFrame];

        VkSemaphoreSubmitInfo signalInfo{};
        signalInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
        signalInfo.semaphore = m_RenderFinishedSemaphores[m_CurrentImageIndex];

        VkCommandBufferSubmitInfo commandBufferInfo{};
        commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
        commandBufferInfo.commandBuffer = m_CommandBuffers[m_CurrentFrame];

        // Wait for the image to be available before rendering the frame and
        // signal the render finished semaphore once rendering is complete.
        VkSubmitInfo2 submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
        submitInfo.waitSemaphoreInfoCount = 1;
        submitInfo.pWaitSemaphoreInfos = &semaphoreInfo;
        submitInfo.commandBufferInfoCount = 1;
        submitInfo.pCommandBufferInfos = &commandBufferInfo;
        submitInfo.signalSemaphoreInfoCount = 1;
        submitInfo.pSignalSemaphoreInfos = &signalInfo;

        // The in flight fence for this frame must be reset.
        vkResetFences(m_Device, 1, &m_InFlightFences[m_CurrentFrame]);
        m_GraphicsQueueMutex.lock();
        LE_CHECK_VK(vkQueueSubmit2(m_TetherCtx.GetQueue(), 1, &submitInfo,
            m_InFlightFences[m_CurrentFrame]));
        m_GraphicsQueueMutex.unlock();

        // Wait for the frame to be rendered until presenting
        // (hence the wait semaphores being the signal semaphores)
        VkSwapchainKHR swapchains[] = { m_Swapchain->Get() };
        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &m_RenderFinishedSemaphores[m_CurrentImageIndex];
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapchains;
        presentInfo.pImageIndices = &m_CurrentImageIndex;

        m_GraphicsQueueMutex.lock();
        vkQueuePresentKHR(m_TetherCtx.GetQueue(), &presentInfo);
        m_GraphicsQueueMutex.unlock();
    }

    void Renderer::CreateSwapchain(const TetherVulkan::SwapchainDetails& details)
    {
        m_Swapchain.emplace(m_TetherCtx, details, m_SurfaceFormat,
            m_Surface, m_RenderTarget.GetWidth(), m_RenderTarget.GetHeight(),
            m_VSync);

        m_SwapchainImages = m_Swapchain->GetImages();
        m_SwapchainImageViews = m_Swapchain->CreateImageViews();
        m_SwapchainImageCount = m_Swapchain->GetImageCount();
    }

    void Renderer::CreateDepthImages()
    {
        auto [width, height] = m_Swapchain->GetExtent();

        TetherVulkan::SingleUseCommandBuffer cmdBuffer(m_TetherCtx);
        cmdBuffer.Begin();

        for (uint32_t i = 0; i < m_TetherCtx.GetFramesInFlight(); i++)
        {
            VkImageCreateInfo imageInfo{};
            imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            imageInfo.imageType = VK_IMAGE_TYPE_2D;
            imageInfo.extent.width = width;
            imageInfo.extent.height = height;
            imageInfo.extent.depth = 1;
            imageInfo.mipLevels = 1;
            imageInfo.arrayLayers = 1;
            imageInfo.format = m_DepthFormat;
            imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
            imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

            VmaAllocationCreateInfo allocInfo{};
            allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

            LE_CHECK_VK(vmaCreateImage(m_TetherCtx.GetAllocator(), &imageInfo, &allocInfo,
                &m_DepthImages[i], &m_DepthAllocs[i], nullptr));

            VkImageViewCreateInfo viewInfo{};
            viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.image = m_DepthImages[i];
            viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            viewInfo.format = m_DepthFormat;
            viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
            viewInfo.subresourceRange.baseMipLevel = 0;
            viewInfo.subresourceRange.levelCount = 1;
            viewInfo.subresourceRange.baseArrayLayer = 0;
            viewInfo.subresourceRange.layerCount = 1;

            LE_CHECK_VK(vkCreateImageView(m_Device, &viewInfo, nullptr,
                &m_DepthImageViews[i]));

            cmdBuffer.TransitionImageLayout(m_DepthImages[i], m_DepthFormat,
                VK_IMAGE_LAYOUT_UNDEFINED,
                VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                VK_IMAGE_ASPECT_DEPTH_BIT);
        }

        cmdBuffer.End();
        cmdBuffer.Submit();
    }

    void Renderer::CreateCommandBuffers()
    {
        m_CommandBuffers.resize(m_TetherCtx.GetFramesInFlight());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = m_TetherCtx.GetCommandPool();
        allocInfo.commandBufferCount = m_CommandBuffers.size();

        LE_CHECK_VK(vkAllocateCommandBuffers(m_Device, &allocInfo, m_CommandBuffers.data()));
    }

    void Renderer::CreateSyncObjects()
    {
        const uint32_t framesInFlight = m_TetherCtx.GetFramesInFlight();
        m_RenderFinishedSemaphores.resize(m_SwapchainImageCount);
        m_ImageAvailableSemaphores.resize(framesInFlight);
        m_InFlightFences.resize(framesInFlight);

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (uint32_t i = 0; i < framesInFlight; i++)
        {
            LE_CHECK_VK(vkCreateFence(m_Device, &fenceInfo, nullptr,
                &m_InFlightFences[i]));
            LE_CHECK_VK(vkCreateSemaphore(m_Device, &semaphoreInfo, nullptr,
                &m_ImageAvailableSemaphores[i]));
        }

        for (uint32_t i = 0; i < m_SwapchainImages.size(); i++)
        {
            LE_CHECK_VK(vkCreateSemaphore(m_Device, &semaphoreInfo, nullptr,
                &m_RenderFinishedSemaphores[i]));
        }
    }

    TetherVulkan::SwapchainDetails Renderer::QuerySwapchainSupport() const
    {
        TetherVulkan::SwapchainDetails details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
            m_PhysicalDevice,
            m_Surface,
            &details.capabilities
        );

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(
            m_PhysicalDevice,
            m_Surface,
            &formatCount,
            nullptr
        );

        if (formatCount != 0)
        {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(
                m_PhysicalDevice,
                m_Surface,
                &formatCount,
                details.formats.data()
            );
        }

        uint32_t presentModeCount = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            m_PhysicalDevice,
            m_Surface,
            &presentModeCount,
            nullptr
        );

        if (presentModeCount != 0)
        {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(
                m_PhysicalDevice,
                m_Surface,
                &presentModeCount,
                details.presentModes.data()
            );
        }

        return details;
    }

    VkFormat Renderer::FindSupportedFormat(const std::vector<VkFormat>& candidates,
        const VkImageTiling tiling, const VkFormatFeatureFlags features) const
    {
        for (const VkFormat format : candidates)
        {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(m_PhysicalDevice, format, &props);

            if (tiling == VK_IMAGE_TILING_LINEAR
                && (props.linearTilingFeatures & features) == features)
                return format;
            if (tiling == VK_IMAGE_TILING_OPTIMAL
                && (props.optimalTilingFeatures & features) == features)
                return format;
        }

        return candidates[0];
    }

    void Renderer::RecreateSwapchain()
    {
        // The m_Device might still have work. Wait for it to finish before
        // recreating the m_Swapchain->
        vkDeviceWaitIdle(m_Device);

        const TetherVulkan::SwapchainDetails details = QuerySwapchainSupport();

        if (details.capabilities.currentExtent.width == 0 ||
            details.capabilities.currentExtent.height == 0)
        {
            m_ShouldRecreateSwapchain = false;
            return;
        }

        DestroySwapchain();

        CreateSwapchain(details);
        CreateDepthImages();
        CreateCommandBuffers();

        m_ShouldRecreateSwapchain = false;
    }

    void Renderer::DestroySwapchain()
    {
        vkFreeCommandBuffers(m_Device, m_TetherCtx.GetCommandPool(),
            static_cast<uint32_t>(m_CommandBuffers.size()), m_CommandBuffers.data());

        for (const VkImageView imageView : m_SwapchainImageViews)
            vkDestroyImageView(m_Device, imageView, nullptr);

        for (uint32_t i = 0; i < m_TetherCtx.GetFramesInFlight(); i++)
        {
            vmaDestroyImage(m_TetherCtx.GetAllocator(), m_DepthImages[i], m_DepthAllocs[i]);
            vkDestroyImageView(m_Device, m_DepthImageViews[i], nullptr);
        }

        m_Swapchain.reset();
    }
}

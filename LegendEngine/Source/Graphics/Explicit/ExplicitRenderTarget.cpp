#include <LE/Graphics/Explicit/ExplicitRenderTarget.hpp>

namespace le
{
//     void Renderer::BeginCommandBuffer()
//     {
//         const VkCommandBuffer buffer = m_CommandBuffers[m_CurrentFrame];
//         const VkExtent2D swapchainExtent = m_Swapchain->GetExtent();
//
//         vkResetCommandBuffer(buffer, 0);
//
//         VkCommandBufferBeginInfo beginInfo{};
//         beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//         LE_CHECK_VK(vkBeginCommandBuffer(buffer, &beginInfo));
//
//         VkImageMemoryBarrier colorBarrier{};
//         colorBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
//         colorBarrier.image = m_SwapchainImages[m_CurrentImageIndex];
//         colorBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//         colorBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
//         colorBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
//         colorBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
//
//         vkCmdPipelineBarrier(buffer,
//             VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
//             VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
//             0, 0, nullptr,
//             0, nullptr,
//             1, &colorBarrier);
//
//         VkViewport viewport{};
//         viewport.x = 0.0f;
//         viewport.y = 0.0f;
//         viewport.width = static_cast<float>(swapchainExtent.width);
//         viewport.height = static_cast<float>(swapchainExtent.height);
//         viewport.minDepth = 0.0f;
//         viewport.maxDepth = 1.0f;
//
//         VkRect2D scissor{};
//         scissor.offset.x = 0;
//         scissor.offset.y = 0;
//         scissor.extent.width = swapchainExtent.width;
//         scissor.extent.height = swapchainExtent.height;
//
//         VkRenderingAttachmentInfoKHR colorAttachmentInfo{};
//         colorAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
//         colorAttachmentInfo.imageView = m_SwapchainImageViews[m_CurrentImageIndex];
//         colorAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
//         colorAttachmentInfo.clearValue =
//             {
//                 m_ClearColor.x,
//                 m_ClearColor.y,
//                 m_ClearColor.z,
//                 m_ClearColor.w
//             };
//         colorAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
//         colorAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
//
//         VkRenderingAttachmentInfoKHR depthAttachmentInfo{};
//         depthAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
//         depthAttachmentInfo.imageView = m_DepthImageViews[m_CurrentFrame];
//         depthAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
//         depthAttachmentInfo.clearValue = {1.0f, 0.0f };
//         depthAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
//         depthAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
//
//         VkRenderingInfoKHR renderInfo{};
//         renderInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
//         renderInfo.colorAttachmentCount = 1;
//         renderInfo.pColorAttachments = &colorAttachmentInfo;
//         renderInfo.layerCount = 1;
//         renderInfo.pDepthAttachment = &depthAttachmentInfo;
//         renderInfo.pStencilAttachment = nullptr;
//         renderInfo.renderArea.offset = { 0, 0 };
//         renderInfo.renderArea.extent = swapchainExtent;
//
//         m_DeviceLoader.vkCmdBeginRenderingKHR(buffer, &renderInfo);
//
//         vkCmdSetViewport(buffer, 0, 1, &viewport);
//         vkCmdSetScissor(buffer, 0, 1, &scissor);
//
//         const auto& vkUniforms = static_cast<DynamicUniforms&>(*m_cameraUniforms);
//         m_Sets[0] = vkUniforms.GetDescriptorSet();
//         m_HaveSetsChanged = true;
//         m_currentShaderID = 0;
//     }
//
//     void Renderer::EndCommandBuffer() const
//     {
//         const VkCommandBuffer buffer = m_CommandBuffers[m_CurrentFrame];
//
//         vkCmdEndRendering(buffer);
//
//         VkImageMemoryBarrier barrier{};
//         barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
//         barrier.image = m_SwapchainImages[m_CurrentImageIndex];
//         barrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
//         barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
//         barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
//         barrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
//
//         vkCmdPipelineBarrier(buffer,
//             VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
//             VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
//             0, 0,
//             nullptr, 0,
//             nullptr, 1, &barrier);
//     }
//
//     void Renderer::EndFrame()
//     {
//         const VkCommandBuffer buffer = m_CommandBuffers[m_CurrentFrame];
//
//         EndCommandBuffer();
//
//         const VkResult result = vkEndCommandBuffer(buffer);
//         LE_ASSERT(result == VK_SUCCESS, "Failed to end command buffer");
//
//         VkSemaphoreSubmitInfo semaphoreInfo{};
//         semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
//         semaphoreInfo.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
//         semaphoreInfo.semaphore = m_ImageAvailableSemaphores[m_CurrentFrame];
//
//         VkSemaphoreSubmitInfo signalInfo{};
//         signalInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
//         signalInfo.semaphore = m_RenderFinishedSemaphores[m_CurrentImageIndex];
//
//         VkCommandBufferSubmitInfo commandBufferInfo{};
//         commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
//         commandBufferInfo.commandBuffer = m_CommandBuffers[m_CurrentFrame];
//
//         // Wait for the image to be available before rendering the frame and
//         // signal the render finished semaphore once rendering is complete.
//         VkSubmitInfo2 submitInfo{};
//         submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
//         submitInfo.waitSemaphoreInfoCount = 1;
//         submitInfo.pWaitSemaphoreInfos = &semaphoreInfo;
//         submitInfo.commandBufferInfoCount = 1;
//         submitInfo.pCommandBufferInfos = &commandBufferInfo;
//         submitInfo.signalSemaphoreInfoCount = 1;
//         submitInfo.pSignalSemaphoreInfos = &signalInfo;
//
//         // The in flight fence for this frame must be reset.
//         vkResetFences(m_Device, 1, &m_InFlightFences[m_CurrentFrame]);
//         m_GraphicsQueueMutex.lock();
//         LE_CHECK_VK(vkQueueSubmit2(m_TetherCtx.GetQueue(), 1, &submitInfo,
//             m_InFlightFences[m_CurrentFrame]));
//         m_GraphicsQueueMutex.unlock();
//
//         // Wait for the frame to be rendered until presenting
//         // (hence the wait semaphores being the signal semaphores)
//         VkSwapchainKHR swapchains[] = { m_Swapchain->Get() };
//         VkPresentInfoKHR presentInfo{};
//         presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
//         presentInfo.waitSemaphoreCount = 1;
//         presentInfo.pWaitSemaphores = &m_RenderFinishedSemaphores[m_CurrentImageIndex];
//         presentInfo.swapchainCount = 1;
//         presentInfo.pSwapchains = swapchains;
//         presentInfo.pImageIndices = &m_CurrentImageIndex;
//
//         m_GraphicsQueueMutex.lock();
//         vkQueuePresentKHR(m_TetherCtx.GetQueue(), &presentInfo);
//         m_GraphicsQueueMutex.unlock();
//     }
//
//     void Renderer::CreateDepthImages()
//     {
//         auto [width, height] = m_Swapchain->GetExtent();
//
//         TetherVulkan::SingleUseCommandBuffer cmdBuffer(m_TetherCtx);
//         cmdBuffer.Begin();
//
//         for (uint32_t i = 0; i < m_TetherCtx.GetFramesInFlight(); i++)
//         {
//             VkImageCreateInfo imageInfo{};
//             imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
//             imageInfo.imageType = VK_IMAGE_TYPE_2D;
//             imageInfo.extent.width = width;
//             imageInfo.extent.height = height;
//             imageInfo.extent.depth = 1;
//             imageInfo.mipLevels = 1;
//             imageInfo.arrayLayers = 1;
//             imageInfo.format = m_DepthFormat;
//             imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
//             imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//             imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
//             imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
//             imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
//
//             VmaAllocationCreateInfo allocInfo{};
//             allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
//
//             LE_CHECK_VK(vmaCreateImage(m_TetherCtx.GetAllocator(), &imageInfo, &allocInfo,
//                 &m_DepthImages[i], &m_DepthAllocs[i], nullptr));
//
//             VkImageViewCreateInfo viewInfo{};
//             viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
//             viewInfo.image = m_DepthImages[i];
//             viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
//             viewInfo.format = m_DepthFormat;
//             viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
//             viewInfo.subresourceRange.baseMipLevel = 0;
//             viewInfo.subresourceRange.levelCount = 1;
//             viewInfo.subresourceRange.baseArrayLayer = 0;
//             viewInfo.subresourceRange.layerCount = 1;
//
//             LE_CHECK_VK(vkCreateImageView(m_Device, &viewInfo, nullptr,
//                 &m_DepthImageViews[i]));
//
//             cmdBuffer.TransitionImageLayout(m_DepthImages[i], m_DepthFormat,
//                 VK_IMAGE_LAYOUT_UNDEFINED,
//                 VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
//                 VK_IMAGE_ASPECT_DEPTH_BIT);
//         }
//
//         cmdBuffer.End();
//         cmdBuffer.Submit();
//     }
//
//     TetherVulkan::SwapchainDetails Renderer::QuerySwapchainSupport() const
//     {
//         TetherVulkan::SwapchainDetails details;
//
//         vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
//             m_PhysicalDevice,
//             m_Surface,
//             &details.capabilities
//         );
//
//         uint32_t formatCount;
//         vkGetPhysicalDeviceSurfaceFormatsKHR(
//             m_PhysicalDevice,
//             m_Surface,
//             &formatCount,
//             nullptr
//         );
//
//         if (formatCount != 0)
//         {
//             details.formats.resize(formatCount);
//             vkGetPhysicalDeviceSurfaceFormatsKHR(
//                 m_PhysicalDevice,
//                 m_Surface,
//                 &formatCount,
//                 details.formats.data()
//             );
//         }
//
//         uint32_t presentModeCount = 0;
//         vkGetPhysicalDeviceSurfacePresentModesKHR(
//             m_PhysicalDevice,
//             m_Surface,
//             &presentModeCount,
//             nullptr
//         );
//
//         if (presentModeCount != 0)
//         {
//             details.presentModes.resize(presentModeCount);
//             vkGetPhysicalDeviceSurfacePresentModesKHR(
//                 m_PhysicalDevice,
//                 m_Surface,
//                 &presentModeCount,
//                 details.presentModes.data()
//             );
//         }
//
//         return details;
//     }
//
//     VkFormat Renderer::FindSupportedFormat(const std::vector<VkFormat>& candidates,
//         const VkImageTiling tiling, const VkFormatFeatureFlags features) const
//     {
//         for (const VkFormat format : candidates)
//         {
//             VkFormatProperties props;
//             vkGetPhysicalDeviceFormatProperties(m_PhysicalDevice, format, &props);
//
//             if (tiling == VK_IMAGE_TILING_LINEAR
//                 && (props.linearTilingFeatures & features) == features)
//                 return format;
//             if (tiling == VK_IMAGE_TILING_OPTIMAL
//                 && (props.optimalTilingFeatures & features) == features)
//                 return format;
//         }
//
//         return candidates[0];
//     }
//
//     void Renderer::RecreateSwapchain()
//     {
//         // The m_Device might still have work. Wait for it to finish before
//         // recreating the m_Swapchain->
//         vkDeviceWaitIdle(m_Device);
//
//         const TetherVulkan::SwapchainDetails details = QuerySwapchainSupport();
//
//         if (details.capabilities.currentExtent.width == 0 ||
//             details.capabilities.currentExtent.height == 0)
//         {
//             m_ShouldRecreateSwapchain = false;
//             return;
//         }
//
//         DestroySwapchain();
//
//         CreateSwapchain(details);
//         CreateDepthImages();
//         CreateCommandBuffers();
//
//         m_ShouldRecreateSwapchain = false;
//     }
//
//     void Renderer::DestroySwapchain()
//     {
//         vkFreeCommandBuffers(m_Device, m_TetherCtx.GetCommandPool(),
//             static_cast<uint32_t>(m_CommandBuffers.size()), m_CommandBuffers.data());
//
//         for (const VkImageView imageView : m_SwapchainImageViews)
//             vkDestroyImageView(m_Device, imageView, nullptr);
//
//         for (uint32_t i = 0; i < m_TetherCtx.GetFramesInFlight(); i++)
//         {
//             vmaDestroyImage(m_TetherCtx.GetAllocator(), m_DepthImages[i], m_DepthAllocs[i]);
//             vkDestroyImageView(m_Device, m_DepthImageViews[i], nullptr);
//         }
//
//         m_Swapchain.reset();
//     }

    ExplicitRenderTarget::ExplicitRenderTarget(ExplicitDriver& driver, Window& window)
        :
        m_driver(driver)
    {
        m_surface = m_driver.CreateSurface(window);
    }

    ExplicitRenderTarget::~ExplicitRenderTarget()
    {
        m_driver.DestroySurface(m_surface);
    }
}

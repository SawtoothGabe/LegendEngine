#include <LE/Application.hpp>
#include <LE/Components/Camera.hpp>
#include <LE/Graphics/Explicit/ExplicitRenderer.hpp>
#include <LE/Graphics/Explicit/ExplicitRenderTarget.hpp>

namespace le
{
    ExplicitRenderTarget::ExplicitRenderTarget(ExplicitResources& resources, const Format colorFormat,
        const Format depthFormat, Window& window)
        :
        m_driver(resources.GetDriver()),
        m_cameraPoolManager(resources.GetCameraPoolManager()),
        m_window(window),
        m_mutex(resources.GetGraphicsMutex()),
        m_queue(resources.GetGraphicsQueue()),
        m_commandPool(resources.GetGraphicsPool()),
        m_colorFormat(colorFormat),
        m_depthFormat(depthFormat),
        m_cameraUniforms(resources, BufferUsageFlagBits::UNIFORM_BUFFER, sizeof(Camera::CameraUniforms))
    {
        m_frames.resize(Application::FRAMES_IN_FLIGHT);
        m_surface = m_driver.CreateSurface(window);

        CreateSwapchain(m_driver.GetSurfaceCapabilities(m_surface));
        CreateCameraUniforms();
        CreateDepthImages();
        CreateSemaphores();
    }

    ExplicitRenderTarget::~ExplicitRenderTarget()
    {
        m_driver.WaitIdle();

        m_driver.FreeDescriptorSets(m_cameraPoolManager, m_cameraPool,
            m_cameraSets.size(), m_cameraSets.data());

        DestroySwapchain();

        for (const PerFrameData& perFrameData : m_frames)
            m_driver.DestroySemaphore(perFrameData.imageAvailableSemaphore);

        m_driver.DestroySurface(m_surface);
    }

    void ExplicitRenderTarget::SetClearColor(const Color clearColor)
    {
        m_clearColor = clearColor;
    }

    void ExplicitRenderTarget::SetVSync(const bool vsync)
    {
        m_vsync = vsync;
    }

    void ExplicitRenderTarget::InvalidateSwapchain()
    {
        m_shouldRecreateSwapchain = true;
    }

    bool ExplicitRenderTarget::StartRendering(const CommandBufferID& c, const size_t currentFrame)
    {
        if (m_shouldRecreateSwapchain)
            RecreateSwapchain();

        if (!m_driver.AcquireNextImage(m_swapchain,
            m_frames[currentFrame].imageAvailableSemaphore, m_imageIndex))
        {
            m_shouldRecreateSwapchain = true;
            return false;
        }

        ImageMemoryBarrier colorBarrier;
        colorBarrier.image = m_images[m_imageIndex];
        colorBarrier.oldLayout = ImageLayout::UNDEFINED;
        colorBarrier.newLayout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL;
        colorBarrier.dstAccessMask = AccessFlagBits::COLOR_ATTACHMENT_WRITE;
        colorBarrier.subresourceRange = { ImageAspect::COLOR, 0, 0, 1 };

        m_driver.CmdPipelineBarrier(
            c,
            PipelineStage::TOP_OF_PIPE,
            PipelineStage::COLOR_ATTACHMENT_OUTPUT,
            std::span(&colorBarrier, 1)
        );

        RenderingAttachmentInfo colorAttachment;
        colorAttachment.imageView = m_imageViews[m_imageIndex];
        colorAttachment.imageLayout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL;
        colorAttachment.clearValue = m_clearColor;

        RenderingInfo renderingInfo;
        renderingInfo.colorAttachments = std::span(&colorAttachment, 1);
        renderingInfo.depthAttachment =
        {
            .imageView = m_frames[currentFrame].depthView,
            .imageLayout = ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            .clearValue = { 1.0f, 0.0f, 0.0f, 0.0f },
        };
        renderingInfo.extent = m_extent;

        m_driver.CmdBeginRendering(c, renderingInfo);
        m_driver.CmdSetViewport(c, m_extent);
        m_driver.CmdSetScissor(c, {{}, m_extent});

        return true;
    }

    void ExplicitRenderTarget::EndRendering(const CommandBufferID& c) const
    {
        m_driver.CmdEndRendering(c);

        ImageMemoryBarrier barrier;
        barrier.image = m_images[m_imageIndex];
        barrier.oldLayout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL;
        barrier.newLayout = ImageLayout::PRESENT_SRC;
        barrier.srcAccessMask = AccessFlagBits::COLOR_ATTACHMENT_WRITE;
        barrier.subresourceRange = { ImageAspect::COLOR, 0, 0, 1 };

        m_driver.CmdPipelineBarrier(c,
                                    PipelineStage::COLOR_ATTACHMENT_OUTPUT,
                                    PipelineStage::BOTTOM_OF_PIPE,
                                    std::span(&barrier, 1)
        );
    }

    void ExplicitRenderTarget::EndFrame(SemaphoreID waitSemaphore)
    {
        PresentInfo info;
        info.swapchains = std::span(&m_swapchain, 1);
        info.waitSemaphores = std::span(&waitSemaphore, 1);
        info.imageIndices = std::span(&m_imageIndex, 1);

        std::scoped_lock lock(m_mutex);
        m_driver.QueuePresent(m_queue, info);
    }

    void ExplicitRenderTarget::UpdateCameraUniforms(const size_t currentFrame, const Camera& camera) const
    {
        Camera::CameraUniforms uniforms;
        uniforms.cameraMatrix = camera.GetCameraMatrix();

        const BufferID buffer = m_cameraUniforms.GetDesc(currentFrame).buffer;
        void* data = m_driver.GetMappedBufferData(buffer);
        memcpy(data, &uniforms, sizeof(uniforms));
    }

    DescriptorSetID ExplicitRenderTarget::GetCameraSet(const size_t currentFrame)
    {
        return m_cameraSets[currentFrame];
    }

    SemaphoreID ExplicitRenderTarget::GetImageAvailableSemaphore(const size_t currentFrame)
    {
        return m_frames[currentFrame].imageAvailableSemaphore;
    }

    void ExplicitRenderTarget::CreateSurface()
    {
        m_surface = m_driver.CreateSurface(m_window);
    }

    void ExplicitRenderTarget::CreateSwapchain(const SurfaceCapabilities& capabilities)
    {
        SwapchainInfo info;
        info.format = m_colorFormat;
        info.extent = m_extent = capabilities.currentExtent;
        info.surface = m_surface;
        info.vsync = m_vsync;

        m_swapchain = m_driver.CreateSwapchain(info);
    }

    void ExplicitRenderTarget::CreateDepthImages()
    {
        CommandBufferID c = m_driver.AllocateCommandBuffer(m_commandPool);
        m_driver.BeginCommandBuffer(c, true);

        for (PerFrameData& frame : m_frames)
        {
            ImageInfo imageInfo;
            imageInfo.width = m_extent.width;
            imageInfo.height = m_extent.height;
            imageInfo.format = m_depthFormat;
            imageInfo.usage = ImageUsageFlagBits::DEPTH_STENCIL_ATTACHMENT;

            frame.depthImage = m_driver.CreateImage(imageInfo);

            ImageViewInfo viewInfo;
            viewInfo.image = frame.depthImage;
            viewInfo.format = imageInfo.format;
            viewInfo.subresourceRange.aspect = ImageAspect::DEPTH;

            frame.depthView = m_driver.CreateImageView(viewInfo);

            m_driver.TransitionImageLayout(c, frame.depthImage,
                ImageLayout::UNDEFINED,
                ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                ImageAspect::DEPTH
            );
        }
        m_driver.EndCommandBuffer(c);

        FenceID fence = m_driver.CreateFence(false);

        SubmitInfo info;
        info.commandBuffer = c;
        info.fence = fence;

        {
            std::scoped_lock lock(m_mutex);
            m_driver.QueueSubmit(m_queue, info);
        }

        m_driver.WaitForFences(1, &fence);
        m_driver.DestroyFence(fence);
        m_driver.FreeCommandBuffers(m_commandPool, 1, &c);
    }

    void ExplicitRenderTarget::CreateSemaphores()
    {
        for (PerFrameData& frame : m_frames)
            frame.imageAvailableSemaphore = m_driver.CreateSemaphore();
    }

    void ExplicitRenderTarget::CreateCameraUniforms()
    {
        m_cameraSets = m_driver.AllocateDescriptorSets(m_cameraPoolManager, m_cameraPool,
            Application::FRAMES_IN_FLIGHT);

        for (size_t i = 0; i < m_cameraSets.size(); ++i)
        {
            DescriptorBufferInfo bufferInfo {
                .buffer = m_cameraUniforms.GetDesc(i).buffer,
                .range = sizeof(Camera::CameraUniforms),
            };

            WriteDescriptorSet write {
                .dstSet = m_cameraSets[i],
                .pBufferInfo = &bufferInfo,
            };

            m_driver.UpdateDescriptorSets(std::span(&write, 1));
        }
    }

    void ExplicitRenderTarget::RecreateSwapchain()
    {
        // The m_Device might still have work. Wait for it to finish before recreating the swapchain.
        m_driver.WaitIdle();

        const SurfaceCapabilities capabilities = m_driver.GetSurfaceCapabilities(m_surface);
        if (capabilities.currentExtent.width == 0 || capabilities.currentExtent.height == 0)
        {
            m_shouldRecreateSwapchain = false;
            return;
        }

        DestroySwapchain();

        CreateSwapchain(capabilities);
        CreateDepthImages();

        m_shouldRecreateSwapchain = false;
    }

    void ExplicitRenderTarget::DestroySwapchain() const
    {
        for (const ImageViewID& view : m_imageViews)
            m_driver.DestroyImageView(view);

        for (const PerFrameData& data : m_frames)
        {
            m_driver.DestroyImage(data.depthImage);
            m_driver.DestroyImageView(data.depthView);
        }

        m_driver.DestroySwapchain(m_swapchain);
    }
}

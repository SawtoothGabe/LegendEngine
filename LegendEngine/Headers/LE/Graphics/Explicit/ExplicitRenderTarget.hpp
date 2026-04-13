#pragma once

#include <LE/Graphics/RenderTarget.hpp>
#include <LE/Graphics/Types.hpp>
#include <LE/Graphics/Explicit/ExplicitDriver.hpp>

namespace le
{
    class ExplicitRenderer;

    class ExplicitRenderTarget final : public RenderTarget
    {
    public:
        explicit ExplicitRenderTarget(const ExplicitRenderer& renderer, Format depthFormat, Window& window);
        ~ExplicitRenderTarget() override;

        void SetClearColor(Color clearColor);
        void SetVSync(bool vsync) override;
        void InvalidateSwapchain() override;

        void StartRendering(const CommandBufferID& c, size_t currentFrame) const;
        void EndRendering(const CommandBufferID& c, size_t currentFrame) const;
        void EndFrame(SemaphoreID waitSemaphore);

        DescriptorSetID GetCameraSet(size_t currentFrame);
        SemaphoreID GetImageAvailableSemaphore(size_t currentFrame);
    private:
        struct PerFrameData
        {
            ImageID depthImage;
            ImageViewID depthView;
            SemaphoreID imageAvailableSemaphore;
        };

        void CreateSurface();
        void CreateSwapchain(const SurfaceCapabilities& capabilities);
        void CreateDepthImages();
        void CreateSemaphores();
        void RecreateSwapchain();
        void DestroySwapchain() const;

        ExplicitDriver& m_driver;
        Window& m_window;

        std::mutex& m_mutex;
        QueueID m_queue;
        CommandPoolID m_commandPool;

        SurfaceID m_surface;
        SwapchainID m_swapchain;
        Extent2D m_extent;

        Format m_depthFormat;

        uint32_t m_imageIndex = 0;

        std::vector<PerFrameData> m_frames;
        std::vector<ImageID> m_images;
        std::vector<ImageViewID> m_imageViews;

        Color m_clearColor;
        bool m_vsync = false;
        bool m_shouldRecreateSwapchain = false;
    };
}

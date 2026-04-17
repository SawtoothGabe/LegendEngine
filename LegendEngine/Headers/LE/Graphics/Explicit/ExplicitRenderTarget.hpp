#pragma once

#include <LE/Components/Camera.hpp>
#include <LE/Graphics/RenderTarget.hpp>
#include <LE/Graphics/Types.hpp>
#include <LE/Graphics/Explicit/ExplicitDriver.hpp>
#include <LE/Graphics/Explicit/PerFrameBuffer.hpp>

namespace le
{
    class ExplicitRenderer;

    class ExplicitRenderTarget final : public RenderTarget
    {
    public:
        explicit ExplicitRenderTarget(ExplicitResources& resources, Format colorFormat, Format depthFormat, Window& window);
        ~ExplicitRenderTarget() override;

        void SetClearColor(Color clearColor);
        void SetVSync(bool vsync) override;
        void InvalidateSwapchain() override;

        bool StartRendering(const CommandBufferID& c, size_t currentFrame);
        void EndRendering(const CommandBufferID& c) const;
        void EndFrame();

        void UpdateCameraUniforms(size_t currentFrame, const Camera& camera) const;

        DescriptorSetID GetCameraSet(size_t currentFrame) const;
        SemaphoreID GetImageAvailableSemaphore(size_t currentFrame) const;
        SemaphoreID GetRenderFinishedSemaphore() const;
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
        void CreateCameraUniforms();
        void RecreateSwapchain();
        void DestroySwapchain() const;

        ExplicitDriver& m_driver;
        DescriptorPoolID m_cameraPool;
        PoolManagerID m_cameraPoolManager;
        Window& m_window;

        std::mutex& m_mutex;
        QueueID m_queue;
        CommandPoolID m_commandPool;

        SurfaceID m_surface;
        SwapchainID m_swapchain;
        Extent2D m_extent{};

        Format m_colorFormat;
        Format m_depthFormat;

        uint32_t m_imageIndex = 0;

        std::vector<PerFrameData> m_frames;
        std::vector<ImageID> m_images;
        std::vector<ImageViewID> m_imageViews;
        std::vector<SemaphoreID> m_renderFinishedSemaphores;

        std::vector<DescriptorSetID> m_cameraSets;
        PerFrameBuffer m_cameraUniforms;

        Color m_clearColor;
        bool m_vsync = false;
        bool m_shouldRecreateSwapchain = false;
    };
}

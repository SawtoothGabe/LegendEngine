#pragma once

#include <GraphicsContext.hpp>

#include "API/Pipeline.hpp"

#include <LE/Components/Light.hpp>
#include <LE/Graphics/Renderer.hpp>
#include <LE/Resources/ResourceManager.hpp>
#include <Tether/Rendering/Vulkan/DescriptorSet.hpp>

#include <Tether/Rendering/Vulkan/Swapchain.hpp>
#include <Tether/Rendering/Vulkan/UniformBuffer.hpp>
#include <Tether/Rendering/Vulkan/Resources/BufferedImage.hpp>

namespace le::vk
{
    namespace TetherVulkan = Tether::Rendering::Vulkan;

    class Renderer final : public le::Renderer
    {
    public:
        explicit Renderer(
            GraphicsContext& context,
            RenderTarget& renderTarget,
            GraphicsResources& resources,
            VkSurfaceFormatKHR surfaceFormat
        );
        ~Renderer() override;

        void SetVSyncEnabled(bool vsync) override;
        void NotifyWindowResized() override;
    private:
        struct LightUniforms
        {
            Color color;
            alignas(16) Vector3f position;
            Light::LightType type;
        };

        struct SceneUniforms
        {
            Color ambientLight;
            alignas(16) LightUniforms lights[8];
        };

        bool StartFrame() override;
        void BeginCommandBuffer();
        void BeginScene(Scene& scene) override;
        void UseMaterial(const Material& material, Ref<Shader> shader) override;
        void DrawMesh(const Mesh& mesh, const Transform& transform, Ref<MeshData> meshData) override;
        void EndCommandBuffer() const;
        void EndFrame() override;

        void CreateSwapchain(const TetherVulkan::SwapchainDetails& details);
        void CreateDepthImages();
        void CreateCommandBuffers();
        void CreateSyncObjects();

        TetherVulkan::SwapchainDetails QuerySwapchainSupport() const;
        [[nodiscard]] VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates,
            VkImageTiling tiling, VkFormatFeatureFlags features) const;

        void RecreateSwapchain();
        void DestroySwapchain();

        bool m_VSync = false;
        bool m_ShouldRecreateSwapchain = false;

        GraphicsContext& m_context;
        TetherVulkan::GraphicsContext& m_TetherCtx;
        const TetherVulkan::DeviceLoader& m_DeviceLoader;
        VkSurfaceKHR m_Surface;

        std::optional<TetherVulkan::Swapchain> m_Swapchain;

        std::vector<VkImage> m_DepthImages;
        std::vector<VmaAllocation> m_DepthAllocs;
        std::vector<VkImageView> m_DepthImageViews;

        uint32_t m_SwapchainImageCount = 0;
        std::vector<VkImage> m_SwapchainImages;
        std::vector<VkImageView> m_SwapchainImageViews;
        std::vector<VkCommandBuffer> m_CommandBuffers;
        std::vector<VkSemaphore> m_ImageAvailableSemaphores;
        std::vector<VkSemaphore> m_RenderFinishedSemaphores;
        std::vector<VkFence> m_InFlightFences;

        VkSurfaceFormatKHR m_SurfaceFormat;

        VkDevice m_Device;
        VkPhysicalDevice m_PhysicalDevice;

        uint32_t m_CurrentFrame = 0;
        uint32_t m_CurrentImageIndex = 0;

        VkFormat m_DepthFormat;

        VkDescriptorSet m_Sets[3] = {};
        bool m_HaveSetsChanged = true;
        Resource::ID<Shader> m_currentShaderID = 0;
        VkPipelineLayout m_currentPipelineLayout = nullptr;

        std::mutex& m_GraphicsQueueMutex;
    };
}

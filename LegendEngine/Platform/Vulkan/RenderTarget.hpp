#pragma once

#include <LE/Common/Defs.hpp>
#include <LE/Graphics/RenderTarget.hpp>
#include <Tether/Window.hpp>
#include <Tether/Rendering/Vulkan/Surface.hpp>

#include <vulkan/vulkan.h>

namespace le::vk
{
    namespace TetherVulkan = Tether::Rendering::Vulkan;

    class RenderTarget final : public le::RenderTarget
    {
    public:
        explicit RenderTarget(const TetherVulkan::GraphicsContext& context); // Headless
        RenderTarget(const TetherVulkan::GraphicsContext& context, Tether::Window& window);
        LE_NO_COPY(RenderTarget);

        VkSurfaceKHR GetSurface();
        [[nodiscard]] bool IsHeadless() const;
    private:
        TetherVulkan::Surface m_Surface;
        bool m_IsHeadless;
    };
}

#include "RenderTarget.hpp"

namespace le::vk
{
    RenderTarget::RenderTarget(const TetherVulkan::GraphicsContext& context)
        :
        m_Surface(context),
        m_IsHeadless(true)
    {}

#ifndef LE_HEADLESS
    RenderTarget::RenderTarget(const TetherVulkan::GraphicsContext& context,
        Tether::Window& window)
        :
        m_Surface(context, window),
        m_IsHeadless(false)
    {}
#endif

    bool RenderTarget::IsHeadless() const
    {
        return m_IsHeadless;
    }

    VkSurfaceKHR RenderTarget::GetSurface()
    {
        return m_Surface.Get();
    }
}

#pragma once

#include <vector>
#include <LE/TetherBindings.hpp>

#include "VkDefs.hpp"

namespace le::PlatformUtils
{
    void LoadVulkanFuncs(vk::Instance instance);
    std::vector<const char*> GetRequiredExtensions();
    vk::SurfaceKHR CreateSurface(vk::Instance instance, Window& window);
}

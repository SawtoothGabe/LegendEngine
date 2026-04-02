#pragma once

#include <LE/Common/Assert.hpp>
#include <vulkan/vulkan.h>

#ifdef NDEBUG
#define LE_CHECK_VK(func) func
#else
#define LE_CHECK_VK(func) do { VkResult leCheckVkResult = (func); LE_ASSERT(leCheckVkResult == VK_SUCCESS, "Vulkan function failed. Result = {}", (int)leCheckVkResult); } while (0)
#define LE_CHECK_VK_MSG(func, ...) LE_ASSERT(func == VK_SUCCESS, __VA_ARGS__)
#endif
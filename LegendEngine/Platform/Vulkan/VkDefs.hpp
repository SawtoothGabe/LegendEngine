#pragma once

#include <LE/Common/Assert.hpp>

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include <vulkan/vulkan.hpp>

#ifdef NDEBUG
#define LE_CHECK_VK(func) func
#else
#define LE_CHECK_VK(func) do { VkResult leCheckVkResult = (func); LE_ASSERT(leCheckVkResult == VK_SUCCESS, #func" failed. Result = {}", (int)leCheckVkResult); } while (0)
#define LE_CHECK_RESULT(func) do { vk::Result leCheckVkResult = (func); LE_ASSERT(leCheckVkResult == vk::Result::eSuccess, #func" failed. Result = {}", (int)leCheckVkResult); } while (0)
#define LE_CHECK_VK_MSG(func, ...) LE_ASSERT(func == VK_SUCCESS, __VA_ARGS__)
#endif
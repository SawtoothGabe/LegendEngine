#include "InstanceUtils.hpp"
#include <LE/Common/Platform.hpp>

#include <vulkan/vulkan.h>

#if defined(LE_PLATFORM_LINUX) && !defined(LE_HEADLESS)
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/XKBlib.h>

#include <vulkan/vulkan_xlib.h>
#elif defined(LE_PLATFORM_WINDOWS)
#ifndef UNICODE
#define UNICODE
#endif

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <vulkan/vulkan_win32.h>
#endif

namespace le
{
    std::vector<const char*> InstanceUtils::GetRequiredExtensions()
    {
        std::vector<const char*> extensions;
        extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);

#ifndef LE_HEADLESS
#ifdef LE_PLATFORM_WINDOWS
        extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined(LE_PLATFORM_LINUX)
        extensions.push_back(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
#endif
#endif

#ifndef NDEBUG
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

        return extensions;
    }
}
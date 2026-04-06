#include <vector>
#include <LE/Common/Platform.hpp>

#if defined(LE_PLATFORM_LINUX) && !defined(LE_HEADLESS)
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/XKBlib.h>

#include <Tether/Platform/X11Window.hpp>
#define VK_USE_PLATFORM_XLIB_KHR
#elif defined(LE_PLATFORM_WINDOWS)
#ifndef UNICODE
#define UNICODE
#endif

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

// I despise the windows api
#undef ERROR

#include <Tether/Platform/Win32Window.hpp>
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#include "PlatformUtils.hpp"

namespace le
{
    void PlatformUtils::LoadVulkanFuncs(const vk::Instance instance)
    {
        VULKAN_HPP_DEFAULT_DISPATCHER.init(instance);
    }

    std::vector<const char*> PlatformUtils::GetRequiredExtensions()
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

#ifndef LE_HEADLESS
    vk::SurfaceKHR PlatformUtils::CreateSurface(const vk::Instance instance, Window& window)
    {
#ifdef LE_PLATFORM_LINUX
        auto& app = static_cast<Platform::X11Application&>(Application::Get());
        auto& windowNative = static_cast<Platform::X11Window&>(window);
        return instance.createXlibSurfaceKHR({{}, app.GetDisplay(), windowNative.GetWindowHandle()});
#elif defined(LE_PLATFORM_WINDOWS)
        auto& nativeWindow = static_cast<Tether::Platform::Win32Window&>(window);
        return instance.createWin32SurfaceKHR({{}, nativeWindow.GetHINSTANCE(), nativeWindow.GetHWND()});
#endif
    }
#endif
}

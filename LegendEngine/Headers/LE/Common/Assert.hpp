#pragma once

#include <LE/Common/Platform.hpp>
#include <LE/IO/Logger.hpp>

#ifndef NDEBUG
#ifdef LE_PLATFORM_WINDOWS
#define LE_DEBUGBREAK() __debugbreak()
#elif defined(LE_PLATFORM_MACOS) || defined(LE_PLATFORM_LINUX)
#include <signal.h>
#define LE_DEBUGBREAK() raise(SIGTRAP)
#endif

#define LE_ASSERT(check, ...) do { if (!(check)) { LE_ERROR_TRACE(__VA_ARGS__); LE_DEBUGBREAK(); } } while(0)
#else
#define LE_DEBUGBREAK() ;
#define LE_ASSERT(...) ;
#endif
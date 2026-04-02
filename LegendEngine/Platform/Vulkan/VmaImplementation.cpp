#include <LE/IO/Logger.hpp>

#ifndef NDEBUG

#include <cstdarg>
#include <cstdio>

static void VmaLog(const std::string_view format, ...)
{
	// Comment this out to see VMA logs
	return;

	va_list vaList;
	va_start(vaList, format);
	const int size = vsnprintf(nullptr, 0, format.data(), vaList);
	va_end(vaList);

	std::string result(size + 1, '\0');
	va_start(vaList, format);
	vsnprintf(result.data(), result.size(), format.data(), vaList);
	va_end(vaList);

	result.resize(size);

	LE_DEBUG("{}", result);
}

#define VMA_DEBUG_LOG_FORMAT(...) VmaLog(__VA_ARGS__)
#endif

#define VMA_IMPLEMENTATION
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 1
#define VMA_STATIC_VULKAN_FUNCTIONS 0
#include <vk_mem_alloc.h>

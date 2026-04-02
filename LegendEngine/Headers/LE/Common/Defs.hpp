#pragma once

#include <cassert>
#include <sstream>

/**
 * @brief Disables object copying.
 */
#define LE_NO_COPY(typename) \
    typename(const typename&) = delete; \
    typename(typename&&) = delete; \
    typename& operator=(const typename&) = delete; \
    typename& operator=(typename&&) = delete

#if defined(NDEBUG)
#define LEGENDENGINE_RELEASE
#else
#define LEGENDENGINE_DEBUG
#endif

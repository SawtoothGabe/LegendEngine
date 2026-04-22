cmake_minimum_required (VERSION 3.10)
project ("Legend")

# Config
option(LE_TETHER_EXTERN "Enable if tether is supplied externally" OFF)
option(LE_BUILD_TESTS "Build tests for LegendEngine" OFF)
option(LE_HEADLESS "Build LegendEngine without window capabilities (headless)" OFF)
option(LE_VULKAN_API "Build LegendEngine with Vulkan capabilities" ON)
option(LE_OPENGL_API "Build LegendEngine with OpenGL capabilities" OFF)
message(STATUS "LE_VULKAN_API = ${LE_VULKAN_API}")
message(STATUS "LE_OPENGL_API = ${LE_OPENGL_API}")
message(STATUS "LE_TETHER_EXTERN = ${LE_TETHER_EXTERN}")
message(STATUS "LE_BUILD_TESTS = ${LE_BUILD_TESTS}")
message(STATUS "LE_HEADLESS = ${LE_HEADLESS}")

# Definitions
set(TETHER_VMA_DIR "")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(TETHER_HEADLESS ${LE_HEADLESS})
set(LEGENDENGINE_BASE_DIR ${CMAKE_CURRENT_LIST_DIR})

if (NOT LE_HEADLESS)
    add_subdirectory(LegendShaders)
endif ()

define_property(GLOBAL PROPERTY LE_VULKAN_TARGET)

add_subdirectory(LegendEngine)

get_property(vulkanTarget GLOBAL PROPERTY LE_VULKAN_TARGET)
set(LE_LIBRARIES "LegendEngine LEShaders ${vulkanTarget}" CACHE INTERNAL "")

add_subdirectory(Legendary)

option(LE_VULKAN_API "Build LegendEngine with Vulkan capabilities" ON)
option(LE_OPENGL_API "Build LegendEngine with OpenGL capabilities" OFF)
message(STATUS "LE_VULKAN_API = ${LE_VULKAN_API}")
message(STATUS "LE_OPENGL_API = ${LE_OPENGL_API}")

define_property(GLOBAL PROPERTY SPV_HEADERS_DIR)

if (LE_OPENGL_API)
    file(GLOB_RECURSE LE_OPENGL_SOURCES "OpenGL/*.cpp" "OpenGL/*.hpp")
    add_library(LEOpenGL STATIC ${LE_OPENGL_SOURCES})

    target_include_directories(LEOpenGL PUBLIC "OpenGL")
    target_compile_definitions(LEOpenGL PUBLIC LE_OPENGL_API)
    target_link_libraries(LEOpenGL PUBLIC LegendEngine)
endif()

if (LE_VULKAN_API)
    find_package(Vulkan)
    if (NOT Vulkan_FOUND)
        message(FATAL_ERROR "LE_VULKAN_API is enabled, but the Vulkan SDK was not found. \nInstall it from https://vulkan.lunarg.com/, or run \"apt install libvulkan-dev vulkan-validationlayers vulkan-tools glslc\"")
    endif()

    file(GLOB_RECURSE LE_VK_SOURCES "Vulkan/*.cpp" "Vulkan/*.hpp")
    add_library(LEVulkan STATIC ${LE_VK_SOURCES})

    target_include_directories(LEVulkan PRIVATE "Vulkan" ${Vulkan_INCLUDE_DIR})
    target_compile_definitions(LEVulkan PUBLIC LE_VULKAN_API)
    target_link_libraries(LEVulkan PUBLIC LegendEngine GPUOpen::VulkanMemoryAllocator)
endif()
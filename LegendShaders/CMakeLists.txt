cmake_minimum_required (VERSION 3.15)
project ("LegendShaders")
include(FetchContent)

file(GLOB_RECURSE LESHADERS_SOURCES "Source/*.cpp" "Source/*.hpp")
add_executable(lesh ${LESHADERS_SOURCES})

set(FETCHCONTENT_QUIET FALSE)

FetchContent_Declare(simdjson GIT_REPOSITORY https://github.com/simdjson/simdjson GIT_PROGRESS TRUE)

if (MINGW)
    FetchContent_Declare(
        slang
        GIT_REPOSITORY https://github.com/shader-slang/slang
        GIT_TAG v2026.5.2
        GIT_PROGRESS TRUE
    )

    string(APPEND CMAKE_CXX_FLAGS " -include \"${CMAKE_CURRENT_LIST_DIR}/mingw_compat.h\"")

    # slang options
    set(SLANG_LIB_TYPE STATIC)
    set(SLANG_ENABLE_GFX OFF CACHE BOOL "" FORCE)
    set(SLANG_ENABLE_TESTS OFF CACHE BOOL "" FORCE)
    set(SLANG_ENABLE_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(SLANG_ENABLE_SPLIT_DEBUG_INFO OFF CACHE BOOL "" FORCE)
    set(SLANG_ENABLE_SLANGC OFF CACHE BOOL "" FORCE)
    set(SLANG_ENABLE_SLANGD OFF CACHE BOOL "" FORCE)
    set(SLANG_ENABLE_SLANGI OFF CACHE BOOL "" FORCE)
    set(SLANG_ENABLE_SLANGRT OFF CACHE BOOL "" FORCE)
    set(SLANG_ENABLE_SLANG_RHI OFF CACHE BOOL "" FORCE)
    set(CMAKE_DISABLE_PRECOMPILE_HEADERS ON)

    FetchContent_MakeAvailable(slang)
else ()
    if (WIN32)
        set(SLANG_LIB_NAME "slang.lib")
        FetchContent_Declare(
            slang
            URL https://github.com/shader-slang/slang/releases/download/v2026.5.2/slang-2026.5.2-windows-x86_64.zip
            DOWNLOAD_EXTRACT_TIMESTAMP NEW
        )
    else ()
        set(SLANG_LIB_NAME "slang.a")
        FetchContent_Declare(
            slang
            URL https://github.com/shader-slang/slang/releases/download/v2026.5.2/slang-2026.5.2-linux-x86_64.zip
            DOWNLOAD_EXTRACT_TIMESTAMP NEW
        )
    endif ()

    FetchContent_MakeAvailable(slang)

    add_library(slang STATIC IMPORTED)
    set_target_properties(slang PROPERTIES
        IMPORTED_LOCATION "${slang_SOURCE_DIR}/lib/${SLANG_LIB_NAME}"
        IMPORTED_IMPLIB "${slang_SOURCE_DIR}/lib/${SLANG_LIB_NAME}"
        INTERFACE_INCLUDE_DIRECTORIES "${slang_SOURCE_DIR}/include"
    )
endif ()

FetchContent_MakeAvailable(simdjson)

if (NOT MSVC)
    set(EXTRA_LIBS stdc++exp)
endif ()

target_include_directories(lesh PRIVATE Source)
target_link_libraries(lesh PRIVATE slang simdjson ${EXTRA_LIBS})
project ("LegendShaders")
include(FetchContent)

file(GLOB_RECURSE LESHADERS_SOURCES "Source/*.cpp" "Source/*.hpp")
add_executable(${PROJECT_NAME} ${LESHADERS_SOURCES})

FetchContent_Declare(
    slang
    GIT_REPOSITORY https://github.com/shader-slang/slang
    GIT_TAG 80b74a9f33c2a598dc37d13a87ed384f5d96f6f6
    GIT_SUBMODULES_RECURSE ON
)
FetchContent_Declare(simdjson GIT_REPOSITORY https://github.com/simdjson/simdjson)

# slang options
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(SLANG_ENABLE_IR_BREAK_ALLOC "$<$<CONFIG:Debug>:TRUE>$<$<NOT:$<CONFIG:Debug>>:FALSE>")
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo;MinSizeRel")
set(SLANG_ENABLE_PCH OFF CACHE BOOL "" FORCE)
set(SLANG_ENABLE_TESTS OFF CACHE BOOL "" FORCE)
set(SLANG_ENABLE_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SLANG_ENABLE_SPLIT_DEBUG_INFO OFF CACHE BOOL "" FORCE)
set(CMAKE_DISABLE_PRECOMPILE_HEADERS ON)

if (MINGW)
    string(APPEND CMAKE_CXX_FLAGS " -include \"${CMAKE_CURRENT_LIST_DIR}/mingw_compat.h\"")
endif ()

message(STATUS "Making dependencies for LegendShaders available")
FetchContent_MakeAvailable(slang simdjson)

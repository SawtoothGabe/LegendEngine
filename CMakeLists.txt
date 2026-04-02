cmake_minimum_required (VERSION 3.10)
project ("Legend")

# Definitions
option(LE_TETHER_EXTERN "Enable if tether is supplied externally" OFF)
option(LE_BUILD_TESTS "Build tests for LegendEngine" OFF)
option(LE_HEADLESS "Build LegendEngine without window capabilities (headless)" OFF)
message(STATUS "LE_TETHER_EXTERN = ${LE_TETHER_EXTERN}")
message(STATUS "LE_BUILD_TESTS = ${LE_BUILD_TESTS}")
message(STATUS "LE_HEADLESS = ${LE_HEADLESS}")

# Config
set(TETHER_VMA_DIR "")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(TETHER_HEADLESS ${LE_HEADLESS})

add_subdirectory(Legendary)
add_subdirectory(LegendEngine)
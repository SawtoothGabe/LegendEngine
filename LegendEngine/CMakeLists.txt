cmake_minimum_required (VERSION 3.10)
project ("LegendEngine")

include(cmake/Deps.cmake)
include(cmake/Assets.cmake)

if (LE_BUILD_TESTS)
	add_subdirectory(Tests)
endif()

file(GLOB_RECURSE LE_SOURCES "Source/*.cpp" "Headers/LE/*.hpp")
add_library(${PROJECT_NAME} STATIC ${LE_SOURCES})

if (MSVC)
	target_compile_options(${PROJECT_NAME} 
        PRIVATE
            "/wd4251;" # Disable the dll-interface warning thingy
    )
else()
	set(EXTRA_LIBS stdc++exp)
endif()

if (LE_HEADLESS)
	target_compile_definitions(${PROJECT_NAME} PUBLIC -DLE_HEADLESS)
endif()

target_include_directories(${PROJECT_NAME} PUBLIC "Headers")
target_link_libraries(${PROJECT_NAME} PUBLIC tether compile-shaders ${EXTRA_LIBS})

add_subdirectory(Platform)

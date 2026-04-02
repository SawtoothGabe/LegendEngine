cmake_minimum_required(VERSION 3.10)
project("Tests")

enable_testing()

add_subdirectory(Visual)

file(GLOB NORMAL_TEST_SOURCES "*.cpp")
add_executable(NormalTests ${NORMAL_TEST_SOURCES})

target_link_libraries(NormalTests GTest::gtest_main LegendEngine)

include(GoogleTest)
gtest_discover_tests(NormalTests)
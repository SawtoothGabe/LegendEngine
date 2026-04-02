cmake_minimum_required(VERSION 3.10)
project("VisualTests")
include(GoogleTest)

function(DeclareVisualTest name)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name}  LegendEngine)
    gtest_discover_tests(${name})
endfunction()

DeclareVisualTest(Visuals)
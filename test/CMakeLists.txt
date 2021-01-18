cmake_minimum_required(VERSION 3.15)

find_package(fmt CONFIG REQUIRED)

add_executable(basic_test "basic.cpp")
target_link_libraries(basic_test PRIVATE fmt::fmt ez-interpolate)

add_subdirectory("interactive")
add_subdirectory("approx")
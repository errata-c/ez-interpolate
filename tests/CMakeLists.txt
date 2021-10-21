cmake_minimum_required(VERSION 3.15)

find_package(fmt CONFIG REQUIRED)
find_package(rt-loader CONFIG REQUIRED COMPONENTS glew)

add_subdirectory("basic")
add_subdirectory("approx")

#add_subdirectory("interactive")
#add_subdirectory("bezier")
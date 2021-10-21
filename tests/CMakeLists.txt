cmake_minimum_required(VERSION 3.16)
project(EZ_INTERPOLATE_TESTS)

find_package(fmt CONFIG REQUIRED)

add_subdirectory("basic")

if(EZ_INTERPOLATE_BUILD_GUI_TESTS)
	add_subdirectory("approx")
endif()

#add_subdirectory("interactive")
#add_subdirectory("bezier")
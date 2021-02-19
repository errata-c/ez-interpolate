cmake_minimum_required(VERSION 3.14)

find_package(ez-imgui CONFIG REQUIRED)
find_package(ez-input CONFIG REQUIRED)
find_package(ez-math CONFIG REQUIRED)
find_package(glew CONFIG REQUIRED)

add_executable(approx_test "main.cpp" "engine.cpp")
target_link_libraries(approx_test PRIVATE 
	fmt::fmt 
	ez-interpolate 
	ez::imgui
	ez::imgui-opengl3 
	ez::input 
	ez::math
	GLEW::GLEW
)
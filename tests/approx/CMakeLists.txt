cmake_minimum_required(VERSION 3.14)

find_package(SFML COMPONENTS system window graphics CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(ImGui-SFML CONFIG REQUIRED)

find_package(ez-input CONFIG REQUIRED)
find_package(ez-math CONFIG REQUIRED)

find_package(GLEW CONFIG REQUIRED)
find_package(nanovg CONFIG REQUIRED)

add_executable(approx_test "main.cpp" "engine.cpp" "nvg.cpp")
target_link_libraries(approx_test PRIVATE 
	ImGui-SFML::ImGui-SFML
	fmt::fmt
	ez::interpolate
	ez::input
	ez::math
	GLEW::GLEW
	nanovg::nanovg
)
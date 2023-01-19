cmake_minimum_required(VERSION 3.24)

include(FetchContent)
FetchContent_Declare(
	fmt
	GIT_REPOSITORY "https://github.com/fmtlib/fmt.git"
	GIT_TAG "9.1.0"
	FIND_PACKAGE_ARGS CONFIG
)
FetchContent_Declare(
	SFML
	GIT_REPOSITORY "https://github.com/SFML/SFML.git"
	GIT_TAG "2.5.1"
	FIND_PACKAGE_ARGS COMPONENTS system window graphics CONFIG
)
FetchContent_Declare(
	imgui
	GIT_REPOSITORY "https://github.com/ocornut/imgui.git"
	GIT_TAG "v1.89.2"
	FIND_PACKAGE_ARGS CONFIG
)
FetchContent_Declare(
	ImGui-SFML
	GIT_REPOSITORY "https://github.com/eliasdaler/imgui-sfml.git"
	GIT_TAG "v2.5"
	FIND_PACKAGE_ARGS CONFIG
)
FetchContent_Declare(
	GLEW
	GIT_REPOSITORY "https://github.com/nigels-com/glew.git"
	GIT_TAG "glew-2.2.0"
	FIND_PACKAGE_ARGS CONFIG
)
FetchContent_Declare(
	ez-input
	GIT_REPOSITORY "https://github.com/errata-c/ez-input.git"
	GIT_TAG "main"
	FIND_PACKAGE_ARGS CONFIG
)
FetchContent_Declare(
	ez-math
	GIT_REPOSITORY "https://github.com/errata-c/ez-math.git"
	GIT_TAG "main"
	FIND_PACKAGE_ARGS CONFIG
)

FetchContent_MakeAvailable(fmt SFML imgui ImGui-SFML GLEW ez-input ez-math)

# Nanovg is a problem, it doesn't have a cmake build system.
find_package(nanovg CONFIG REQUIRED)

add_library(gui_test_interface STATIC
	"core/engine.cpp"
	"core/nvg.cpp"
)
target_link_libraries(gui_test_interface PUBLIC
	ImGui-SFML::ImGui-SFML
	fmt::fmt
	
	GLEW::GLEW
	nanovg::nanovg

	ez::interpolate
	ez::input
	ez::math
)
target_include_directories(gui_test_interface PUBLIC
	"core"
)


add_executable(gui_approx_test "approx.cpp")
target_link_libraries(gui_approx_test PRIVATE gui_test_interface)

add_executable(gui_split_test "split.cpp")
target_link_libraries(gui_split_test PRIVATE gui_test_interface)

add_executable(gui_through_test "through.cpp")
target_link_libraries(gui_through_test PRIVATE gui_test_interface)

add_executable(gui_bpath_test "bpath.cpp")
target_link_libraries(gui_bpath_test PRIVATE gui_test_interface)

add_executable(gui_bpath2_test "bpath2.cpp")
target_link_libraries(gui_bpath2_test PRIVATE gui_test_interface)

add_executable(gui_length_test "length.cpp")
target_link_libraries(gui_length_test PRIVATE gui_test_interface)

add_executable(gui_offset_test "offset.cpp")
target_link_libraries(gui_offset_test PRIVATE gui_test_interface)